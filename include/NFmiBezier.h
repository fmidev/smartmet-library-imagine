// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiBezier
 */
// ======================================================================

#ifndef IMAGINE_NFMIBEZIER_H
#define IMAGINE_NFMIBEZIER_H

namespace Imagine
{
  class NFmiPath;

  namespace NFmiBezier
  {

	NFmiPath CardinalApproximation(const NFmiPath & thePath,
								   double theSmoothness);

	NFmiPath ToleranceApproximation(const NFmiPath & thePath,
									double theMaxError);

  } // namespace NFmiBezier
} // namespace Imagine

#endif // IMAGINE_NFMIBEZIER_H

// ======================================================================
