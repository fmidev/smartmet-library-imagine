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

#pragma once

#include "NFmiEsriShape.h"
#include "NFmiEsriTools.h"
#include "NFmiPath.h"

#include "NFmiColorTools.h"
#include "NFmiDrawable.h"
#include "NFmiFillMap.h"
#include "NFmiImage.h"

#include <newbase/NFmiArea.h>

#include <stdexcept>

namespace Imagine
{
// Implemented shape types

enum NFmiGeoShapeType
{
  kFmiGeoShapeEsri,       // ESRI shapefiles (.shp,.shx,.dbf)
  kFmiGeoShapeShoreLine,  // GMT shoreline databases
  kFmiGeoShapeGMT         // GMT type ASCII databases
};

//! Generic shape error
#if 0
// koskelam: Replaced by Fmi::Exception

struct NFmiGeoShapeError : public std::runtime_error
{
  NFmiGeoShapeError(const std::string &s) : std::runtime_error(s) {}
};
#endif

class NFmiGeoShape
    : public NFmiDrawable
{
 public:
  // Constructor
  NFmiGeoShape() : itsType(kFmiGeoShapeEsri), itsEsriShape(nullptr) {}
  NFmiGeoShape(const std::string& theFilename,
               NFmiGeoShapeType theType = kFmiGeoShapeEsri,
               const std::string& theFilter = "")
      : itsType(theType), itsEsriShape(nullptr)
  {
    Read(theFilename, theType, theFilter);
  }

  // Destructor

  ~NFmiGeoShape() override { delete itsEsriShape; }
  NFmiGeoShape(const NFmiGeoShape&) = delete;
  NFmiGeoShape& operator=(const NFmiGeoShape&) = delete;
  // Data-access

  NFmiGeoShapeType Type() const { return itsType; }
  // Project the data

  void ProjectXY(NFmiArea &theArea);

  // Create a path from the map data

  const NFmiPath Path() const;

// Add the data to a fill map
  void Add(NFmiFillMap& theMap) const override;

  // Stroke onto given image using various Porter-Duff rules

  void Stroke(NFmiImage& theImage,
              NFmiColorTools::Color theColor,
              NFmiColorTools::NFmiBlendRule theRule = NFmiColorTools::kFmiColorCopy) const override;

  // Mark the coordinates

  void Mark(NFmiImage &theImage,
            const NFmiImage &theMarker,
            NFmiColorTools::NFmiBlendRule theRule,
            NFmiAlignment theAlignment = kFmiAlignCenter,
            float theAlpha = 1.0) const;

  // Write imagemap data to a file

  void WriteImageMap(std::ostream &os, const std::string &theFieldName) const;

  void Read(const std::string &theFilename, NFmiGeoShapeType theType, const std::string &theFilter)
  {
    delete itsEsriShape;
    itsEsriShape = nullptr;
    itsType = theType;
    switch (itsType)
    {
      case kFmiGeoShapeEsri:
        itsEsriShape = new NFmiEsriShape();
        if (!itsEsriShape->Read(theFilename))
          throw Fmi::Exception(BCP, std::string("Failed to read shape ") + theFilename);

        if (!theFilter.empty())
        {
          NFmiEsriShape *tmp = NFmiEsriTools::filter(*itsEsriShape, theFilter);
          delete itsEsriShape;
          itsEsriShape = tmp;
        }

        break;
      case kFmiGeoShapeShoreLine:
        throw Fmi::Exception(BCP, "kFmiGeoShapeShoreLine not implemented");
      case kFmiGeoShapeGMT:
        throw Fmi::Exception(BCP, "kFmiGeoShapeFMT not implemented");
    }
  }

  using NFmiDrawable::Fill;
  void Fill(Imagine::NFmiImage& image,
            Imagine::NFmiColorTools::Color col,
            NFmiColorTools::NFmiBlendRule rule) const override
  {
    PathEsri().Fill(image, col, rule);
  }

 private:
  // Path creation

  const NFmiPath PathEsri() const;

  // Stroking

  void StrokeEsri(Imagine::NFmiImage &theImage,
                  NFmiColorTools::Color theColor,
                  NFmiColorTools::NFmiBlendRule theRule = NFmiColorTools::kFmiColorCopy) const;

// Adding to a fillmap
  void AddEsri(NFmiFillMap &theMap) const;

  // Mark the coordinates

  void MarkEsri(Imagine::NFmiImage &theImage,
                const Imagine::NFmiImage &marker,
                NFmiColorTools::NFmiBlendRule theRule,
                NFmiAlignment theAlignment = kFmiAlignCenter,
                float theAlpha = 1.0) const;

  // Write imagemap data to a file

  void WriteImageMapEsri(std::ostream &os, const std::string &theFieldName) const;

  // Data-part uses pointers, so that we do not have to initialize
  // all different map data elements, since only one can be used.

  NFmiGeoShapeType itsType;
  NFmiEsriShape *itsEsriShape;
};

}  // namespace Imagine

// ======================================================================
