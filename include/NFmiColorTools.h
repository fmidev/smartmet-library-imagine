// ======================================================================
/*!
 * \file NFmiColorTools.h
 * \brief Tools to handle RGBA colors.
 *
 * \b History:
 *
 * \li 21.08.2001 Mika Heiskanen\par
 * Implemented.
 *
 * \li 14.09.2001 Mika Heiskanen\par
 * Completed set of blending rules.
 *
 * \li 29.09.2001 Mika Heiskanen\par
 * Added \c kFmiColorOnOpaque and \c kFmiColorOnTransparent rules.
 *
 * \li 06.12.2001 Mika Heiskanen\par
 * Separated color blending templates into NFmiColorBlend.h.
 */
// ======================================================================

// ======================================================================
/*!
 * \class NFmiColorTools
 *
 * This provides methods to operator on RGBA colors by providing static
 * member functions. Hence the members can be and are intended to be used
 * as in
 *
 * \code
 * NFmiColorTools::Color color = NFmiColorTools::MakeColor(red,green,blue,alpha);
 * \endcode
 *
 * The better alternative would be to use a \c namespace, but they do not
 * work properly with the GNU C++ compiler.
 *
 * <b>Notes about colour blending with Porter-Duff rules:</b>
 *
 * A general Porter-Duff rule is expressed with formulas
 *
 * \f$C_d = C_s F_s + C_d F_d\f$
 *
 * \f$A_d = A_s F_s + A_d F_d\f$
 *
 * where \f$C_s\f$, \f$C_d\f$ stand for source and destination colour
 * components, \f$A_s\f$, \f$A_d\f$ source and destination alphas.
 * \f$F_s\f$ ans \f$F_d\f$ denote the fractions used in the blending as
 * defined by each compositing rule.
 *
 * Note that here having alpha value 1 implies an opaque colour and
 * zero a transparent colour, while in code we actuall prefer to use 0 for
 * opaque and 127 for transparent.
 *
 * Hence in all formulas we must substitute
 *
 * \f$ A_s = (\alpha_{max} - \alpha)/\alpha_{max}\f$
 *
 * \f$ 1-A_s = \alpha/\alpha_{max}\f$
 *
 * Also, the rules apply in premultiplied RGBA space, where each component
 * value has already been premultiplied by the alpha. We do this explicitly
 * when applying the rules.
 *
 * The rules as found from for example Java2D documentation are described
 * below. The name on the left corresponds to the chosen name, the name
 * below it the logical description of the operation.
 *
 * \li \b Clear\par
 * Both the color and the alpha of destination are cleared. Neither the
 * source nor the destination is used as input.
 *
 * \li \b Copy\par
 * The source is copied to the destination. The destination is not used
 * as input.
 *
 * \li \b Keep\par
 * The destination is kept. Useless operation for us.
 *
 * \li \b Over\par
 * The source is composited over the destination. 
 *
 * \li \b Under\par
 * The destination is composited over the source and the result replaces the
 * destination. This is \b Over with the operands reversed.
 *
 * \li \b In\par
 * The part of the source lying inside of the destination replaces
 * the destination.
 *
 * \li \b KeepIn\par
 * The part of the destination lying inside of the source replaces the
 * destination. This is \b In with the operands reversed.
 *
 * \li \b Out\par
 * The part of the source lying outside of the destination replaces
 * the destination.
 *
 * \li \b KeepOut\par
 * The part of the destination lying outside of the source replaces
 * the destination. This is \b Out with the operands reversed.
 *
 * \li \b Atop\par
 * The part of the source inside the destination replaces the part inside
 * the destination.
 *
 * \li \b KeepAtop\par
 * The part of the destination inside the source replaces the part inside
 * the source in the destination. This is \b Atop reversed.
 *
 * \li \b Xor\par
 * Only non-overlapping areas of source and destination are kept.
 *
 * <b>Additional blending rules</b>
 *
 * In addition to Ported-Duff rules, we define miscellaneous blending-rule
 * type functions. Many of these are found for example from Imagemagick.
 *
 * \li \b Plus\par
 * Add RGBA values using Porter-Duff type rules.
 *
 * \li \b Minus\par
 * Substract RGBA values using Porter-Duff type rules.
 *
 * \li \b Add\par
 * Add RGBA values.
 *
 * \li \b Substract\par
 * Substract RGBA values.
 *
 * \li \b Multiply\par
 * Multiply RGBA values
 *
 * \li \b Difference\par
 * Absolute difference of RGBA values.
 *
 * \li \b Bumpmap\par
 * Adjust by intensity of source color.
 *
 * \li \b Dentmap\par
 * Adjust by intensity of destination color.
 *
 * \li \b CopyRed\par
 * Copy red component only.
 *
 * \li \b CopyGreen\par
 * Copy green component only.
 *
 * \li \b CopyBlue\par
 * Copy blue component only.
 *
 * \li \b CopyMatte\par
 * Copy opacity only.
 *
 * \li \b CopyHue\par
 * Copy hue component only.
 *
 * \li \b CopyLightness\par
 * Copy light component only.
 *
 * \li \b CopySaturation\par
 * Copy saturation component only.
 *
 * \li \b KeepMatte\par
 * Keep target matte only.
 *
 * \li \b KeepHue\par
 * Keep target hue only.
 *
 * \li \b KeepLightness\par
 * Keep target lightness only.
 *
 * \li \b KeepSaturation\par
 * Keep target saturation only.
 *
 * \li \b AddContrast\par
 * Enhance the contrast of target pixel.
 *
 * \li \b ReduceContrast\par
 * Reduce the contrast of target pixel.
 *
 * \li \b OnOpaque\par
 * Draw on opaque parts only.
 *
 * \li \b OnTransparent\par
 * Draw on transparent parts only.
 *
 * Note that ImageMagick Dissolve and Plus are equivalent.
 *
 */
