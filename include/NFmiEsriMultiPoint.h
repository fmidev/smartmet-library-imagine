// ======================================================================
//
// Esri Shapefile Techinical Description, page 6
//
//
// Position	Field	Value	Type	Number	Endian
//
// Byte 0	Type	8	int	1	little
// Byte 4	Box	Box	double	4	little
// Byte 36	NPoint	NPoints	int	1	little
// Byte 40	Points	Points	Point	NPoints	little
//
// ======================================================================

#ifndef _NFMIESRIMULTIPOINT_H
#define _NFMIESRIMULTIPOINT_H

#include <vector>

#include "NFmiEsriPoint.h"
#include "NFmiEsriBox.h"

class NFmiEsriMultiPoint : public NFmiEsriElement
{
public:
  
  // Constructors, destructors
  
  ~NFmiEsriMultiPoint(void) {}
  
  NFmiEsriMultiPoint(int theNumber=0,
					 NFmiEsriElementType theType=kFmiEsriMultiPoint)
    : NFmiEsriElement(theType,theNumber)
  {}
  
  NFmiEsriMultiPoint(const std::string & theBuffer, int thePos=0, int theNumber=0);
  
  // Data access
  
  const NFmiEsriBox & Box(void) const		{ return itsBox; }
  int NumPoints(void) const			{ return itsPoints.size(); }
  const std::vector<NFmiEsriPoint> & Points(void) const	{ return itsPoints; }
  
  // This is intended to be used by projection etc methods
  
  void Points(const std::vector<NFmiEsriPoint> & pts) { itsPoints = pts; }
  
  // Adding a point
  
  void Add(const NFmiEsriPoint & thePoint)
  {
    itsPoints.push_back(thePoint);
    itsBox.Update(thePoint.X(),thePoint.Y());
  }
  
  // Updating bounding boxes
  
  void Update(NFmiEsriBoxZ & theBox) const
  {
    static_cast<NFmiEsriBox &>(theBox).Update(itsBox);
  }
  
  // String buffer size, write and string
  
  int StringSize(void) const;
  void Write(std::ostream & os) const;
  
  // Projection
  
  void Project(const NFmiEsriProjector & theProjector);
  
private:
  
  NFmiEsriBox	itsBox;		// Bounding box
  
  // int	itsNumPoints;	// Number of points = size of itsPoints
  
  std::vector<NFmiEsriPoint>	itsPoints;
};

#endif // _NFMIESRIMULTIPOINT_H

// ======================================================================
