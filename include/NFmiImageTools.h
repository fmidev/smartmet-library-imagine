// ======================================================================
/*!
 * \file
 * \brief Interface of namespace Imagine::NFmiImageTools
 */
// ======================================================================

#include <string>

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

	std::string MimeType(const std::string & theFileName);

  } // namespace NFmiImageTools

} // namespace Imagine

// ======================================================================