// ======================================================================
//
// TrueType Font Container Class
//
// History:
//
// 19.11.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef IMAGINE_NFMIFONTTRUETYPE_H
#define IMAGINE_NFMIFONTTRUETYPE_H

#include "NFmiFont.h"

namespace Imagine
{

  class _FMI_DLL NFmiFontTrueType : public NFmiFont
  {
  public:
	
	// Constructors, destructors
	
	NFmiFontTrueType(const std::string & theName)
	  : NFmiFont(theName)
	{
	  itsType = kFmiFontTrueType;
	}
	
	~NFmiFontTrueType(void) {}
	
	// Copy, assignment
	
	NFmiFontTrueType(const NFmiFontTrueType & theFont)
	  : NFmiFont(theFont)
	{ }
	
	NFmiFontTrueType & operator=(const NFmiFontTrueType & theFont)
	{
	  NFmiFont::operator=(theFont);
	  return *this;
	}
	
	// Cloning
	
	virtual NFmiFont * Clone(void) const
	{
	  return new NFmiFontTrueType(*this);
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
	
	const NFmiPath Path(const NFmiText & theText) const;
	
	// Data-part
	
  private:
	
  };

} // namespace Imagine
  
#endif // IMAGINE_NFMIFONRTRUETYPE_H
  
// ======================================================================
