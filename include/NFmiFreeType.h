// ======================================================================
/*!
 * \file
 * \brief Interface of singleton class Imagine::NFmiFreeType
 */
// ======================================================================

#ifdef UNIX

#ifndef IMAGINE_NFMIFREETYPE_H
#define IMAGINE_NFMIFREETYPE_H

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Imagine
{
  class NFmiFreeType
  {
  public:

	static NFmiFreeType & Instance();
	void Init();

  private:

	~NFmiFreeType();
	NFmiFreeType();
	NFmiFreeType(const NFmiFreeType & theOb);
	NFmiFreeType & operator=(const NFmiFreeType & theOb);

	bool itsInitialized;
	FT_Library itsLibrary;

  }; // class NFmiFreeType
} // namespace Imagine

#endif // IMAGINE_NFMIFREETYPE_H
#endif // UNIX

// ======================================================================
