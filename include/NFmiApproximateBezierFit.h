// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiApproximateBezierFit
 */
// ======================================================================

#ifndef IMAGINE_NFMIAPPROXIMATEBEZIERFIT_H
#define IMAGINE_NFMIAPPROXIMATEBEZIERFIT_H

namespace Imagine
{
  class NFmiPath;

  namespace NFmiApproximateBezierFit
  {

	NFmiPath Fit(const NFmiPath & thePath, double theMaxError);

  } // namespace NFmiApproximateBezierFit
} // namespace Imagine

#endif // IMAGINE_NFMIAPPROXIMATEBEZIERFIT_H

// ======================================================================
