// ======================================================================
//
// Abstract base class for shapefile ESRI element types
// and their attributes.
//
// ======================================================================
#if (defined _MSC_VER) && (_MSC_VER < 1310) // uusi MSVC 7.1 (1310 eli ver 13.1 eli MSVC++ 7.1) hoitaa homman ilman pragmoja
#pragma warning(disable : 4786) // poistaa n kpl VC++ kääntäjän varoitusta (liian pitkä nimi >255 merkkiä joka johtuu 'puretuista' STL-template nimistä)
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
  
