// ======================================================================
/*!
 * \file NFmiAffine.h
 * \brief Definition of a class to perform affine transformations.
 *
 * \b History:
 *
 * \li 27.11.2001, Mika Heiskanen\par
 * Implemented
 */
// ======================================================================

#ifndef IMAGINE_NFMIAFFINE_H
#define IMAGINE_NFMIAFFINE_H

#include "NFmiGlobals.h"

namespace Imagine
{

  //! A class to perform affine transformations
  class _FMI_DLL NFmiAffine
  {
  public:
	
	~NFmiAffine();
	NFmiAffine();
	NFmiAffine(float theA, float theB, float theC,
			   float theD, float theE, float theF);
	
	NFmiAffine(const NFmiAffine & theAffine);
	NFmiAffine & operator=(const NFmiAffine & theAffine);
	
	void Translate(float tx, float ty);
	void Scale(float s);
	void Scale(float sx, float sy);
	void Rotate(float a);
	void Multiply(const NFmiAffine & theAffine);
	float X(float x, float y);
	float Y(float x, float y);

  private:
	
	float itsA;
	float itsB;
	float itsC;
	float itsD;
	float itsE;
	float itsF;
	
  };

} // namespace Imagine

#endif // IMAGINE_NFMIAFFINE_H

// ======================================================================
