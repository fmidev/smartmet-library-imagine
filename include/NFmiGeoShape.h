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

#ifndef IMAGINE_NFMIGEOSHAPE_H
#define IMAGINE_NFMIGEOSHAPE_H

#include "NFmiArea.h"
#include "NFmiEsriShape.h"
#include "NFmiPath.h"
#include "NFmiFillMap.h"
#include <string>
#include <stdexcept>

namespace Imagine
{

  // Implemented shape types

  enum NFmiGeoShapeType
	{
	  kFmiGeoShapeEsri,			// ESRI shapefiles (.shp,.shx,.dbf)
	  kFmiGeoShapeShoreLine,	// GMT shoreline databases
	  kFmiGeoShapeGMT			// GMT type ASCII databases
	};

  //! Generic shape error
  struct NFmiGeoShapeError : public std::runtime_error
  {
	NFmiGeoShapeError(const std::string & s) : std::runtime_error(s) { }
  };
  
  class _FMI_DLL NFmiGeoShape : public NFmiDrawable
  
  {
  public:
	
	// Constructor
  NFmiGeoShape(void)
	:itsType(kFmiGeoShapeEsri)
	,itsEsriShape(0)
  {}

	
	NFmiGeoShape(const std::string & theFilename,
				 NFmiGeoShapeType theType = kFmiGeoShapeEsri)
	  : itsType(theType)
	{
	  Read(theFilename, theType);
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
	
  void Read(const std::string & theFilename, NFmiGeoShapeType theType)
  {
	delete itsEsriShape;
	itsEsriShape = 0;
    itsType = theType;
    switch(itsType)
      {
      case kFmiGeoShapeEsri:
		itsEsriShape = new NFmiEsriShape();
		if(!itsEsriShape->Read(theFilename))
		  throw NFmiGeoShapeError(std::string("Failed to read shape ")+theFilename);
		break;
      case kFmiGeoShapeShoreLine:
		throw NFmiGeoShapeError("kFmiGeoShapeShoreLine not implemented");
      case kFmiGeoShapeGMT:
		throw NFmiGeoShapeError("kFmiGeoShapeFMT not implemented");
      }
  }

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

} // namespace Imagine
  
#endif
  
// ======================================================================
