// ======================================================================
/*!
 * \file
 * \brief Interface of class Imagine::NFmiFace
 */
// ======================================================================

#ifdef UNIX
#ifndef IMAGINE_NFMIFACE_H
#define IMAGINE_NFMIFACE_H

#include "NFmiColorTools.h"
#include "boost/shared_ptr.hpp"
#include <string>

namespace Imagine
{
  class NFmiText;
  class NFmiImage;

  class NFmiFace
  {
  public:

	~NFmiFace();
	NFmiFace(const NFmiFace & theFace);
	NFmiFace & operator=(const NFmiFace & theFace);
	NFmiFace(const std::string & theFile);
	
	void Draw(const NFmiText & theText,
			  NFmiImage & theImage,
			  NFmiColorTools::Color theColor,
			  NFmiColorTools::NFmiBlendRule theRule = NFmiColorTools::kFmiColorCopy) const;

  private:

	NFmiFace();

	class Pimple;
	boost::shared_ptr<Pimple> itsPimple;

  }; // class NFmiFace
} // namespace Imagine

#endif // IMAGINE_NFMIFACE_H
#endif // UNIX

// ======================================================================
