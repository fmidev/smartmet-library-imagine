// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiImageTools
 */
// ======================================================================

namespace Imagine
{
  class NFmiImage;

  namespace NFmiImageTools
  {
	void CompressBits(NFmiImage & theImage,
					  int theRedBits = 8,
					  int theGreenBits = 8,
					  int theBlueBits = 8,
					  int theAlphaBits = 8);

  } // namespace NFmiImageTools

} // namespace Imagine

// ======================================================================
