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

#ifndef IMAGINE_NFMIESRIBOXZ_H
#define IMAGINE_NFMIESRIBOXZ_H

#include "NFmiEsriBoxM.h"

namespace Imagine
{

  class _FMI_DLL NFmiEsriBoxZ : public NFmiEsriBoxM
  {
  public:
	
	// Constructors, destructors
	
	~NFmiEsriBoxZ(void) {}
	NFmiEsriBoxZ(void)
	  : NFmiEsriBoxM()
	  , itsValidityZ(false)
	  , itsZmin(0.0)
	  , itsZmax(0.0)
	{ }
	
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

} // namespace Imagine
  
#endif // IMAGINE_NFMIESRIBOXZ_H
  
// ======================================================================
  
