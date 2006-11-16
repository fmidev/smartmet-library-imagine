// ======================================================================
//
// NFmiImage addendum - TIFF reading and writing
// ======================================================================

#include "NFmiImage.h"
#include "NFmiStringTools.h"
#include "geo_config.h"
#include "xtiffio.h"
#include "geotiffio.h"
#include "geotiff.h"
#include "geo_normalize.h"
#include "geovalues.h"
#include "tiffio.h"
#include "NFmiYKJArea.h"
#include "NFmiLatLonArea.h"
#include "NFmiGeoShape.h"

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
			  //			  const int a = (color & 0xFF000000) >> 24;
			  const int b = (color & 0x00FF0000) >> 16;
			  const int g = (color & 0x0000FF00) >> 8;
			  const int r = color & 0x000000FF;
			  (*this)(i,j) = NFmiColorTools::MakeColor(r,g,b);
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
    uint32 width = itsWidth;
    uint32 height = itsHeight;
    uint32	rowsperstrip = static_cast<uint32>(-1);
	uint32 row;

    int	pixel_count = static_cast<int>(width*height);
	uint32 *raster = static_cast<uint32*>(_TIFFmalloc(pixel_count * sizeof (uint32)));

    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);

    rowsperstrip = TIFFDefaultStripSize(out, rowsperstrip);
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB); 
	TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

	NFmiLatLonArea area(itsTopleft,itsBottomright, NFmiPoint(0,0), NFmiPoint(itsWidth,itsHeight)); 
	area.Init(true);

	NFmiPoint topleft = area.LatLonToWorldXY(itsTopleft);
	NFmiPoint bottomright = area.LatLonToWorldXY(itsBottomright);

	if ( 0 ) {
       cout << "topleft x " << static_cast<long>(topleft.X()) << endl;
	   cout << "topleft y " << static_cast<long>(topleft.Y()) << endl;
	   cout << "bottomright x " << static_cast<long>(bottomright.X()) << endl;
	   cout << "bottomright y " << static_cast<long>(bottomright.Y()) << endl << endl;

       cout << "ToXY " << (area.ToXY(itsBottomright)) << endl;

	   cout << "xscale " << area.XScale() << endl;
	   cout << "yscale " << area.YScale() << endl;

	}

	// GeoTiff headers
	GTIF *gtif = GTIFNew(out);

	GTIFKeySet(gtif, GTModelTypeGeoKey, TYPE_SHORT, 1, ModelTypeProjected);
    GTIFKeySet(gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1, RasterPixelIsArea);
	GTIFKeySet(gtif, GTCitationGeoKey, TYPE_ASCII, 22, "Imagine GeoTIFF Writer");
	GTIFKeySet(gtif, GeogGeodeticDatumGeoKey, TYPE_SHORT, 1, Datum_WGS84);
	GTIFKeySet(gtif, GeogLinearUnitsGeoKey, TYPE_SHORT, 1, Linear_Meter);
	GTIFKeySet(gtif, GeogAngularUnitsGeoKey, TYPE_SHORT, 1, Angular_Degree);
	GTIFKeySet(gtif, GeogSemiMajorAxisGeoKey, TYPE_DOUBLE, 1, 6370997);
    GTIFKeySet(gtif, ProjCoordTransGeoKey, TYPE_SHORT, 1, CT_Mercator);
	GTIFKeySet(gtif, ProjFalseEastingGeoKey, TYPE_DOUBLE, 1, 0);
	GTIFKeySet(gtif, ProjFalseNorthingGeoKey, TYPE_DOUBLE, 1, 0);
  
    double centlon = abs(itsBottomright.X() - itsTopleft.X()) / 2;
	double centlat = abs(itsBottomright.Y() - itsTopleft.Y()) / 2;

    GTIFKeySet(gtif, ProjCenterLongGeoKey, TYPE_DOUBLE, 1, centlon);
    GTIFKeySet(gtif, ProjCenterLatGeoKey, TYPE_DOUBLE, 1, centlat);

	/* geotiff tiepoint matriisi
	   Tx = X - I * Sx; mallin x - image x * xpixelscale 
	   Ty = Y + J * Sy; mallin y + image y * ypixelscale
       Tz = Z - K * Sz, (2D:ssä ei tarvita)
	*/

	double tiePoints[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    double pixelScale[3] = { 0.0, 0.0, 0.0 };

	tiePoints[3] = static_cast<double>(topleft.X() - area.ToXY(itsTopleft).X() * (1/area.XScale()));
	tiePoints[4] = static_cast<double>(bottomright.Y() + area.ToXY(itsBottomright).Y() * (1 / area.YScale()));

	pixelScale[0] = 1/(area.XScale());
	pixelScale[1] = 1/(area.YScale());

	TIFFSetField( out, TIFFTAG_GEOTIEPOINTS, 6, tiePoints );
    TIFFSetField( out, TIFFTAG_GEOPIXELSCALE, 3, pixelScale );

	GTIFWriteKeys(gtif);


	GTIFFree(gtif);

    for(int j=0; j<itsHeight; j++) 
		for(int i=0; i<itsWidth; i++) {
		  const NFmiColorTools::Color color = (*this)(i,j);
		  const unsigned char b = NFmiColorTools::GetBlue(color);
		  const unsigned char g = NFmiColorTools::GetGreen(color);
		  const unsigned char r = NFmiColorTools::GetRed(color);
		  raster[i+j*itsWidth] = (r + (g << 8) + (b << 16) + (0xFF << 24));
	}

	for( row = 0; row < height; row += rowsperstrip )
    {
        unsigned char * raster_strip;
        int	rows_to_write;
        int	bytes_per_pixel;

        raster_strip = reinterpret_cast<unsigned char*>(raster + row * width);
        bytes_per_pixel = 4;

        if( row + rowsperstrip > height )
            rows_to_write = height - row;
        else
            rows_to_write = rowsperstrip;

        if( TIFFWriteEncodedStrip( out, row / rowsperstrip, raster_strip,
                             bytes_per_pixel * rows_to_write * width ) == -1 )
        {
            _TIFFfree( raster );
			return;
        }
    }

    _TIFFfree( raster );



  }

} // namespace Imagine
  
// ======================================================================
  
