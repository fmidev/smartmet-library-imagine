// ======================================================================
//
// NFmiImage addendum - TIFF reading and writing
// ======================================================================

#include "NFmiImage.h"
#include "NFmiStringTools.h"
#include "geo_config.h"
#include "xtiffio.h"


#include <set>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;
namespace Imagine
{
  // ----------------------------------------------------------------------
  // Read GTIFF image
  // ----------------------------------------------------------------------

  void NFmiImage::ReadGTIFF(TIFF *in)
  {
	uint32 w, h;
    uint32 npixels;
    uint32* raster;
        
    TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(in, TIFFTAG_IMAGELENGTH, &h);
    npixels = w * h;
    raster = static_cast<uint32*>(_TIFFmalloc(npixels * sizeof (uint32)));
    if (raster != NULL) {
       if (TIFFReadRGBAImage(in, w, h, raster, 0)) {
		  // mirroroidaan scanlinet, sillä TIFFReadRGBAImage asettaa 0,0:n vasempaan alakulmaan 

		  uint32 *wrk_line = static_cast<uint32*>(_TIFFmalloc(w * sizeof (uint32)));
		  for( uint32 row = 0; row < h / 2; row++ ) {
			uint32	*top_line, *bottom_line;

			top_line = raster + w * row;
			bottom_line = raster + w * (h-row-1);

			_TIFFmemcpy(wrk_line, top_line, 4*w);
			_TIFFmemcpy(top_line, bottom_line, 4*w);
			_TIFFmemcpy(bottom_line, wrk_line, 4*w);
		  }
		  _TIFFfree( wrk_line );

 		  int width = static_cast<int>(w);
 		  int height = static_cast<int>(h);

		  Allocate(width,height);
		  for(int j=0; j<height; j++) 
			for(int i=0; i<width; i++) {
			  uint32 color = raster[i+j*w];
			  // data 32-bit ABGR -paketteja 
			  const int a = color & 0xFF000000;
			  const int b = color & 0x00FF0000;
			  const int g = color & 0x0000FF00;
			  const int r = color & 0x000000FF;
			  (*this)(i,j) = NFmiColorTools::MakeColor(r,g,b,a);
			}
        }
        _TIFFfree(raster);
	}
  }

  // ----------------------------------------------------------------------
  // Write GTIFF image
  // ----------------------------------------------------------------------

  void NFmiImage::WriteGTIFF(TIFF *out) const
  {
	

  }

} // namespace Imagine
  
// ======================================================================
  
