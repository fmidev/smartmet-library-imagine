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
 * imagine::font_path
 * \endcode
 * are used. Normally the value is something like
 * \code
 * imagine::font_path = /data/share/fonts:/usr/lib/X11/fonts
 * \endcode
 *
 */
// ======================================================================

#ifdef UNIX

#include "NFmiFace.h"
#include "NFmiColorBlend.h"
#include "NFmiFreeType.h"
#include "NFmiImage.h"
#include "NFmiPath.h"

#include "NFmiFileSystem.h"
#include "NFmiSettings.h"
#include "NFmiStringTools.h"

#include FT_GLYPH_H

#include <stdexcept>
#include <vector>

// Required on Mandrake 9.0 (freetype 9.0.3, freetype 9.3.3 is fine)

#ifndef FT_PIXEL_MODE_GRAY
#define FT_PIXEL_MODE_GRAY ft_pixel_mode_grays
#define FT_KERNING_DEFAULT ft_kerning_default
#define FT_RENDER_MODE_NORMAL ft_render_mode_normal
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

	void Background(bool theMode) { itsBackgroundOn = theMode; }
	void BackgroundWidth(int theWidth) { itsBackgroundWidth = theWidth; }
	void BackgroundHeight(int theHeight) { itsBackgroundHeight = theHeight; }
	void BackgroundColor(NFmiColorTools::Color theColor) { itsBackgroundColor = theColor; }
	void BackgroundRule(NFmiColorTools::NFmiBlendRule theRule) { itsBackgroundRule = theRule; }

  private:

	Pimple();
	Pimple(const Pimple & thePimple);
	Pimple & operator=(const Pimple & thePimple);

	FT_BBox compute_bbox(const vector<FT_Glyph> & theGlyphs,
						 const vector<FT_Vector> & thePositions);

	template <class T>
	void Draw(T theBlender,
			  NFmiImage & theImage,
			  NFmiColorTools::Color theColor,
			  const FT_Bitmap & theBitmap,
			  FT_Int theX,
			  FT_Int theY);

	string itsFile;
	int itsWidth;
	int itsHeight;

	bool itsBackgroundOn;
	int itsBackgroundWidth;
	int itsBackgroundHeight;
	NFmiColorTools::Color itsBackgroundColor;
	NFmiColorTools::NFmiBlendRule itsBackgroundRule;

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
	, itsBackgroundOn(false)
	, itsBackgroundWidth(3)
	, itsBackgroundHeight(3)
	, itsBackgroundColor(NFmiColorTools::MakeColor(180,180,180,32))
	, itsBackgroundRule(NFmiColorTools::kFmiColorOnOpaque)
  {
	if(itsWidth < 0 || itsHeight < 0)
	  throw runtime_error("Face width and height cannot both be zero");

	// Find the face

	const string path = NFmiSettings::Optional<string>("imagine::font_path",".");
	const string file = NFmiFileSystem::FileComplete(itsFile,path);

	// Create the face

	FT_Error error = FT_New_Face(theLibrary,
								 file.c_str(),
								 0,
								 &itsFace);

	if(error == FT_Err_Unknown_File_Format)
	  throw runtime_error("Unknown font format in '"+file+"'");

	if(error)
	  throw runtime_error("Failed while reading font '"+file+"'");

	error = FT_Set_Pixel_Sizes(itsFace,theWidth,theHeight);
	if(error)
	  throw runtime_error("Failed to set font size "+
						  NFmiStringTools::Convert(theWidth) +
						  'x' +
						  NFmiStringTools::Convert(theHeight) +
						  " in font '" +
						  file +
						  "'");
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Compute glyph sequence bounding box
   */
  // ----------------------------------------------------------------------

  FT_BBox NFmiFace::Pimple::compute_bbox(const vector<FT_Glyph> & theGlyphs,
										 const vector<FT_Vector> & thePositions)
  {
	FT_BBox bbox;

	// initialize string bbox to "empty" values
	bbox.xMin = bbox.yMin = 32000;
	bbox.xMax = bbox.yMax = -32000;

	// for each glyph image, compute its bounding box,
	// translate it, and grow the string bbox

	for(string::size_type i = 0; i < theGlyphs.size(); i++)
	  {
		FT_BBox glyph_bbox;
		FT_Glyph_Get_CBox(theGlyphs[i], ft_glyph_bbox_pixels, &glyph_bbox );

		glyph_bbox.xMin += thePositions[i].x;
		glyph_bbox.xMax += thePositions[i].x;
		glyph_bbox.yMin += thePositions[i].y;
		glyph_bbox.yMax += thePositions[i].y;

		if ( glyph_bbox.xMin < bbox.xMin ) bbox.xMin = glyph_bbox.xMin;
		if ( glyph_bbox.yMin < bbox.yMin ) bbox.yMin = glyph_bbox.yMin;
		if ( glyph_bbox.xMax > bbox.xMax ) bbox.xMax = glyph_bbox.xMax;
		if ( glyph_bbox.yMax > bbox.yMax ) bbox.yMax = glyph_bbox.yMax;
	  }

	// check that we really grew the string bbox

	if ( bbox.xMin > bbox.xMax )
	  {
		bbox.xMin = 0;
		bbox.yMin = 0;
		bbox.xMax = 0;
		bbox.yMax = 0;
	  }

	// return string bbox

	return bbox;
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
	FT_UInt glyph_index;
	FT_Vector pen;

	vector<FT_Glyph> glyphs(theText.size());
	vector<FT_Vector> pos(theText.size());

	// start at (0,0)
	pen.x = 0;
	pen.y = 0;

	FT_Bool use_kerning = FT_HAS_KERNING(itsFace);
	FT_UInt previous = 0;

	string::size_type i;
	for(i = 0; i<theText.size(); i++)
	  {
		glyph_index = FT_Get_Char_Index(itsFace,theText[i]);

		// Retrieve kerning distance and move pen accordingly
		if(use_kerning && previous!=0 && glyph_index!=0)
		  {
			FT_Vector delta;
			FT_Get_Kerning(itsFace,previous,glyph_index,
						   FT_KERNING_DEFAULT,&delta);
			pen.x += (delta.x >> 6);
		  }

		// store current pen position
		pos[i] = pen;

		// load glyph image into the slow without rendering

		error = FT_Load_Glyph(itsFace, glyph_index, FT_LOAD_DEFAULT);
		if(error)
		  continue;

		// Extract glyph image and store it in our table

		error = FT_Get_Glyph(itsFace->glyph,&glyphs[i]);
		if(error)
		  continue;

		// Increment pen position
		pen.x += (slot->advance.x >> 6);

		// Record current glyph index
		previous = glyph_index;

	  }

	// Compute bounding box
	const FT_BBox bbox = compute_bbox(glyphs,pos);

	// string pixel size

	const int width  = bbox.xMax - bbox.xMin;
	const int height = bbox.yMax - bbox.yMin;

	// Compute start position in 26.6 cartesian pixels

	const double xfactor = XAlignmentFactor(theAlignment);
	const double yfactor = YAlignmentFactor(theAlignment);

	const int start_x = static_cast<int>(64*(theX - xfactor*width));
	const int start_y = static_cast<int>(64*(theY + (1- yfactor)*height));

	// Render the background

	if(itsBackgroundOn)
	  {
		const int x1 = (start_x >> 6) - itsBackgroundWidth;
		const int y2 = (start_y >> 6) + itsBackgroundHeight;
		const int x2 = x1 + width  + 2*itsBackgroundWidth;
		const int y1 = y2 - height - 2*itsBackgroundHeight;

		NFmiPath path;
		path.MoveTo(x1,y1);
		path.LineTo(x2,y1);
		path.LineTo(x2,y2);
		path.LineTo(x1,y2);
		path.CloseLineTo();

		path.Fill(theImage,itsBackgroundColor,itsBackgroundRule);
	  }

	// And render the glyphs

	for(i = 0; i<glyphs.size(); i++)
	  {
		FT_Glyph image = glyphs[i];

		pen.x = start_x + 64*pos[i].x;
		pen.y = start_y + 64*pos[i].y;

		error = FT_Glyph_To_Bitmap(&image, FT_RENDER_MODE_NORMAL, &pen, 0);

		if(!error)
		  {
			FT_BitmapGlyph bit = reinterpret_cast<FT_BitmapGlyph>(image);
			this->Draw(theBlender, theImage, theColor,
					   bit->bitmap,
					   (pen.x >> 6) + bit->left,
					   (pen.y >> 6) - bit->top);

			FT_Done_Glyph(image);
		  }

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
							  const FT_Bitmap & theBitmap,
							  FT_Int theX,
							  FT_Int theY)
  {
	FT_Int i, j, p, q;
	FT_Int x_max = theX + theBitmap.width;
	FT_Int y_max = theY + theBitmap.rows;
	
	for ( i = theX, p = 0; i < x_max; i++, p++ )
	  {
		for ( j = theY, q = 0; j < y_max; j++, q++ )
		  {
			if ( i<0 || j<0 || i >= theImage.Width() || j >= theImage.Height())
			  continue;

			int alpha = 0;
			if(theBitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
			  alpha = theBitmap.buffer[q * theBitmap.width + p];
			else
			  {
				int value = theBitmap.buffer[q * (theBitmap.pitch) + (p>>3)];
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
   * \brief Set background rendering on or off
   */
  // ----------------------------------------------------------------------

  void NFmiFace::Background(bool theMode)
  {
	itsPimple->Background(theMode);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set background margins
   *
   * \param theWidth The extra padding in x-direction
   * \param theHeight The extra padding in y-direction
   */
  // ----------------------------------------------------------------------

  void NFmiFace::BackgroundMargin(int theWidth, int theHeight)
  {
	if(theWidth < 0 || theHeight < 0)
	  throw runtime_error("Background margins must be nonnegative");
	itsPimple->BackgroundWidth(theWidth);
	itsPimple->BackgroundHeight(theHeight);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set background color
   *
   * \param theColor The background color
   */
  // ----------------------------------------------------------------------

  void NFmiFace::BackgroundColor(NFmiColorTools::Color theColor)
  {
	itsPimple->BackgroundColor(theColor);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Set background blending rule
   *
   * \param theRule The blending rule
   */
  // ----------------------------------------------------------------------

  void NFmiFace::BackgroundRule(NFmiColorTools::NFmiBlendRule theRule)
  {
	itsPimple->BackgroundRule(theRule);
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
