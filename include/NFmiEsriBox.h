// ======================================================================
//
// Class to define a bounding box in 2D
//
// History:
//
// 31.08.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef _FNMIESRIBOX_H
#define _FNMIESRIBOX_H

#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b) ? (a) : (b))
#endif

class _FMI_DLL NFmiEsriBox
{
public:
  
  // Constructors, destructors
  
  virtual ~NFmiEsriBox(void) {}
  
  NFmiEsriBox(void)
    : itsValidity(false)
	, itsXmin(0.0)
	, itsXmax(0.0)
	, itsYmin(0.0)
	, itsYmax(0.0)
 { }
  
  void Init(void)
  {
    itsValidity = false;
    itsXmin = 0.0;
    itsXmax = 0.0;
    itsYmin = 0.0;
    itsYmax = 0.0;
  }
  
  // Data access
  
  bool IsValid(void) const { return itsValidity; }
  
  double Xmin(void) const { return itsXmin; }
  double Xmax(void) const { return itsXmax; }
  double Ymin(void) const { return itsYmin; }
  double Ymax(void) const { return itsYmax; }
  
  // Note: There is no direct setting methods for the limits,
  // since we wish to make sure the validity flag is correct
  
  // Update utility
  
  void Update(double theX, double theY)
  {
    if(itsValidity)
      {
		itsXmin = MIN(itsXmin,theX);
		itsXmax = MAX(itsXmax,theX);
		itsYmin = MIN(itsYmin,theY);
		itsYmax = MAX(itsYmax,theY);
      }
    else
      {
		itsXmin = itsXmax = theX;
		itsYmin = itsYmax = theY;
		itsValidity = true;
      }
  }
  
  void Update(const NFmiEsriBox & theBox)
  {
    if(theBox.IsValid())
      {
		Update(theBox.Xmin(),theBox.Ymin());
		Update(theBox.Xmax(),theBox.Ymax());
      }
  }
  
private:
  
  bool itsValidity;
  double itsXmin;
  double itsXmax;
  double itsYmin;
  double itsYmax;
  
};

#endif // _FNMIESRIBOX_H

// ======================================================================
