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
/*!
 * \class NFmiAffine
 *
 * Mathematically, all transformations can be represented as 3x3
 * transformation matrices of the following form:
 * \f[
 * xyz
 * \f]
 * 
 * Since only six values are used in the above 3x3 matrix, a
 * transformation matrix is also expressed as a vector:
 * \f$\left[a b c d e f\right]\f$.
 * 
 * Transformations map coordinates and lengths from a new coordinate
 * system into a previous coordinate system:
 * \f[
 *    xyz
 * \f]
 * For more documentation on affine transformations see for example
 * the SVG specifications at W3C.
 */
// ======================================================================

#ifndef IMAGINE_NFMIAFFINE_H
#define IMAGINE_NFMIAFFINE_H

#ifdef __BORLANDC__
     using std::sin;
     using std::cos;
#endif

namespace Imagine
{

  //! A class to perform affine transformations
  class _FMI_DLL NFmiAffine
  {
  public:
	
	//! Default constructor creates an identify transformation
	
	NFmiAffine(void)
	  : itsA(1.0), itsB(0.0), itsC(0.0), itsD(1.0), itsE(0.0), itsF(0.0)
    {}
	
	//! Constructor for all 6 matrix elements
	
	NFmiAffine(float theA, float theB, float theC,
			   float theD, float theE, float theF)
	  : itsA(theA), itsB(theB), itsC(theC), itsD(theD), itsE(theE), itsF(theF)
	{}
	
	//! Destructor
	
	~NFmiAffine(void) {}
	
	//! Copy constructor
	
	NFmiAffine(const NFmiAffine & theAffine)
	  : itsA(theAffine.itsA), itsB(theAffine.itsB), itsC(theAffine.itsC)
		, itsD(theAffine.itsD), itsE(theAffine.itsE), itsF(theAffine.itsE)
	{}
    
	//! Assignment operator
	
	NFmiAffine & operator=(const NFmiAffine & theAffine)
	{
	  itsA=theAffine.itsA; itsB=theAffine.itsB; itsC=theAffine.itsC;
	  itsD=theAffine.itsD; itsE=theAffine.itsE; itsF=theAffine.itsF;
	  return *this;
	}
	
	//! Add translation to the transformation
	
	void Translate(float tx, float ty)
	{
	  // Multiply(1,0,0,1,tx,ty)
	  
	  itsE += itsA * tx + itsC * ty;
	  itsF += itsB * tx + itsD * ty;
	}
	
	//! Add uniform scaling to the transformation
	
	void Scale(float s)
	{
	  // Multiply(NFmiAffine(s,0,0,s,0,0));
	  itsA *= s;
	  itsB *= s;
	  itsC *= s;
	  itsD *= s;
	}
	
	//! Add general scaling to the transformation
	
	void Scale(float sx, float sy)
	{
	  // Multiply(NFmiAffine(sx,0,0,sy,0,0));
	  itsA *= sx;
	  itsB *= sx;
	  itsC *= sy;
	  itsD *= sy;
	  
	}
	
	//! Add rotation to the transformation
	
	void Rotate(float a)
	{
	  // Multiply(NFmiAffine(cos(a),sin(a),-sin(a),cos(a),0,0))
	
	  const float pi180 = static_cast<float>(3.141592658579323846/180.0);
	  float ca = cos(a*pi180);
	  float sa = sin(a*pi180);
	  
	  float oldA = itsA;
	  float oldB = itsB;
	  float oldC = itsC;
	  float oldD = itsD;
	  
	  itsA =   oldA * ca + oldC * sa;
	  itsB =   oldB * ca + oldD * sa;
	  itsC = - oldA * sa + oldC * ca;
	  itsD = - oldB * sa + oldD * ca;
	}
	
	//! Multiplication of affine transformations are done by post-multiplication
	
	void Multiply(const NFmiAffine & theAffine)
	{
	  float oldA = itsA;
	  float oldB = itsB;
	  float oldC = itsC;
	  float oldD = itsD;
	  float oldE = itsE;
	  float oldF = itsF;
	  
	  itsA = oldA * theAffine.itsA + oldC * theAffine.itsB;
	  itsB = oldB * theAffine.itsA + oldD * theAffine.itsB;
	  itsC = oldA * theAffine.itsC + oldC * theAffine.itsD;
	  itsD = oldB * theAffine.itsC + oldD * theAffine.itsD;
	  itsE = oldA * theAffine.itsE + oldC * theAffine.itsF + oldE;
	  itsF = oldB * theAffine.itsE + oldD * theAffine.itsF + oldF;
	}
	
	//! Calculate transformed X-coordinate
	
	float X(float x, float y)
	{
	  return itsA*x + itsC*y + itsE;
	}
	
	//! Calculate transformed Y-coordinate
	
	float Y(float x, float y)
	{
	  return itsB*x + itsD*y + itsF;
	}
	
  private:
	
	float itsA, itsB, itsC, itsD, itsE, itsF;
	
  };

} // namespace Imagine

#endif // IMAGINE_NFMIAFFINE_H

// ======================================================================
