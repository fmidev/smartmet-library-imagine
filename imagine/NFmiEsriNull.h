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

#pragma once

#include "NFmiEsriBox.h"
#include "NFmiEsriBuffer.h"
#include "NFmiEsriElement.h"

namespace Imagine
{
using namespace NFmiEsriBuffer;  // Conversion tools

class NFmiEsriNull : public NFmiEsriElement
{
 public:
  // Constructors, destructors

  ~NFmiEsriNull() override {}
  NFmiEsriNull(const NFmiEsriNull& theNull) : NFmiEsriElement(theNull) {}
  NFmiEsriNull(int theNumber = 0) : NFmiEsriElement(kFmiEsriNull, theNumber) {}
  // Copying

  NFmiEsriNull& operator=(const NFmiEsriNull& theNull)
  {
    if (this != &theNull)
      NFmiEsriElement::operator=(theNull);
    return *this;
  }

  NFmiEsriElement* Clone() const override { return new NFmiEsriNull(*this); }
  // Updating bounding boxes

  void Update(NFmiEsriBox& theBox) const override {}
  void Project(const NFmiEsriProjector& theProjector) override {}
  // Writing string buffer

  std::ostream& Write(std::ostream& os) const override
  {
    os << LittleEndianInt(kFmiEsriNull);
    return os;
  }

  // Conversion to string buffer

  const std::string ToString() const { return LittleEndianInt(kFmiEsriNull); }
  // Calculating string buffer size

  int StringSize() const override { return 4; }  // the type takes 4
};

}  // namespace Imagine

// ======================================================================
