// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace Imagine::NFmiImageTools
 */
// ======================================================================

#include "NFmiImageTools.h"
#include "NFmiImage.h"

#include <cassert>
#include <stdexcept>

using namespace std;

namespace Imagine
{
  namespace
  {
	// ----------------------------------------------------------------------
	/*!
	 * \brief Compress integer in range 0-255 to given number of bits
	 *
	 *  - 0 reduces to values 0,FF
	 *  - 1 reduces to values 0,80,FF
	 *  - 2 reduces to values 0,40,80,C0,FF
	 *  - 3 reduces to values 0,20,40,...,FF
	 *  - 4 reduces to values 0,10,20,...,FF
	 *  - 5 reduces to values 0,08,10,...,FF
	 *  - 6 reduces to values 0,04,08,...,FF
	 *  - 7 reduces to values 0,02,04,...,FF
	 *  - 8 reduces to values 0,01,02,...,FF
	 *
	 * Each value is rounded to the value nearest to it
	 */
	// ----------------------------------------------------------------------

	inline
	int compress_bits(int theValue, int theBits)
	{
	  assert(theBits>=0 && theBits<=8);
	  if(theBits==8) return theValue;
	  int round = (theValue >> (8-theBits-1)) & 1;
	  int value = (theValue >> (8-theBits)) + round;
	  return min(255,value << (8-theBits));
	}

  } // namespace anonymous


  namespace NFmiImageTools
  {

	// ----------------------------------------------------------------------
	/*!
	 * \brief Reduce the accuracy of the color components
	 *
	 * This is a poor mans color reduction algorithm.
	 *
	 * \param theImage The image to reduce unique colors from
	 * \param theRedBits The desired accuracy of red bits
	 * \param theGreenBits The desired accuracy of green bits
	 * \param theBlueBits The desired accuracy of blue bits
	 * \param theAlphaBits The desired accuracy of alpha bits
	 */
	// ----------------------------------------------------------------------

	void CompressBits(NFmiImage & theImage,
					  int theRedBits,
					  int theGreenBits,
					  int theBlueBits,
					  int theAlphaBits)
	{
	  using namespace NFmiColorTools;

	  if(theRedBits < 0 || theRedBits > 8 ||
		 theGreenBits < 0 || theGreenBits > 8 ||
		 theBlueBits < 0 || theBlueBits > 8 ||
		 theAlphaBits < 0 || theAlphaBits > 8)
		{
		  throw runtime_error("Color resolution must be 0-8 bits");
		}

	  for(int j=0; j<theImage.Height(); j++)
		for(int i=0; i<theImage.Width(); i++)
		  {
			Color & c = theImage(i,j);
			const int r = compress_bits(GetRed(c),theRedBits);
			const int g = compress_bits(GetGreen(c),theGreenBits);
			const int b = compress_bits(GetBlue(c),theBlueBits);
			const int a = compress_bits(GetAlpha(c),theAlphaBits);
			c = MakeColor(r,g,b,a);
		  }
	}

  } // namespace NFmiImageTools

} // namespace Imagine

// ======================================================================
