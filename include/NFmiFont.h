// ======================================================================
//
// Abstract Font Container Class.
//
// This class does nothing but stores a font for the purpose of being
// rendered by methods in the NFmiText class.
//
// History:
//
// 19.11.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef _NFMIFONT_H
#define _NFMIFONT_H

#include <string>

#include "NFmiColorTools.h"
#include "NFmiPath.h"
#include "NFmiImage.h"

class NFmiText;

// Different font types

enum NFmiFontType { kFmiFontMissing,
					kFmiFontHershey,
					kFmiFontTrueType,
					kFmiFontType1 };

class _FMI_DLL NFmiFont
{
public:
  
  // Constructors, destructors
  
  NFmiFont(const std::string & theName)
    : itsType(kFmiFontHershey)
    , itsName(theName)
  {}
  
  virtual ~NFmiFont(void) {}
  
  // Copy, assignment
  
  NFmiFont(const NFmiFont & theFont)
    : itsType(theFont.itsType)
    , itsName(theFont.itsName)
  {}
  
  NFmiFont & operator=(const NFmiFont & theFont)
  {
    itsType = theFont.itsType;
    itsName = theFont.itsName;
    return *this;
  }
  
  // Cloning
  
  virtual NFmiFont * Clone(void) const
  {
    return new NFmiFont(*this);
  }
  
  // Data-access
  
  const std::string & Name(void) const	{ return itsName; }
  NFmiFontType Type(void) const		{ return itsType; }
  
  // Font-type conversions
  
  NFmiFontType FontType(const std::string & theName) const;
  std::string FontType(NFmiFontType theType) const;
  
  // Rendering text on an image.
  
  virtual void Fill(const NFmiText & theText,
					NFmiImage & theImage,
					NFmiColorTools::Color theColor,
					NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const
  {}
  
  virtual void Fill(const NFmiText & theText,
					NFmiImage & theImage,
					const NFmiImage & thePattern,
					NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy,
					float theAlphaFactor=1.0) const
  {}
  
  virtual void Stroke(const NFmiText & theText,
					  NFmiImage & theImage,
					  NFmiColorTools::Color theColor,
					  NFmiColorTools::NFmiBlendRule theRule=NFmiColorTools::kFmiColorCopy) const
  {}
  
  // Return text as a path
  
  virtual NFmiPath Path(const NFmiText & theText) const
  {
	return NFmiPath();
  }
  
  // Data-part
  
protected:
  
  NFmiFontType itsType;		// Type of the respective font
  std::string itsName;		// Name given in constructor
  
private:
  
};

#endif // _NFMIFONT_H

// ======================================================================

