// ======================================================================
/*!
 * \file
 * \brief Interface of class Imagine::NFmiFace
 */
// ======================================================================

#ifdef UNIX
#ifndef IMAGINE_NFMIFACE_H
#define IMAGINE_NFMIFACE_H

#include "NFmiAlignment.h"
#include "NFmiColorTools.h"

extern "C" {
 #include <ft2build.h>
 #include FT_FREETYPE_H
}

#include "boost/shared_ptr.hpp"
#include <string>

namespace Imagine
{
  class NFmiFreeType;
  class NFmiImage;

  class NFmiFace
  {
  public:

	~NFmiFace();
	NFmiFace(const NFmiFace & theFace);
	NFmiFace & operator=(const NFmiFace & theFace);
	
	void Draw(NFmiImage & theImage,
			  int theX, int theY,
			  const std::string & theText,
			  NFmiAlignment theAlignment = kFmiAlignNorthWest,
			  NFmiColorTools::Color theColor = NFmiColorTools::Black,
			  NFmiColorTools::NFmiBlendRule theRule = NFmiColorTools::kFmiColorAtop) const;

  private:

	friend class NFmiFreeType;
	NFmiFace(FT_Library & theLibrary,
			 const std::string & theFile,
			 int theWidth,
			 int theHeight);

	NFmiFace();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class NFmiFace
} // namespace Imagine

#endif // IMAGINE_NFMIFACE_H
#endif // UNIX

// ======================================================================
