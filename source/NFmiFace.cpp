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
#include "NFmiFreeType.h"
#include "NFmiImage.h"
#include "NFmiText.h"

#include "NFmiStringTools.h"

#include <stdexcept>

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
	Pimple(FT_Library & theLibrary,const string & theFile, int theWidth, int theHeight);

  private:

	Pimple();
	Pimple(const Pimple & thePimple);
	Pimple & operator=(const Pimple & thePimple);

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
  
  void NFmiFace::Draw(const NFmiText & theText,
					  NFmiImage & theImage,
					  NFmiColorTools::Color theColor,
					  NFmiColorTools::NFmiBlendRule theRule) const
  {
  }

} // namespace Imagine

#endif // UNIX

// ======================================================================
