// ======================================================================
//
// Abstract base class for shapefile ESRI element types
// and their attributes.
//
// ======================================================================

#ifndef _NFMIESRIELEMENT_H
#define _NFMIESRIELEMENT_H

#include <string>
#include <vector>
#include <list>
#include <iostream>

#include "NFmiEsriProjector.h"
#include "NFmiEsriAttribute.h"
#include "NFmiEsriBuffer.h"

enum NFmiEsriElementType { kFmiEsriNull		= 0,
						   kFmiEsriPoint	= 1,
						   kFmiEsriPolyLine	= 3,
						   kFmiEsriPolygon	= 5,
						   kFmiEsriMultiPoint	= 8,
						   kFmiEsriPointZ	= 11,
						   kFmiEsriPolyLineZ	= 13,
						   kFmiEsriPolygonZ	= 15,
						   kFmiEsriMultiPointZ	= 18,
						   kFmiEsriPointM	= 21,
						   kFmiEsriPolyLineM	= 23,
						   kFmiEsriPolygonM	= 25,
						   kFmiEsriMultiPointM	= 28,
						   kFmiEsriMultiPatch	= 31 };

class NFmiEsriBoxZ;

class NFmiEsriElement
{
public:
  
  // Tulostusoperaattori ylikuirmitetaan
  
  friend std::ostream & operator<<(std::ostream & os,
								   const NFmiEsriElement & theElement);
  
  virtual ~NFmiEsriElement(void) {}
  
  NFmiEsriElement(NFmiEsriElementType theType, int theNumber=0)
    : itsType(theType), itsNumber(theNumber)
  {}
  
  // Adding an attribute
  
  void Add(const NFmiEsriAttribute & theAttribute)
  {
    itsAttributes.push_back(theAttribute);
  }
  
  // Returning an attribute value
  
  std::string GetString(const std::string & theName) const;
  int GetInteger(const std::string & theName) const;
  double GetDouble(const std::string & theName) const;
  
  // Update given bounding box
  
  virtual void Update(NFmiEsriBoxZ & theBox) const {}
  
  // Write element as character buffer
  
  virtual void Write(std::ostream & os) const {}
  
  // Return only the size the character buffer would take
  
  virtual int StringSize(void) const { return 0; }
  
  // Data access
  
  virtual NFmiEsriElementType Type(void) const	{ return itsType; }
  virtual int Number(void) const		{ return itsNumber; }
  
  virtual double X(void) const { return -999.0; }
  virtual double Y(void) const { return -999.0; }
  virtual double Z(void) const { return -999.0; }
  virtual double M(void) const { return -999.0; }
  
  virtual int NumParts(void) const { return -1; }
  virtual int NumPoints(void) const { return -1; }
  
  // Projection
  
  virtual void Project(const NFmiEsriProjector & theProjector) {}
  
protected:
  
  NFmiEsriElement(void) {}
  
  NFmiEsriElementType		itsType;
  
  int				itsNumber;
  std::list<NFmiEsriAttribute>	itsAttributes;
  
};

#endif // _NFMIESRIELEMENT_H

// ======================================================================
