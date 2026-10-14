#include "NFmiEsriTools.h"
#include "NFmiEsriShape.h"
#include <macgyver/Exception.h>
#include <newbase/NFmiStringTools.h>
#include <list>
#include <stdexcept>

using namespace std;

namespace Imagine
{
namespace NFmiEsriTools
{
// ----------------------------------------------------------------------
/*!
 * \brief Filter a shape based on a condition of form NAME<op>VALUE
 */
// ----------------------------------------------------------------------

NFmiEsriShape* filter(const NFmiEsriShape& theShape, const std::string& theCondition)
{
  try
  {
    // Parse the relevant option

    list<string> comparisons;
    comparisons.emplace_back("==");
    comparisons.emplace_back("<=");
    comparisons.emplace_back(">=");
    comparisons.emplace_back("<>");
    comparisons.emplace_back("<");
    comparisons.emplace_back(">");
    comparisons.emplace_back("=");

    string fieldname;
    string fieldvalue;
    string fieldcomparison;
    list<string> words;

    for (auto& comparison : comparisons)
    {
      string::size_type pos = theCondition.find(comparison);
      if (pos != string::npos)
      {
        fieldcomparison = comparison;
        fieldname = theCondition.substr(0, pos);
        fieldvalue = theCondition.substr(pos + comparison.size());
        break;
      }
    }

    if (fieldcomparison.empty())
      throw Fmi::Exception(BCP, "Unable to parse comparison option");

    // Fetch the attribute name

    const NFmiEsriAttributeName* name = theShape.AttributeName(fieldname);
    if (name == nullptr)
      throw Fmi::Exception(BCP, "The shape does not have a field named '" + fieldname + "'");

    // Preparse the desired field value

    const NFmiEsriAttributeType atype = name->Type();

    string svalue;
    int ivalue = 0;
    double dvalue = 0;

    switch (atype)
    {
      case kFmiEsriString:
        svalue = fieldvalue;
        break;
      case kFmiEsriInteger:
        ivalue = NFmiStringTools::Convert<int>(fieldvalue);
        break;
      case kFmiEsriDouble:
        dvalue = NFmiStringTools::Convert<double>(fieldvalue);
        break;
      default:
        throw Fmi::Exception(BCP, "The field '" + fieldname + "' is of unknown type");
    }

    auto* shape = new NFmiEsriShape(theShape.Type());

    for (auto* ait : theShape.Attributes())
    {
      shape->Add(new NFmiEsriAttributeName(*ait));
    }

    for (auto* it : theShape.Elements())
    {
      // Skip empty elements
      if (it == nullptr)
        continue;

      // Check if the value is correct

      bool ok = false;
      if (fieldcomparison == "==" || fieldcomparison == "=")
      {
        if (atype == kFmiEsriString)
          ok = (it->GetString(fieldname) == svalue);
        else if (atype == kFmiEsriInteger)
          ok = (it->GetInteger(fieldname) == ivalue);
        else
          ok = (it->GetDouble(fieldname) == dvalue);
      }
      else if (fieldcomparison == "<>")
      {
        if (atype == kFmiEsriString)
          ok = (it->GetString(fieldname) != svalue);
        else if (atype == kFmiEsriInteger)
          ok = (it->GetInteger(fieldname) != ivalue);
        else
          ok = (it->GetDouble(fieldname) != dvalue);
      }
      else if (fieldcomparison == "<")
      {
        if (atype == kFmiEsriString)
          ok = (it->GetString(fieldname) < svalue);
        else if (atype == kFmiEsriInteger)
          ok = (it->GetInteger(fieldname) < ivalue);
        else
          ok = (it->GetDouble(fieldname) < dvalue);
      }
      else if (fieldcomparison == ">")
      {
        if (atype == kFmiEsriString)
          ok = (it->GetString(fieldname) > svalue);
        else if (atype == kFmiEsriInteger)
          ok = (it->GetInteger(fieldname) > ivalue);
        else
          ok = (it->GetDouble(fieldname) > dvalue);
      }
      else if (fieldcomparison == "<=")
      {
        if (atype == kFmiEsriString)
          ok = (it->GetString(fieldname) <= svalue);
        else if (atype == kFmiEsriInteger)
          ok = (it->GetInteger(fieldname) <= ivalue);
        else
          ok = (it->GetDouble(fieldname) <= dvalue);
      }
      else if (fieldcomparison == ">=")
      {
        if (atype == kFmiEsriString)
          ok = (it->GetString(fieldname) >= svalue);
        else if (atype == kFmiEsriInteger)
          ok = (it->GetInteger(fieldname) >= ivalue);
        else
          ok = (it->GetDouble(fieldname) >= dvalue);
      }

      if (!ok)
        continue;

      NFmiEsriElement* tmp = it->Clone();
      shape->Add(tmp);
    }

    return shape;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

}  // namespace NFmiEsriTools
}  // namespace Imagine
