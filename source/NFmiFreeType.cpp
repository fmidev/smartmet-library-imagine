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
 * Any code requiring a face must always use
 * \code
 * NFmiFreeType::Instance().Face(name,width,size)
 * \endcode
 * The singleton makes sure the FreeType library is initialized
 * when the first face is requested.
 *
 */
// ======================================================================

#ifdef UNIX

#include "NFmiFreeType.h"
#include "NFmiFace.h"
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

  // ----------------------------------------------------------------------
  /*!
   * \brief Create a new face object
   *
   * \param theFile The file containing the face
   * \param theWidth The desired width of the face (or zero)
   * \param theHeight The desired height of the face (or zero)
   * \return A face object
   */
  // ----------------------------------------------------------------------

  NFmiFace NFmiFreeType::Face(const string & theFile,
							  int theWidth,
							  int theHeight)
  {
	Init();
	NFmiFace face(theFile,theWidth,theHeight);
	return face;
  }

} // namespace Imagine

#endif // UNIX

// ======================================================================
