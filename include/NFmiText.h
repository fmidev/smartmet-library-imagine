// ======================================================================
//
// C++ Text rendering API
//
// History:
//
// 24.09.2001 Mika Heiskanen
//
//	First draft version
//
// 19.11.2001 Mika Heiskanen
//
//	Final draft version
//
// ======================================================================

#ifndef _NFMITEXT_H
#define _NFMITEXT_H

#include <typeinfo>

#include <string>		// for strings

#include "NFmiDrawable.h"	// base class
#include "NFmiAlignment.h"	// text alignment
#include "NFmiFont.h"

class NFmiText : public NFmiDrawable
{
public:

  // Constructor. Note that by forcing InitFreeType() in the constructor
  // we're guaranteed to always have an instatiation of FreeType ready.

  NFmiText(const std::string & theText,
	   const NFmiFont & theFont,
	   float theSize=14,
	   float theX=0.0,
	   float theY=0.0,
	   NFmiAlignment theAlignment = kFmiAlignNorthWest,
	   float theAngle=0.0)
    : itsText(theText)
    , itsFont(theFont.Clone())
    , itsSize(theSize)
    , itsX(theX)
    , itsY(theY)
    , itsAlignment(theAlignment)
    , itsAngle(theAngle)
    {
    }

  virtual ~NFmiText(void) { delete itsFont; }

  // Data-access

  const std::string & Text(void) const	{ return itsText; }
  float Size(void) const		{ return itsSize; }
  float X(void) const			{ return itsX; }
  float Y(void) const			{ return itsY; }
  float Angle(void) const		{ return itsAngle; }
  NFmiAlignment Alignment(void) const	{ return itsAlignment; }

  // Setting data members

  void Text(const std::string & theText)	{ itsText = theText; }
  void Size(float theSize)			{ itsSize = theSize; }
  void X(float theX)				{ itsX = theX; }
  void Y(float theY)				{ itsY = theY; }
  void Angle(float theAngle)			{ itsAngle = theAngle; }
  void Alignment(NFmiAlignment theAlignment)	{ itsAlignment = theAlignment; }

  // Rendering

  void Fill(NFmiImage & theImage,
	    NFmiColorTools::Color theColor,
	    NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const
  {
    if(theColor == NFmiColorTools::NoColor)
      return;

    NFmiColorTools::NFmiBlendRule rule =
      NFmiColorTools::Simplify(theRule,NFmiColorTools::GetAlpha(theColor));

    if(rule==NFmiColorTools::kFmiColorKeep)
      return;

    itsFont->Fill(*this,theImage,theColor,rule);
  }

  void Fill(NFmiImage & theImage,
	    const NFmiImage & thePattern,
	    NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy,
	    float theAlphaFactor=1.0) const
  {
    itsFont->Fill(*this,theImage,thePattern,theRule,theAlphaFactor);
  }
	    
  void Stroke(NFmiImage & theImage,
	      NFmiColorTools::Color theColor,
	      NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const
  {
    if(theColor == NFmiColorTools::NoColor)
      return;

    NFmiColorTools::NFmiBlendRule rule =
      NFmiColorTools::Simplify(theRule,NFmiColorTools::GetAlpha(theColor));

    if(rule==NFmiColorTools::kFmiColorKeep)
      return;

    itsFont->Stroke(*this,theImage,theColor,rule);
  }

  // Return text as a path
  
  NFmiPath Path(void) const
  {
    return itsFont->Path(*this);
  }
  
private:

  // Protection from empty constructor

  NFmiText(void);

  // Data members

  std::string		itsText;
  NFmiFont *		itsFont;
  float			itsSize;
  float			itsX;
  float			itsY;
  NFmiAlignment		itsAlignment;
  float			itsAngle;

};

#endif // _NFMITEXT_H

// ======================================================================