// ======================================================================

#ifndef _NFMICOLORTOOLS_H
#define _NFMICOLORTOOLS_H

#include "NFmiDef.h"
#include <cmath>	// max,abs etc
#include <string>	// for color name definitions

//! Loki template to convert enumerated value to a class for dispatching
template <int v>
struct Blend2Type
{ enum { value = v }; };

//! Utilities for dealing with RGBA colours.

class _FMI_DLL NFmiColorTools
{
public:
  
  //! An RGBA colour is internally represented as an integer of the form 0xaarrggbb
  
  typedef int Color;
  
  // -------------------- special colors --------------------
  
  //! A missing color means the color is unknown.
  
  static const Color MissingColor; // 18.12.2001/Marko Moved static varible instantiation after class declaration
  
  //! A "no-color" means no color is to be used. Strokes and fills are cancelled.
  
  static const Color NoColor; // 18.12.2001/Marko Moved static varible instantiation after class declaration
  
  //! Helper variable to identify a transparent color.
  
  static const Color TransparentColor; // 18.12.2001/Marko Moved static varible instantiation after class declaration
  
  //! Helper variable to identify a black color
  
  static const Color Black; // 18.12.2001/Marko Moved static varible instantiation after class declaration
  
  // -------------------- range definitions --------------------
  
  //! RGB values are in range 0-255.
  
  static const int MaxRGB; // 18.12.2001/Marko Moved static varible instantiation after class declaration
  
  //! Alpha values are in range 0-127, as in the GD library.
  
  static const int MaxAlpha; // 18.12.2001/Marko Moved static varible instantiation after class declaration
  
  // -------------------- opacity definitions --------------------
  
  //! A color is opaque when its alpha value is zero.
  
  static const int Opaque; // 18.12.2001/Marko Moved static varible instantiation after class declaration
  
  //! A color is transparent when its alpha value is 127.
  
  static const int Transparent; // 18.12.2001/Marko Moved static varible instantiation after class declaration
  
