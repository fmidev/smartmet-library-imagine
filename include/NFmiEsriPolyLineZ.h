// ======================================================================
//
// Esri Shapefile Techinical Description, page 7
//
//
// Position	Field	Value	Type	Number	Endian
//
// Byte 0	Type	13	int	1	little
// Byte 4	Box	Box	double	4	little
// Byte 36	NParts	NParts	int	1	little
// Byte 40	NPoints	NPoints	int	1	little
// Byte 44	Parts	Parts	int	NParts	little
// Byte X	Points	Points	point	NPoints	little
// Byte Y	Zmin	Zmin	double	1	little
// Byte Y+8	Zmax	Zmax	double	1	little
// Byte Y+16	Zarray	Zarray	double	NPoints	little
// Byte Z*	Mmin	Mmin	double	1	little
// Byte Z+8*	Mmax	Mmax	double	1	little
// Byte Z+16*	Marray	Marray	double	NPoints	little
//
// Note: X = 44 + 4 * NumParts
//       Y = X + 16 * NumPoints
//       Z = Y + 16 + 8 * NumPoints
//
// ======================================================================

#ifndef _NFMIESRIPOLYLINEZ_H
#define _NFMIESRIPOLYLINEZ_H

#include "NFmiEsriPointZ.h"
#include "NFmiEsriBoxZ.h"
#include "NFmiEsriPolyLineM.h"

class NFmiEsriPolyLineZ : public NFmiEsriPolyLineM
{

public:

  // Constructors, destructors

  ~NFmiEsriPolyLineZ(void) {}

  NFmiEsriPolyLineZ(int theNumber=0,NFmiEsriElementType theType=kFmiEsriPolyLineZ)
    : NFmiEsriPolyLineM(theNumber,theType)
  {}

  NFmiEsriPolyLineZ(const std::string & theBuffer, int thePos=0, int theNumber=0);

  // Data access

  const NFmiEsriBoxZ & Box(void) const	{ return itsBox; }

  int NumPoints(void) const		{ return itsPoints.size(); }
  int NumParts(void) const		{ return itsParts.size(); }

  const std::vector<int> & Parts(void) const		 	{ return itsParts; }
  const std::vector<NFmiEsriPointZ> & Points(void) const	{ return itsPoints; }

  // This is intended to be used by projection etc methods

  void Points(const std::vector<NFmiEsriPointZ> & pts) { itsPoints = pts; }

  // Adding a new data point to the current part, or the first
  // one if this is the first point

  void Add(const NFmiEsriPointZ & thePoint)
  {
    itsPoints.push_back(thePoint);
    itsBox.Update(thePoint.X(),thePoint.Y(),thePoint.Z(),thePoint.M());
    if(NumParts()==0)			// user should have used AddPart,
      itsParts.push_back(0);		// this will fix things
  }

  // Add a new data point and a new part

  void AddPart(const NFmiEsriPointZ & thePoint)
  {
    itsParts.push_back(NumPoints());	// index of next free location
    Add(thePoint);
  }

  // Updating bounding boxes
  
  void Update(NFmiEsriBoxZ & theBox) const
  {
    theBox.Update(itsBox);
  }

  // String buffer size, write and string

  int StringSize(void) const;
  void Write(std::ostream & os) const;

private:

  NFmiEsriBoxZ	itsBox;		// Bounding Box, xmin,ymin,xmax,ymax
  // int	itsNumParts;	// Number of parts = size of itsParts
  // int	itsNumPoints;	// Number of points = size of itsPoints

  std::vector<int>			itsParts;	// Index to first point in part
  std::vector<NFmiEsriPointZ>	itsPoints;	// Points for all parts

};

#endif // _NFMIESRIPOLYLINEZ_H

// ======================================================================
