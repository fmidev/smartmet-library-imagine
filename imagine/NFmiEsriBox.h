// ======================================================================
//
// Class to define a bounding box in 2D, plus bounding box for measure
//
// History:
//
// 31.08.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#pragma once

#include <newbase/NFmiGlobals.h>

namespace Imagine
{
class NFmiEsriBox
{
 public:
  // Constructors, destructors

  NFmiEsriBox();

#ifdef NO_COMPILER_OPTIMIZED
  ~NFmiEsriBox(void);
  NFmiEsriBox(const NFmiEsriBox& theBox);
  NFmiEsriBox& operator=(const NFmiEsriBox& theBox);
#endif

  void Init();

  // Data access

  bool IsValid() const { return itsValidity; }
  double Xmin() const { return itsXmin; }
  double Xmax() const { return itsXmax; }
  double Ymin() const { return itsYmin; }
  double Ymax() const { return itsYmax; }
  double Mmin() const { return itsMmin; }
  double Mmax() const { return itsMmax; }
  double Zmin() const { return itsZmin; }
  double Zmax() const { return itsZmax; }
  // Update utilities

  void Update(double theX, double theY, double theM, double theZ);
  void Update(double theX, double theY, double theM);
  void Update(double theX, double theY);

  void Update(const NFmiEsriBox& theBox);

 private:
  bool itsValidity;
  double itsXmin;
  double itsXmax;
  double itsYmin;
  double itsYmax;
  double itsMmin;
  double itsMmax;
  double itsZmin;
  double itsZmax;
};

}  // namespace Imagine

// ======================================================================
