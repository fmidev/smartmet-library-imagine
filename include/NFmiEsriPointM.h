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

#ifndef IMAGINE_NFMIESRIPOINTM_H
#define IMAGINE_NFMIESRIPOINTM_H

#include "NFmiEsriPoint.h"
#include "NFmiEsriBoxM.h"

namespace Imagine
{

  class _FMI_DLL NFmiEsriPointM : public NFmiEsriPoint
  {
  public:
	
	// Constructors, destructors
	
	~NFmiEsriPointM(void) {}
	
	NFmiEsriPointM(double theX, double theY, double theM,
				   int theNumber=0, NFmiEsriElementType theType=kFmiEsriPointM)
	  : NFmiEsriPoint(theX,theY,theNumber,theType)
	  , itsM(theM)
	{ }
	
	NFmiEsriPointM(const std::string & theBuffer, int thePos=0, int theNumber=0);
	
	// Data access
	
	double M(void) const	{ return itsM; }
	void M(double theM)	{ itsM = theM; }
	
	// Updating bounding boxes
	
	void Update(NFmiEsriBoxZ & theBox) const
	{
	  static_cast<NFmiEsriBoxM &>(theBox).Update(X(),Y(),M());
	}
	
	// String buffer size, write and string
	
	int StringSize(void) const;
	void Write(std::ostream & os) const;
	
  private:
	
	NFmiEsriPointM(void);
	
	double itsM;	// measure
  };

} // namespace Imagine
  
#endif // IMAGINE_NFMIESRIPOINTM_H
  
// ======================================================================
  
