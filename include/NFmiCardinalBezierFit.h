// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiCardinalBezierFit
 */
// ======================================================================

#ifndef IMAGINE_NFMICARDINALBEZIERFIT_H
#define IMAGINE_NFMICARDINALBEZIERFIT_H

namespace Imagine
{
  class NFmiPath;

  namespace NFmiCardinalBezierFit
  {

	NFmiPath Fit(const NFmiPath & thePath, double theSmoothness);

  } // namespace NFmiCardinalBezierFit
} // namespace Imagine

#endif // IMAGINE_NFMICARDINALBEZIERFIT_H

// ======================================================================