  // -------------------- extracting color components --------------------
  
  //! Extract alpha component from a color.
  
  static inline int GetAlpha(Color c)	{ return (c>>24)&0x7F; }
  
  //! Extract red component from a color.
  
  static inline int GetRed(Color c)	{ return (c>>16)&0xFF; }
  
  //! Extract green component from a color.
  
  static inline int GetGreen(Color c)	{ return (c>>8)&0xFF; }
  
  //! Extract blue component from a color.
  
  static inline int GetBlue(Color c)	{ return (c&0xFF); }
  
  //! Extract RGB component from a color. Alpha will be zero (opaque color).
  
  static inline Color GetRGB(Color c)	{ return (c&0xFFFFFF); }
  
  // -------------------- building color from components --------------------
  
  //! Convert RGBA values into a color. No argument checking.
  
  static inline Color MakeColor(int r, int g, int b, int a=Opaque);
  
  //! Convert RGBA values into a color. Positive overflows are checked.
  
  static inline Color SafeColor(int r, int g, int b, int a=Opaque);
  
  //! Convert RGBA values into a color. Positive and negative overflows are checked.
  
  static inline Color SafestColor(int r, int g, int b, int a=Opaque);
  
  // -------------------- replacing individual color components --------------------
  
  //! Set new alpha component into the color.
  
  static inline Color ReplaceAlpha(Color c, int alpha)
  { return ((c&0x00FFFFFF)|(alpha<<24)); }
  
  //! Set new red component into the color.
  
  static inline Color ReplaceRed(Color c, int red)
  { return ((c&0xFF00FFFF)|(red<<16)); }
  
  //! Set new green component into the color.
  
  static inline Color ReplaceGreen(Color c, int green)
  { return ((c&0xFFFF00FF)|(green<<8)); }
  
  //! Set new blue component into the color
  
  static inline Color ReplaceBlue(Color c, int blue)
  { return ((c&0xFFFFFF00)|(blue)); }
  
  // -------------------- color intensity tools --------------------
  
  //! Intensity range is 0-255.
  
  static const int MaxIntensity; // 18.12.2001/Marko Moved static varible instantiation after class declaration
  
  //! Return intensity of given RGBA components.
  
  static inline int Intensity(int r, int g, int b, int a=0);
  
  //! Return intensity of given color.
  
  static inline int Intensity(Color c);
  
  // -------------------- color contrast tools --------------------
  
  // Adding contrast = increasing difference between light and dark colors
  // Reducing contrast = decreasing difference between light and dark colors
  
  //! Add contrast to given color
  
  static Color AddContrast(Color theColor)    { return Contrast(theColor,1); }
  
  //! Reduce contrast in the given color
  
  static Color ReduceContrast(Color theColor) { return Contrast(theColor,-1); }
  
  //! Modify contrast into the given direction
  
  static Color Contrast(Color theColor, int theSign);
  
  // -------------------- colour blending --------------------
  
  // Don't forget to update BlendNamesInit() when updating!
  
  //! All available colour blending rules.
  
  enum NFmiBlendRule { kFmiColorRuleMissing,
					   kFmiColorClear,
					   kFmiColorCopy,
					   kFmiColorKeep,
					   kFmiColorOver,
					   kFmiColorUnder,
					   kFmiColorIn,
					   kFmiColorKeepIn,
					   kFmiColorOut,
					   kFmiColorKeepOut,
					   kFmiColorAtop,
					   kFmiColorKeepAtop,
					   kFmiColorXor,
					   
