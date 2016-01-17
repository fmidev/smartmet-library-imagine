// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiColorReduce
 */
// ======================================================================

#ifndef IMAGINE_NFMICOLORREDUCE_H
#define IMAGINE_NFMICOLORREDUCE_H

#include "NFmiColorTools.h"
#include <functional>

#include <map>

namespace Imagine
{
class NFmiImage;

namespace NFmiColorReduce
{
//! A Histogram container
typedef std::multimap<int, NFmiColorTools::Color, std::greater<int> > Histogram;

const Histogram CalcHistogram(const NFmiImage& theImage);
void AdaptiveReduce(NFmiImage& theImage, float theQuality = 10);
void AdaptiveReduce(NFmiImage& theImage, float theQuality, int theMaxColors, float theErrorFactor);

}  // namespace NFmiColorReduce

}  // namespace Imagine

#endif  // IMAGINE_NFMICOLORREDUCE_H

// ======================================================================
