// ======================================================================
//
// Class to define a bounding box in 3D, plus bounding box for measure
//
// History:
//
// 31.08.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef _FNMIESRIBOXZ_H
#define _FNMIESRIBOXZ_H

#include "NFmiEsriBoxM.h"

class _FMI_DLL NFmiEsriBoxZ : public NFmiEsriBoxM
{
public:
  
  // Constructors, destructors
  
  ~NFmiEsriBoxZ(void) {}
  NFmiEsriBoxZ(void) { Init(); }
  
  void Init(void)
  {
	NFmiEsriBoxM::Init();
	itsValidityZ = false;
	itsZmin = 0.0;
	itsZmax = 0.0;
  }
  
  // Data access
  
  bool IsValidZ(void) const { return itsValidityZ; }
  
  double Zmin(void) const { return itsZmin; }
  double Zmax(void) const { return itsZmax; }
  
  // Update utilities
  
  void UpdateZ(double theZ)
  {
    if(itsValidityZ)
      {
		itsZmin = MIN(itsZmin,theZ);
		itsZmax = MAX(itsZmax,theZ);
      }
    else
      {
		itsZmin = itsZmax = theZ;
		itsValidityZ = true;
      }
  }
  
  void Update(double theX, double theY, double theZ, double theM)
  {
    NFmiEsriBox::Update(theX,theY);
    NFmiEsriBoxM::UpdateM(theM);
    UpdateZ(theZ);
  }
  
  void Update(const NFmiEsriBoxZ & theBox)
  {
	NFmiEsriBoxM::Update(theBox);
	if(theBox.IsValidZ())
	  {
		UpdateZ(theBox.Zmin());
		UpdateZ(theBox.Zmax());
	  }
  }
  
private:
  
  bool itsValidityZ;
  double itsZmin;
  double itsZmax;
  
};

#endif // _FNMIESRIBOXZ_H

// ======================================================================
