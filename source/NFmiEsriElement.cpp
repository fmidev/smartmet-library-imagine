// ======================================================================
//
// Abstract base class for shapefile ESRI element types
// and their attributes.
//
// ======================================================================

#ifdef OLD_MSC
#pragma warning(disable : 4786)
#endif

#include "NFmiEsriElement.h"

using namespace std;

namespace Imagine
{

  // ----------------------------------------------------------------------
  // Return string value of attribute
  // ----------------------------------------------------------------------
  
  std::string NFmiEsriElement::GetString(const string &  theName) const
  {
	list<NFmiEsriAttribute>::const_iterator begin = itsAttributes.begin();
	list<NFmiEsriAttribute>::const_iterator end   = itsAttributes.end();
	list<NFmiEsriAttribute>::const_iterator iter;
	
	for(iter=begin; iter!=end; ++iter)
	  {
		if( (*iter).GetName() == theName )
		  if( (*iter).GetType() == kFmiEsriString)
			return (*iter).GetString();
	  }
	
	// Return empty string if field not found
	
	static const string tmp = "";
	return tmp;
	
  }
  
  // ----------------------------------------------------------------------
  // Return integer value of attribute
  // ----------------------------------------------------------------------
  
  int NFmiEsriElement::GetInteger(const string &  theName) const
  {
	list<NFmiEsriAttribute>::const_iterator begin = itsAttributes.begin();
	list<NFmiEsriAttribute>::const_iterator end   = itsAttributes.end();
	list<NFmiEsriAttribute>::const_iterator iter;
	
	for(iter=begin; iter!=end; ++iter)
	  {
		if( (*iter).GetName() == theName )
		  if( (*iter).GetType() == kFmiEsriInteger)
			return (*iter).GetInteger();
	  }
	
	// Maybe should error instead..
	
	return 0;
	
  }
  
  // ----------------------------------------------------------------------
  // Return double value of attribute
  // ----------------------------------------------------------------------
  
  double NFmiEsriElement::GetDouble(const string &  theName) const
  {
	list<NFmiEsriAttribute>::const_iterator begin = itsAttributes.begin();
	list<NFmiEsriAttribute>::const_iterator end   = itsAttributes.end();
	list<NFmiEsriAttribute>::const_iterator iter;
	
	for(iter=begin; iter!=end; ++iter)
	  {
		if( (*iter).GetName() == theName )
		  if( (*iter).GetType() == kFmiEsriDouble)
			return (*iter).GetDouble();
	  }
	
	// Maybe should error instead..
	
	return 0.0;
	
  }

} // namespace Imagine


std::ostream & operator<<(std::ostream & os, const Imagine::NFmiEsriElement & theElement)
{
  return theElement.Write(os);
}
  
// ----------------------------------------------------------------------
  
