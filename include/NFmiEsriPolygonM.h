// ======================================================================
//
// Esri Shapefile Techinical Description, page 13
//
//
// Position	Field	Value	Type	Number	Endian
//
// Byte 0	Type	5	int	1	little
// Byte 4	Box	Box	double	4	little
// Byte 36	NParts	NParts	int	1	little
// Byte 40	NPoints	NPoints	int	1	little
// Byte 44	Parts	Parts	int	NParts	little
// Byte X	Points	Points	point	NPoints	little
// Byte Y*	Mmin	Mmin	double	1	little
// Byte Y+8*	Mmax	Mmax	double	1	little
// Byte Y+16*	Marray	Marray	double	NPoints	little
//
// ======================================================================

#ifndef _NFMIESRIPOLYGONM_H
#define _NFMIESRIPOLYGONM_H

#include "NFmiEsriPointM.h"
#include "NFmiEsriPolygon.h"
#include "NFmiEsriBoxM.h"

class _FMI_DLL NFmiEsriPolygonM : public NFmiEsriPolygon
{
public:
  
  // Constructors, destructors
  
  ~NFmiEsriPolygonM(void) {}
  
  NFmiEsriPolygonM(int theNumber=0,NFmiEsriElementType theType=kFmiEsriPolygonM)
    : NFmiEsriPolygon(theNumber,kFmiEsriPolygonM)
  {}
  
  NFmiEsriPolygonM(const std::string & theBuffer, int thePos=0, int theNumber=0);
  
  // Data access
  
  const NFmiEsriBoxM & Box(void) const	{ return itsBox; }
  
  int NumPoints(void) const		{ return itsPoints.size(); }
  int NumParts(void) const		{ return itsParts.size(); }
  
  const std::vector<int> & Parts(void) const 		{ return itsParts; }
  const std::vector<NFmiEsriPointM> & Points(void) const	{ return itsPoints; }
  
  // This is intended to be used by projection etc methods
  
  void Points(const std::vector<NFmiEsriPointM> & pts) { itsPoints = pts; }
  
  // Adding a new data point to the current part, or the first
  // one if this is the first point
  
  void Add(const NFmiEsriPointM & thePoint)
  {
    itsPoints.push_back(thePoint);
    itsBox.Update(thePoint.X(),thePoint.Y(),thePoint.M());
    if(NumParts()==0)			// user should have used AddPart,
      itsParts.push_back(0);		// this will fix things
  }
  
  // Add a new data point and a new part
  
  void AddPart(const NFmiEsriPointM & thePoint)
  {
    itsParts.push_back(NumPoints());	// index of next free location
    Add(thePoint);
  }
  
  // Updating bounding boxes
  
  void Update(NFmiEsriBoxZ & theBox) const
  {
    static_cast<NFmiEsriBoxM &>(theBox).Update(itsBox);
  }
  
  // String buffer size, write and string
  
  int StringSize(void) const;
  void Write(std::ostream & os) const;
  
private:
  
  NFmiEsriBoxM	itsBox;		// Bounding Box, xmin,ymin,xmax,ymax
  // int	itsNumParts;	// Number of parts = size of itsParts
  // int	itsNumPoints;	// Number of points = size of itsPoints
  
  std::vector<int>			itsParts;	// Index to first point in part
  std::vector<NFmiEsriPointM>	itsPoints;	// Points for all parts
  
};

#endif // _NFMIESRIPOLYGONM_H

// ======================================================================
