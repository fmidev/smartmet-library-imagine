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

#ifndef _NFMIESRIPROJECTOR_H
#define _NFMIESRIPROJECTOR_H

class NFmiEsriPoint;	// introduce projector argument type

class NFmiEsriProjector
{
public:
  virtual ~NFmiEsriProjector(void) {};
  virtual NFmiEsriPoint operator()(const NFmiEsriPoint & thePoint) const = 0;
};

#endif // NFMIESRIPROJECTOR_H

// ======================================================================
