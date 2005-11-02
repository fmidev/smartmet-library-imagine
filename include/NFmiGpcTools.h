// ======================================================================
/*!
 * \file
 * \brief Declaration of a namespace to perform 2D logical operations on paths
 */
// ----------------------------------------------------------------------

#ifndef IMAGINE_NFMIGPCTOOLS_H
#define IMAGINE_NFMIGPCTOOLS_H
//#ifdef UNIX

namespace Imagine
{
  class NFmiPath;

  namespace NFmiGpcTools
  {
	const NFmiPath Minus(const NFmiPath & theP1, const NFmiPath & theP2);
	const NFmiPath And(const NFmiPath & theP1, const NFmiPath & theP2);
	const NFmiPath Or(const NFmiPath & theP1, const NFmiPath & theP2);
	const NFmiPath Xor(const NFmiPath & theP1, const NFmiPath & theP2);

  } // namespace NFmiGpcTools
} // namespace Imagine

//#endif // UNIX
#endif // IMAGINE_NFMIGPCTOOLS_H

// ======================================================================
