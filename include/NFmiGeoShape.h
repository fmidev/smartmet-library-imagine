// ======================================================================
//
// Generic map vector data (point,polyline,polygon) container and
// renderer.
//
// History:
//
// 25.09.2001 Mika Heiskanen
//
//	Implemented ESRI mapdata renderer based on libesri
//
// ======================================================================

#ifndef _NFMIGEOSHAPE_H
#define _NFMIGEOSHAPE_H

#include "NFmiArea.h"
#include "NFmiEsriShape.h"
#include "NFmiPath.h"
#include "NFmiFillMap.h"

// Implemented shape types

enum NFmiGeoShapeType
{
  kFmiGeoShapeEsri,		// ESRI shapefiles (.shp,.shx,.dbf)
  kFmiGeoShapeShoreLine,	// GMT shoreline databases
  kFmiGeoShapeGMT		// GMT type ASCII databases
};

class NFmiGeoShape : public NFmiDrawable
{
public:

  // Constructor

	NFmiGeoShape(const std::string & theFilename,
	       NFmiGeoShapeType theType = kFmiGeoShapeEsri)
    : itsType(theType)
  {
    switch(theType)
      {
      case kFmiGeoShapeEsri:
	itsEsriShape = new NFmiEsriShape();
	if(!itsEsriShape->Read(theFilename))
	  {
		std::cerr << "Error: Failed to read shape " << theFilename << std::endl; // 2.1.2002/Marko Windows ohjelmat eivät tykkää cerr:istä muistaakseni, pitää kokeilla.
	    exit(1); // 2.1.2002/Marko Tämä on ikävä tapa hoitaa virhetilanteet.
	  }
	break;
      case kFmiGeoShapeShoreLine:
		  std::cerr << "Error: NFmiGeoShape kFmiGeoShapeShoreLine not implemented" << std::endl;
	exit(1);
	break;
      case kFmiGeoShapeGMT:
		  std::cerr << "Error: NFmiGeoShape kFmiGeoShapeGMT not implemented" << std::endl;
	exit(1);
	break;
      }
  }

  // Destructor

  virtual ~NFmiGeoShape(void)
  {
    if(itsEsriShape!=NULL) itsEsriShape->~NFmiEsriShape();
  }

  // Data-access

  NFmiGeoShapeType Type(void) const	{ return itsType; }

  // Project the data

  void ProjectXY(const NFmiArea & theArea);

  // Create a path from the map data

  NFmiPath Path(void) const;

  // Add the data to a fill map

  void Add(NFmiFillMap & theMap) const;

  // Stroke onto given image using various Porter-Duff rules

  void Stroke(NFmiImage & theImage,
	      NFmiColorTools::Color theColor,
	      NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const;

  // Mark the coordinates

  void Mark(NFmiImage & theImage,
	    const NFmiImage & theMarker,
	    NFmiColorTools::NFmiBlendRule theRule,
	    NFmiAlignment theAlignment = kFmiAlignCenter,
	    float theAlpha=1.0) const;

  // Write imagemap data to a file

  void WriteImageMap(std::ostream & os, const std::string & theFieldName) const;

private:

  // Path creation

  NFmiPath PathEsri(void) const;

  // Stroking

  void StrokeEsri(NFmiImage & theImage,
		  NFmiColorTools::Color theColor,
		  NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const;

  // Adding to a fillmap

  void AddEsri(NFmiFillMap & theMap) const;

  // Mark the coordinates

  void MarkEsri(NFmiImage & theImage,
		const NFmiImage & theMarker,
		NFmiColorTools::NFmiBlendRule theRule,
		NFmiAlignment theAlignment = kFmiAlignCenter,
		float theAlpha=1.0) const;

  // Write imagemap data to a file

  void WriteImageMapEsri(std::ostream & os, const std::string & theFieldName) const;

  // Data-part uses pointers, so that we do not have to initialize
  // all different map data elements, since only one can be used.

  NFmiGeoShapeType	itsType;
  NFmiEsriShape		*itsEsriShape;
  
};

#endif

// ======================================================================
