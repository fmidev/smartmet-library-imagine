// ======================================================================
//
// Esri Shapefile Techinical Description, page 5
//
//
// Position	Field	Value	Type	Number	Endian
//
// 0		Type	0	int	1	little
//
// ======================================================================

#ifndef _NFMIESRINULL_H
#define _NFMIESRINULL_H

#include "NFmiEsriElement.h"
#include "NFmiEsriBox.h"

using namespace NFmiEsriBuffer;	// Conversion tools

class NFmiEsriNull : public NFmiEsriElement
{
public:
  
  // Constructors, destructors
  
  ~NFmiEsriNull(void) {}
  
  NFmiEsriNull(int theNumber=0)
    : NFmiEsriElement(kFmiEsriNull,theNumber)
  {}
  
  // Updating bounding boxes
  
  void Update(NFmiEsriBox & theBox) const
  {
  }
  
  // Writing string buffer
  
  void Write(std::ostream & os) const
  {
    os << LittleEndianInt(kFmiEsriNull);
  }
  
  // Conversion to string buffer
  
  std::string ToString(void) const
  {
    return LittleEndianInt(kFmiEsriNull);
  }
  
  // Calculating string buffer size
  
  int StringSize(void) const	{ return 4; }	// the type takes 4
  
};

#endif // _NFMIESRINULL_H

// ======================================================================
