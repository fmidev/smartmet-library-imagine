// ======================================================================
//
// Hershey Font Container Class
//
// History:
//
// 19.11.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef _NFMIFONTHERSHEY_H
#define _NFMIFONTHERSHEY_H

#include "NFmiFont.h"

class NFmiFontHershey : public NFmiFont
{
public:

  // Constructors, destructors

  NFmiFontHershey(const std::string & theName)
    : NFmiFont(theName)
  {
    itsType = kFmiFontHershey;
  }

  virtual ~NFmiFontHershey(void) {}

  // Copy, assignment

  NFmiFontHershey(const NFmiFontHershey & theFont)
    : NFmiFont(theFont)
  { }

  NFmiFontHershey & operator=(const NFmiFontHershey & theFont)
  {
    NFmiFont::operator=(theFont);
    return *this;
  }
  
  // Cloning

  NFmiFont * Clone(void) const
  {
    return new NFmiFontHershey(*this);
  }

  // Rendering

  void Fill(const NFmiText & theText,
	    NFmiImage & theImage,
	    NFmiColorTools::Color theColor,
	    NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const;

  void Fill(const NFmiText & theText,
	    NFmiImage & theImage,
	    const NFmiImage & thePattern,
	    NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy,
	    float theAlphaFactor=1.0) const;

  void Stroke(const NFmiText & theText,
	      NFmiImage & theImage,
	      NFmiColorTools::Color theColor,
	      NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const;

  // Return text as a path

  NFmiPath Path(const NFmiText & theText) const;

  // Data-part

private:

};

#endif // _NFMIFONRHERSHEY_H

// ======================================================================

