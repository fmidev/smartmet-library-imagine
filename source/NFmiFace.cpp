// ======================================================================
/*!
 * \file
 * \brief Implementation of class Imagine::NFmiFace
 */
// ======================================================================
/*!
 * \class Imagine::NFmiFace
 *
 * \brief A font class providing text rendering services
 *
 * The class implements a simple interface for rendering any
 * font type supported by the FreeType library. The face
 * objects are instantiated by giving the filename of the
 * font object as an argument to the constructor. If the
 * filename is relative, the search paths defined by
 * \code
 * imagine::font_paths
 * \endcode
 * are used. Normally the value is something like
 * \code
 * imagine::font_paths = /data/share/fonts:/usr/lib/X11/fonts
 * \endcode
 *
 */
// ======================================================================

#ifdef UNIX

#include "NFmiFace.h"
#include "NFmiColorBlend.h"
#include "NFmiFreeType.h"
#include "NFmiImage.h"

#include "NFmiStringTools.h"

#include <stdexcept>

// Required on Mandrake 9.0 (freetype 9.0.3, freetype 9.3.3 is fine)

#ifndef FT_PIXEL_MODE_GRAY
#define FT_PIXEL_MODE_GRAY ft_pixel_mode_grays
#endif

using namespace std;

namespace Imagine
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Implementation hiding pimple
   */
  // ----------------------------------------------------------------------

  class NFmiFace::Pimple
  {
  public:
	~Pimple();
	Pimple(FT_Library & theLibrary,
		   const string & theFile,
		   int theWidth,
		   int theHeight);

	template <class T>
	void Draw(T theBlender,
			  NFmiImage & theImage,
			  int theX, int theY,
			  const string & theText,
			  NFmiAlignment theAlignment,
			  NFmiColorTools::Color theColor);

  private:

	Pimple();
	Pimple(const Pimple & thePimple);
	Pimple & operator=(const Pimple & thePimple);

	template <class T>
	void Draw(T theBlender,
			  NFmiImage & theImage,
			  NFmiColorTools::Color theColor,
			  FT_Bitmap * theBitmap,
			  FT_Int theX,
			  FT_Int theY);

	string itsFile;
	int itsWidth;
	int itsHeight;
	FT_Face itsFace;

  }; // class NFmiFace::Pimple

  // ----------------------------------------------------------------------
  /*!
   * \brief Pimple destructor
   */
  // ----------------------------------------------------------------------

  NFmiFace::Pimple::~Pimple()
  {
	FT_Done_Face(itsFace);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Pimple constructor
   */
  // ----------------------------------------------------------------------

  NFmiFace::Pimple::Pimple(FT_Library & theLibrary,
						   const string & theFile,
						   int theWidth,
						   int theHeight)
	: itsFile(theFile)
	, itsWidth(theWidth)
	, itsHeight(theHeight)
  {
	if(itsWidth < 0 || itsHeight < 0)
	  throw runtime_error("Face width and height cannot both be zero");

	// Create the face

	FT_Error error = FT_New_Face(theLibrary,
								 itsFile.c_str(),
								 0,
								 &itsFace);

	if(error == FT_Err_Unknown_File_Format)
	  throw runtime_error("Unknown font format in '"+itsFile+"'");

	if(error)
	  throw runtime_error("Failed while reading font '"+itsFile+"'");

	error = FT_Set_Pixel_Sizes(itsFace,theWidth,theHeight);
	if(error)
	  throw runtime_error("Failed to set font size "+
						  NFmiStringTools::Convert(theWidth) +
						  'x' +
						  NFmiStringTools::Convert(theHeight) +
						  " in font '" +
						  itsFile +
						  "'");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Render the given text
   */
  // ----------------------------------------------------------------------

  template <class T>
  void NFmiFace::Pimple::Draw(T theBlender,
							  NFmiImage & theImage,
							  int theX, int theY,
							  const string & theText,
							  NFmiAlignment theAlignment,
							  NFmiColorTools::Color theColor)
  {
	FT_Error error;
	FT_GlyphSlot slot = itsFace->glyph;
	FT_Vector pen;
	pen.x = 64*theX;
	pen.y = 64*theY;

	for(string::size_type i = 0; i<theText.size(); i++)
	  {
		error = FT_Load_Char(itsFace,theText[i], FT_LOAD_RENDER);
		if(error) continue;
		this->Draw(theBlender,theImage,theColor,
				   &slot->bitmap,
				   (pen.x >> 6) + slot->bitmap_left,
				   (pen.y >> 6) + slot->bitmap_top);

		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	  }

  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Render the given glyph
   */
  // ----------------------------------------------------------------------

  template <class T>
  void NFmiFace::Pimple::Draw(T theBlender,
							  NFmiImage & theImage,
							  NFmiColorTools::Color theColor,
							  FT_Bitmap * theBitmap,
							  FT_Int theX,
							  FT_Int theY)
  {
	FT_Int i, j, p, q;
	FT_Int x_max = theX + theBitmap->width;
	FT_Int y_max = theY + theBitmap->rows;
	
	for ( i = theX, p = 0; i < x_max; i++, p++ )
	  {
		for ( j = theY, q = 0; j < y_max; j++, q++ )
		  {
			if ( i<0 || j<0 || i >= theImage.Width() || j >= theImage.Height())
			  continue;

			int alpha = 0;
			if(theBitmap->pixel_mode == FT_PIXEL_MODE_GRAY)
			  alpha = theBitmap->buffer[q * theBitmap->width + p];
			else
			  {
				int value = theBitmap->buffer[q * (theBitmap->pitch) + (p>>3)];
				int bit = (value << (p&7)) & 128;
				alpha = (bit!=0 ? 255 : 0);
			  }
			if(alpha == 255)
			  theImage(i,j) = theBlender.Blend(theColor,theImage(i,j));
			else
			  {
				int a = NFmiColorTools::GetAlpha(theColor);
				int aa = static_cast<int>(a + (1.0-alpha/255.0)*(NFmiColorTools::MaxAlpha-a));
				NFmiColorTools::Color c = NFmiColorTools::ReplaceAlpha(theColor,aa);
				theImage(i,j) = theBlender.Blend(c,theImage(i,j));
			  }
		  }
	  }
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  NFmiFace::~NFmiFace()
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Copy constructor
   */
  // ----------------------------------------------------------------------

  NFmiFace::NFmiFace(const NFmiFace & theFace)
	: itsPimple(theFace.itsPimple)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Assignment operator
   */
  // ----------------------------------------------------------------------

  NFmiFace & NFmiFace::operator=(const NFmiFace & theFace)
  {
	if(this != &theFace)
	  itsPimple = theFace.itsPimple;
	return *this;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor
   *
   * \param theFile The file defining the font
   * \param theWidth The font width in pixels
   * \param theHeight The font height in pixels
   */
  // ----------------------------------------------------------------------

  NFmiFace::NFmiFace(FT_Library & theLibrary,
					 const string & theFile,
					 int theWidth,
					 int theHeight)
	: itsPimple(new Pimple(theLibrary,theFile,theWidth,theHeight))
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Render text onto image
   *
   * \param theText The text object to render
   * \param theImage The image to render into
   * \param theColor To color of the text
   * \param theRule The color blending rule
   */
  // ----------------------------------------------------------------------
  
  void NFmiFace::Draw(NFmiImage & theImage,
					  int theX, int theY,
					  const string & theText,
					  NFmiAlignment theAlignment,
					  NFmiColorTools::Color theColor,
					  NFmiColorTools::NFmiBlendRule theRule) const
  {
	// Quick exit if color is not real
	
	if(theColor==NFmiColorTools::NoColor)
	  return;
	
	// When the color is opaque or transparent, some rules will simplify.
	// Instead of using ifs in the innermost loop, we will simplify the
	// rule itself here.

	int alpha = NFmiColorTools::GetAlpha(theColor);
	NFmiColorTools::NFmiBlendRule rule = NFmiColorTools::Simplify(theRule,alpha);

	// If the result is ColorKeep, the source alpha is such that there
	// is nothing to do!

	if(rule==NFmiColorTools::kFmiColorKeep)
	  return;

	switch(rule)
	  {
	  case NFmiColorTools::kFmiColorClear:
		itsPimple->Draw(NFmiColorBlendClear(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorCopy:
		itsPimple->Draw(NFmiColorBlendCopy(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorAddContrast:
		itsPimple->Draw(NFmiColorBlendAddContrast(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorReduceContrast:
		itsPimple->Draw(NFmiColorBlendReduceConstrast(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorOver:
		itsPimple->Draw(NFmiColorBlendOver(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorUnder:
		itsPimple->Draw(NFmiColorBlendUnder(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorIn:
		itsPimple->Draw(NFmiColorBlendIn(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorKeepIn:
		itsPimple->Draw(NFmiColorBlendKeepIn(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorOut:
		itsPimple->Draw(NFmiColorBlendOut(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorKeepOut:
		itsPimple->Draw(NFmiColorBlendKeepOut(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorAtop:
		itsPimple->Draw(NFmiColorBlendAtop(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorKeepAtop:
		itsPimple->Draw(NFmiColorBlendKeepAtop(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorXor:
		itsPimple->Draw(NFmiColorBlendXor(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorPlus:
		itsPimple->Draw(NFmiColorBlendPlus(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorMinus:
		itsPimple->Draw(NFmiColorBlendMinus(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorAdd:
		itsPimple->Draw(NFmiColorBlendAdd(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorSubstract:
		itsPimple->Draw(NFmiColorBlendSubstract(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorMultiply:
		itsPimple->Draw(NFmiColorBlendMultiply(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorDifference:
		itsPimple->Draw(NFmiColorBlendDifference(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorCopyRed:
		itsPimple->Draw(NFmiColorBlendCopyRed(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorCopyGreen:
		itsPimple->Draw(NFmiColorBlendCopyGreen(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorCopyBlue:
		itsPimple->Draw(NFmiColorBlendCopyBlue(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorCopyMatte:
		itsPimple->Draw(NFmiColorBlendCopyMatte(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorCopyHue:
		itsPimple->Draw(NFmiColorBlendCopyHue(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorCopyLightness:
		itsPimple->Draw(NFmiColorBlendCopyLightness(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorCopySaturation:
		itsPimple->Draw(NFmiColorBlendCopySaturation(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorKeepMatte:
		itsPimple->Draw(NFmiColorBlendKeepMatte(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorKeepHue:
		itsPimple->Draw(NFmiColorBlendKeepHue(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorKeepLightness:
		itsPimple->Draw(NFmiColorBlendKeepLightness(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorKeepSaturation:
		itsPimple->Draw(NFmiColorBlendKeepSaturation(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorBumpmap:
		itsPimple->Draw(NFmiColorBlendBumpmap(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorDentmap:
		itsPimple->Draw(NFmiColorBlendDentmap(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorOnOpaque:
		itsPimple->Draw(NFmiColorBlendOnOpaque(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
	  case NFmiColorTools::kFmiColorOnTransparent:
		itsPimple->Draw(NFmiColorBlendOnTransparent(), theImage, theX, theY, theText, theAlignment, theColor);
		break;
		
		// Some special cases
	  case NFmiColorTools::kFmiColorKeep:
	  case NFmiColorTools::kFmiColorRuleMissing:
		break;
	  }
  }

} // namespace Imagine

#endif // UNIX

// ======================================================================
