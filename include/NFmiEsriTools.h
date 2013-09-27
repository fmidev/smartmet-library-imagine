#pragma once

#include "NFmiEsriShape.h"
#include <string>

namespace Imagine
{
  namespace NFmiEsriTools
  {
	NFmiEsriShape * filter(const NFmiEsriShape & theShape,
						   const std::string & theCondition);
  }
}
