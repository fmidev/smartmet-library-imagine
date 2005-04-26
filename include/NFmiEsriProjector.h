// ======================================================================
//
// Abstract base class, from which any actual projector should be derived.
//
// History:
//
// 25.09.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef IMAGINE_NFMIESRIPROJECTOR_H
#define IMAGINE_NFMIESRIPROJECTOR_H

#include "NFmiDef.h"

namespace Imagine
{
  class NFmiEsriPoint;	// introduce projector argument type
  
  class _FMI_DLL NFmiEsriProjector
  {
  public:
	virtual ~NFmiEsriProjector(void) {};
	NFmiEsriProjector() {}
	virtual NFmiEsriPoint operator()(const NFmiEsriPoint & thePoint) const = 0;
  };

} // namespace Imagine
  
#endif // NFMIESRIPROJECTOR_H
  
// ======================================================================
  
