// ======================================================================
//
// C++ Image API, defining basic image operations.
//
// This is a wrapper for the GD graphics library, with some additional
// methods defined to improve performance.
//
// History:
//
// 13.08.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifndef _NFMIIMAGE_H
#define _NFMIIMAGE_H

#include <string>	// for filenames, drawing text etc
#include <utility>	// for pairs
#include <vector>	// for vectors
#include <set>		// for sets
#include <cmath>	// for min,max,floor,ceil etc	
#include <cassert>
#include <cstdio>

#include "NFmiDrawable.h"
#include "NFmiAlignment.h"

class NFmiImage : public NFmiDrawable
{
private:
  
  // Data elements
  
  int			itsWidth;
  int			itsHeight;
  NFmiColorTools::Color	*itsPixels;
  
  // Various options
  
  int	itsJpegQuality;		// JPEG compression quality, 0-100
  int	itsPngQuality;		// PNG compression level, 0-9
  int	itsPngFilter;		// PNG filter
  int	itsAlphaLimit;		// alpha<=limit is considered opaque
  float itsGamma;		// Gamma, 1-3 is reasonable
  std::string itsIntent;	// Rendering intent (PNG)
  
  bool	itsSaveAlphaFlag;	// true if alpha is to be saved
  bool  itsWantPaletteFlag;	// true if palette is desired when possible
  bool	itsForcePaletteFlag;	// true if palette is to be forced
  
public:
  
  // Constructors, destructors
  
  NFmiImage(int theWidth=0, int theHeight=0, NFmiColorTools::Color theColor=0);
  NFmiImage(const NFmiImage & theImg);
  NFmiImage(const std::string & theFileName);
  virtual ~NFmiImage(void) { Destroy(); }
  
  // Data access
  
  int Width(void) const { return itsWidth; }
  int Height(void) const { return itsHeight; }
  
  // All constructors call this to set the default options
  
  void DefaultOptions(void);
  
  // Access to individual options
  
  int JpegQuality(void) const		{ return itsJpegQuality; }
  int PngQuality(void) const		{ return itsPngQuality; }
  int AlphaLimit(void) const		{ return itsAlphaLimit; }
  bool SaveAlpha(void) const		{ return itsSaveAlphaFlag; }
  bool WantPalette(void) const		{ return itsWantPaletteFlag; }
  bool ForcePalette(void) const		{ return itsForcePaletteFlag; }
  float Gamma(void) const		{ return itsGamma; }
  std::string Intent(void) const	{ return itsIntent; }
  
  void JpegQuality(int quality)		{ itsJpegQuality = quality; }
  void PngQuality(int quality)		{ itsPngQuality = quality; }
  void AlphaLimit(int limit)		{ itsAlphaLimit = limit; }
  void SaveAlpha(bool flag)		{ itsSaveAlphaFlag = flag; }
  void WantPalette(bool flag)		{ itsWantPaletteFlag = flag; }
  void ForcePalette(bool flag)		{ itsForcePaletteFlag = flag; }
  void Gamma(float value)		{ itsGamma = value; }
  void Intent(const std::string & value){ itsIntent = value; }
  
  // This makes  A(i,j) = B(x,y) work
  
  NFmiColorTools::Color & operator()(int i, int j) const
  {
    return itsPixels[j*itsWidth+i];
  }
  
  // Assignment operator
  
  NFmiImage & operator= (const NFmiImage & theImage);
  NFmiImage & operator= (const NFmiColorTools::Color theColor);
  
  // Reading an image
  
  void Read(const std::string & theFileName);
  
  // Writing the image
  
  void WriteJpeg(const std::string & theFileName) const;
  void WritePng(const std::string & theFileName) const;
  void WriteGif(const std::string & theFileName) const;
  
  // Erasing image with desired colour
  
  void Erase(NFmiColorTools::Color theColor);
  
  // Reducing the number of colors in an image
  
  void Quantize(int maxcolors,
				int opaquethreshold=-1,
				bool ignoreAlpha=false) const;
  
  // A simple non-antialiased line of width 1 pixel
  
  void StrokeBasic(float theX1, float theY1,
				   float theX2, float theY2,
				   NFmiColorTools::Color theColor,
				   NFmiColorTools::NFmiBlendRule theRule);
  
  // Composite image over another
  
  void Composite(const NFmiImage & theImage,
				 NFmiColorTools::NFmiBlendRule theRule,
				 NFmiAlignment theAlignment = kFmiAlignNorthWest,
				 int theX=0,
				 int theY=0,
				 float theAlpha=1.0);
  
private:
  
  // Constructor, destructor utilities
  
  void Destroy(void);
  void Allocate(int width, int height);
  void Reallocate(int width, int height);
  
  // Reading and writing various image formats
  
  void ReadPNG(FILE *in);
  void WritePNG(FILE *out) const;
  
  void ReadJPEG(FILE *in);
  void WriteJPEG(FILE *out) const;
  
  void ReadGIF(FILE *in);
  void WriteGIF(FILE *out) const;
  
  // Test whether the image is opaque
  
  bool IsOpaque(int threshold = -1) const;
  
  // Test whether the image is fully opaque/transparent
  
  bool IsFullyOpaqueOrTransparent(int threshold = -1) const;
  
  // Returns an RGB value which does NOT occur in the image
  
  NFmiColorTools::Color UnusedColor(void) const;
  
  // Put image colors into the given set
  
  bool AddColors(std::set<NFmiColorTools::Color> & theSet,
				 int maxcolors=-1,
				 int opaquethreshold=-1,
				 bool ignoreAlpha=false) const;
  
  // Strokebasic low level drivers for each blending rule
  // By default we use the generally faster RGBA version below
  // Specializations later on in the code may overload the latter
  // interface below for defining possible optimized versions.
  
  //  template <class T> // 2.1.2002/Marko Muutin static-funktioksi cpp-tiedostoon MSVC-k��nt�j�n virheen takia.
  //  void StrokeBasic(float theX1, float theY1, float theX2, float theY2,
  //		   int r, int g, int b, int a);
  
  //  template <class T> // 2.1.2002/Marko Muutin static-funktioksi cpp-tiedostoon MSVC-k��nt�j�n virheen takia.
  //  void StrokeBasic(float theX1, float theY1,
  //		   float theX2, float theY2,
  //		   NFmiColorTools::Color theColor);
  
  // Composite low level drivers for each blending rule
  
  //  template <class T> // 2.1.2002/Marko Muutin static-funktioksi cpp-tiedostoon MSVC-k��nt�j�n virheen takia.
  //  void Composite(const NFmiImage & theImage,
  //		 int theX, int theY, float theAlpha);
  
};

#endif // _NFMIIMAGE_H

// ----------------------------------------------------------------------