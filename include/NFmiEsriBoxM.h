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

#ifndef _FNMIESRIBOXM_H
#define _FNMIESRIBOXM_H

#include "NFmiEsriBox.h"

class _FMI_DLL NFmiEsriBoxM : public NFmiEsriBox
{
public:
  
  // Constructors, destructors
  
  ~NFmiEsriBoxM(void) {}
  
  NFmiEsriBoxM(void) { Init(); }
  
  void Init(void)
  {
	NFmiEsriBox::Init();
	itsValidityM = false;
	itsMmin = 0.0;
	itsMmax = 0.0;
  }
  
  // Data access
  
  bool IsValidM(void) const { return itsValidityM; }
  
  double Mmin(void) const { return itsMmin; }
  double Mmax(void) const { return itsMmax; }
  
  // Update utilities
  
  void UpdateM(double theM)
  {
    if(itsValidityM)
      {
		itsMmin = MIN(itsMmin,theM);
		itsMmax = MAX(itsMmax,theM);
      }
    else
      {
		itsMmin = itsMmax = theM;
		itsValidityM = true;
      }
  }
  
  void Update(double theX, double theY, double theM)
  {
    UpdateM(theM);
    NFmiEsriBox::Update(theX,theY);
  }
  
  void Update(const NFmiEsriBoxM & theBox)
  {
    NFmiEsriBox::Update(theBox);
    if(theBox.IsValidM())
      {
		UpdateM(theBox.Mmin());
		UpdateM(theBox.Mmax());
      }
  }
  
private:
  
  bool itsValidityM;
  double itsMmin;
  double itsMmax;
  
};

#endif // _FNMIESRIBOX_H

// ======================================================================
