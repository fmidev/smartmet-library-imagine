// ======================================================================
/*!
 * \file
 * \brief Implementation of singleton class Imagine::NFmiFreeType
 */
// ======================================================================
/*!
 * \class Imagine::NFmiFreeType
 *
 * \brief FreeType instance management class
 *
 * The class makes sure FreeType is always properly initialized
 * on entry and FreeType memory deallocated on exit.
 *
 * Any code using FreeType functions should always call
 * \code
 * NFmiFreeType::Instance().Init();
 * \endcode
 * first. The class makes the FreeType initialization only once
 * regardless of how many times Init is called.
 *
 * The singleton may later on be used to access the FreeType
 * cache, which is now in a beta phase.
 *
 */
// ======================================================================

#ifdef UNIX

#include "NFmiFreeType.h"
#include <stdexcept>

using namespace std;

namespace Imagine
{
  // ----------------------------------------------------------------------
  /*!
   * \brief Destructor
   */
  // ----------------------------------------------------------------------

  NFmiFreeType::~NFmiFreeType()
  {
	if(itsInitialized)
	  FT_Done_FreeType(itsLibrary);
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Constructor used privately by Instance()
   */
  // ----------------------------------------------------------------------

  NFmiFreeType::NFmiFreeType()
	: itsInitialized(false)
  {
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Return an instance of NFmiFreeType
   *
   * \return A reference to a NFmiFreeType singleton
   */
  // ----------------------------------------------------------------------

  NFmiFreeType & NFmiFreeType::Instance()
  {
	static NFmiFreeType freetype;
	return freetype;
  }

  // ----------------------------------------------------------------------
  /*!
   * \brief Initialize freetype
   *
   * Throws std::runtime_error if FreeType initialization call
   * fails.
   */
  // ----------------------------------------------------------------------

  void NFmiFreeType::Init()
  {
	// Quick exit if initialization was already done
	if(itsInitialized)
	  return;

	FT_Error error = FT_Init_FreeType(&itsLibrary);
	if(error)
	  throw runtime_error("Initializing FreeType failed");

	itsInitialized = true;
  }

} // namespace Imagine

#endif // UNIX

// ======================================================================
