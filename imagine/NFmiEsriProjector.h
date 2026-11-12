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

#pragma once

namespace Imagine
{
class NFmiEsriBox;
class NFmiEsriPoint;  // introduce projector argument type

class NFmiEsriProjector
{
 public:
  virtual ~NFmiEsriProjector() = default;
  NFmiEsriProjector() = default;
  virtual NFmiEsriPoint operator()(const NFmiEsriPoint& thePoint) const = 0;
  virtual void SetBox(const NFmiEsriBox& theBox) const = 0;
};

}  // namespace Imagine

// ======================================================================
