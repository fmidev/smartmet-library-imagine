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
	Pimple(const string & theFile);

  private:

	Pimple();
	Pimple(const Pimple & thePimple);
	Pimple & operator=(const Pimple & thePimple);

	string itsFile;
	bool itsInitialized;

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

  NFmiFace::Pimple::Pimple(const string & theFile)
	: itsFile(theFile)
	, itsInitialized(false)
  {
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
   */
  // ----------------------------------------------------------------------

  NFmiFace::NFmiFace(const string & theFile)
	: itsPimple(new Pimple(theFile))
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