					   kFmiColorPlus,
					   kFmiColorMinus,
					   kFmiColorAdd,
					   kFmiColorSubstract,
					   kFmiColorMultiply,
					   kFmiColorDifference,
					   kFmiColorCopyRed,
					   kFmiColorCopyGreen,
					   kFmiColorCopyBlue,
					   kFmiColorCopyMatte,
					   kFmiColorCopyHue,
					   kFmiColorCopyLightness,
					   kFmiColorCopySaturation,
					   kFmiColorKeepMatte,
					   kFmiColorKeepHue,
					   kFmiColorKeepLightness,
					   kFmiColorKeepSaturation,
					   kFmiColorBumpmap,
					   kFmiColorDentmap,
					   kFmiColorAddContrast,
					   kFmiColorReduceContrast,
					   
					   kFmiColorOnOpaque,
					   kFmiColorOnTransparent  };
  
  // -------------------- colour name conversion --------------------
  
  // Color name <-> color conversion.
  
  //! Convert colour name to colour.
  
  static Color ColorValue(const std::string & theName); // 18.12.2001/Marko Added the std::-namespace mark
  
  //! Convert colour to colour name.
  
  static const std::string ColorName(const Color & theColor); // 18.12.2001/Marko Added the std::-namespace mark
  
  //! Initialize colour name table.
  
  static void ColorNamesInit(void);
  
  //! A general purpose string to colour conversion
  
  static Color ToColor(const std::string & theColor); // 18.12.2001/Marko Added the std::-namespace mark
  
  //! Utility used by ToColor for converting hex to dec
  
  static Color HexToColor(const std::string & theHex); // 18.12.2001/Marko Added the std::-namespace mark
  
  // -------------------- blend name conversion --------------------
  
  //! Convert blending rule name to enum.
  
  static const NFmiBlendRule BlendValue(const std::string & theName); // 18.12.2001/Marko Added the std::-namespace mark
  
  //! Convert blending rule to string name.
  
  static const std::string BlendName(const NFmiBlendRule & theRule); // 18.12.2001/Marko Added the std::-namespace mark
  
  //! Initialize blending rule name table .
  
  static void BlendNamesInit(void);
  
  // -------------------- color space conversion --------------------
  
  //! Convert RGB values to HLS values.
  
  static void RGBtoHLS(int red, int green, int blue, float *h, float *l, float *s);
  
  //! Convert HLS values to RGB values
  
  static void HLStoRGB(float h, float l, float s, int *r, int *g, int *b);
  
  //! Utility used by HLS -- RGB conversion functions.
  
  static float hls_to_rgb_util(float m1, float m2, float h);
  
  // -------------------- miscellaneous --------------------
  
  //! Simplify a blending rule given the source color alpha value
  
  static NFmiBlendRule Simplify(NFmiBlendRule theRule, int alpha);
  
  //! Interpolate linearly in HLS space between 2 colors.
  
  static int Interpolate(Color c1, Color c2, float fraction);
  
  //! Simplify a color if possible.
  /*!
   * A color can be simplified when the user wishes to get rid of
   * the alpha channel, or to force it to extreme values.
   * A nonnegative opacity threshold opaquethreshold is used to divide colours
   * into opaque and transparent ones. An additional option ignorealpha
   * is used to indicate the alpha channel can be ignored completely, in which
   * case the color is made completely opaque.
   */
  
  static Color Simplify(Color c, int opaquethreshold, bool ignorealpha)
  {
    if(ignorealpha)
      return ReplaceAlpha(c,0);
    if(GetAlpha(c)==NFmiColorTools::MaxAlpha)
      return TransparentColor;
    if(opaquethreshold<0)
      return c;
    if(GetAlpha(c)<opaquethreshold)
      return ReplaceAlpha(c,0);
    else
      return TransparentColor;
  }
  
};


// ----------------------------------------------------------------------
/*!
 * Converting RGBA values into a color. No checks are made on the
 * input. The acceptable range of input for the RGB components
 * is 0 - MaxRGB, for the alpha component 0 - MaxAlpha.
 */
// ----------------------------------------------------------------------

inline NFmiColorTools::Color NFmiColorTools::MakeColor
(int r, int g, int b, int a)
{
  //  return reinterpret_cast<NFmiColorTools::Color> // 18.12.2001/Marko Visual C++ compiler didn't allow 'int' to 'int' reinterpret_cast
  return ((a<<24) + (r<<16) + (g<<8) + b);
}

