// ======================================================================
//
// Type1 Font Container Class
//
// History:
//
// 19.11.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef IMAGINE_NFMIFONTTYPE1_H
#define IMAGINE_NFMIFONTTYPE1_H

#include "NFmiFont.h"

namespace Imagine
{

  class _FMI_DLL NFmiFontType1 : public NFmiFont
  {
  public:
	
	// Constructors, destructors
	
	NFmiFontType1(const std::string & theName)
	  : NFmiFont(theName)
	{
	  itsType = kFmiFontType1;
	}
	
	~NFmiFontType1(void) {}
	
	// Copy, assignment
	
	NFmiFontType1(const NFmiFontType1 & theFont)
	  : NFmiFont(theFont)
	{ }
	
	NFmiFontType1 & operator=(const NFmiFontType1 & theFont)
	{
	  NFmiFont::operator=(theFont);
	  return *this;
	}
	
	// Cloning
	
	virtual NFmiFont * Clone(void) const
	{
	  return new NFmiFontType1(*this);
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

} // namespace Imagine
  
#endif // IMAGINE_NFMIFONRTYPE1_H
  
// ======================================================================
