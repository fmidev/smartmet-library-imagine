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
#include <string>

namespace Imagine
{
  class NFmiFace;

  class NFmiFreeType
  {
  public:

	static NFmiFreeType & Instance();
	NFmiFace Face(const std::string & theFile, int theWidth, int theHeight);

  private:

	~NFmiFreeType();
	NFmiFreeType();

	void Init();
	bool itsInitialized;
	FT_Library itsLibrary;

	// Not implemented:
	NFmiFreeType(const NFmiFreeType & theOb);
	NFmiFreeType & operator=(const NFmiFreeType & theOb);


  }; // class NFmiFreeType
} // namespace Imagine

#endif // IMAGINE_NFMIFREETYPE_H
#endif // UNIX

// ======================================================================