// ----------------------------------------------------------------------
/*!
 * Converting RGBA values into a color. The acceptable range of input
 * for the RGB components is 0 - MaxRGB, for the alpha component 0 - MaxAlpha.
 * Any value greater than the upper limit is converted to the upper limit.
 * This makes building colours from positive definite linear combinations
 * safe.
 */
// ----------------------------------------------------------------------

inline NFmiColorTools::Color NFmiColorTools::SafeColor
(int r, int g, int b, int a)
{
  int rr = (r>NFmiColorTools::MaxRGB)   ? NFmiColorTools::MaxRGB   : r;
  int gg = (g>NFmiColorTools::MaxRGB)   ? NFmiColorTools::MaxRGB   : g;
  int bb = (b>NFmiColorTools::MaxRGB)   ? NFmiColorTools::MaxRGB   : b;
  int aa = (a>NFmiColorTools::MaxAlpha) ? NFmiColorTools::MaxAlpha : a;
  
  return NFmiColorTools::MakeColor(rr,gg,bb,aa);
  
  // For some reason g++ does not get this right.
  //  return reinterpret_cast<NFmiColorTools::Color>
  //    ( ((a>NFmiColorTools::MaxAlpha) ? NFmiColorTools::MaxAlpha : a)<< 24 +
  //      ((r>NFmiColorTools::MaxRGB  ) ? NFmiColorTools::MaxRGB   : r)<< 16 +
  //      ((g>NFmiColorTools::MaxRGB  ) ? NFmiColorTools::MaxRGB   : g)<< 8 +
  //      ((b>NFmiColorTools::MaxRGB  ) ? NFmiColorTools::MaxRGB   : b) );
}

// ----------------------------------------------------------------------
/*!
 * Converting RGBA values into a color. The acceptable range of input
 * for the RGB components is 0 - MaxRGB, for the alpha component 0 - MaxAlpha.
 * Any value outside the valid range is converted to the nearest valid
 * value (zero or the maximum).
 */
// ----------------------------------------------------------------------

inline NFmiColorTools::Color NFmiColorTools::SafestColor
(int r, int g, int b, int a)
{
  //  return reinterpret_cast<NFmiColorTools::Color> // 18.12.2001/Marko Visual C++ compiler didn't allow 'int' to 'int' reinterpret_cast
  return
    (
     (((a<0) ? 0 : (a>NFmiColorTools::MaxAlpha)? NFmiColorTools::MaxAlpha: a)<< 24) + // 18.12.2001/Marko VC++ wanted 'extra' parentheses
     (((r<0) ? 0 : (r>NFmiColorTools::MaxRGB)  ? NFmiColorTools::MaxRGB  : r)<< 16) +
     (((g<0) ? 0 : (g>NFmiColorTools::MaxRGB)  ? NFmiColorTools::MaxRGB  : g)<<  8) +
     ((b<0) ? 0 : (b>NFmiColorTools::MaxRGB)  ? NFmiColorTools::MaxRGB  : b)
     );
}

// ----------------------------------------------------------------------
/*!
 * Returns intensity of the given color in range 0-255. This is based
 * on a well known formula in floating point arithmetic. The integer
 * arithmetic version here is taken from ImageMagick.
 */
// ----------------------------------------------------------------------

inline int NFmiColorTools::Intensity(NFmiColorTools::Color c)
{
  return NFmiColorTools::Intensity(NFmiColorTools::GetRed(c),
								   NFmiColorTools::GetGreen(c),
								   NFmiColorTools::GetBlue(c));
}

inline int NFmiColorTools::Intensity(int r, int g, int b, int a)
{
  return ((9798*r+19235*g+3735*b)/32768L);
}

#endif // _NFMICOLORTOOLS_H

// ----------------------------------------------------------------------
