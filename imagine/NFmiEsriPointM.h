// ======================================================================
//
// Esri Shapefile Techinical Description, page 10
//
//
// Position	Field	Value	Type	Number	Endian
//
// Byte 0	Type	21	int	1	little
// Byte 4	X	X	double	1	little
// Byte 12	Y	Y	double	1	little
// Byte 20	M	M	double	1	little
//
// ======================================================================

#pragma once

#include "NFmiEsriBox.h"
#include "NFmiEsriPoint.h"

namespace Imagine
{
class NFmiEsriPointM : public NFmiEsriPoint
{
 public:
  // Constructors, destructors

  ~NFmiEsriPointM() override {}
  NFmiEsriPointM(const NFmiEsriPointM& thePoint);

  NFmiEsriPointM(double theX,
                 double theY,
                 double theM,
                 int theNumber = 0,
                 NFmiEsriElementType theType = kFmiEsriPointM)
      : NFmiEsriPoint(theX, theY, theNumber, theType), itsM(theM)
  {
  }

  NFmiEsriPointM(const std::string& theBuffer, int thePos = 0, int theNumber = 0);

  // Copying

  NFmiEsriPointM& operator=(const NFmiEsriPointM& thePoint);

  NFmiEsriElement* Clone() const override;

  // Data access

  double M() const override { return itsM; }
  void M(double theM) { itsM = theM; }
  // Updating bounding boxes

  void Update(NFmiEsriBox& theBox) const override { theBox.Update(X(), Y(), M()); }
  // String buffer size, write and string

  int StringSize() const override;
  std::ostream& Write(std::ostream& os) const override;

 private:
  NFmiEsriPointM();

  double itsM;  // measure
};

}  // namespace Imagine

// ======================================================================
