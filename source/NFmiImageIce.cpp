// ======================================================================
//
// NFmiImage addendum - ICE writing. Reading not implemented
// ======================================================================

#include "NFmiImage.h"
#include "NFmiStringTools.h"
#include "NFmiMercatorArea.h"

#include <set>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include "icehdr.h"

	struct ICEFileHeader {
 	  char magic[3];
	  char version;
	  char datum[6];
      long length_or_offset;      // version 1: filesize, version 2: pointer to colortable
      char compressiontype;
      char filler1;
	  char imagename[40];
      long imagex;
	  long imagey;
	  long bits_per_pixel;
      char channel[16];
      char calibrated;
      char filler2;
      short minimum_pixel_value;
      long minimum_physical_value; // times * 1000
      short maximum_pixel_value;
      long maximum_physical_value; // times * 1000
	  char projection;
	  char filler3;
      long left_longitude;
	  long north_latitude;
	  long pixel_size_at_61_40;
	  long center_longitude;
	  char satellite_name[26];
	  long orbit_number;
	  char start_date;
	  char start_time;
	  short duration_in_seconds;
	  short pixel_value_null_area;
	  short pixel_different_values;
	  short lut_info;
	  char lut_data[52];
	  char class_names[12];
	  short offset_image_x;
	  short offset_image_y;
	  char filler4;
	  char image_identifier[12];
	};

	struct ICEColortableHeader {
	  char tag[2];
	  LONG next_record_offset;
	  UBYTE white_pixel_limit;
	  UBYTE black_pixel_limit;
	  WORD entries;
	};


using namespace std;
namespace Imagine
{
  const string BigEndianInt(int theValue)
  {
        int value = theValue;

        // Initialize string by reinterpreting 4 value bytes as characters

        unsigned char *svalue = reinterpret_cast<unsigned char *>(&value);

        // string tmp(svalue,4);  // Perkele kun ei toimi g++:ssa

        string tmp(4,'\0');
        for(int i=0; i<4; i++)
          tmp[i] = svalue[i];

        swap(tmp[0],tmp[3]);
        swap(tmp[1],tmp[2]);
        return tmp;

  }

  // ----------------------------------------------------------------------
  // Read ICE image
  // ----------------------------------------------------------------------

  void NFmiImage::ReadICE(FILE *in)
  {
    //coming really soon

	const int maxbufsize = 1024;
	char buffer[maxbufsize+1];

	// Safety against empty lines in future fgets calls
	buffer[0] = ' ';

	// The first line contains the P6 header

	fseek(in,3,SEEK_CUR);

	// Then there may be multiple comment lines

	fgets(buffer,maxbufsize,in);
	while(buffer[0] == '#' || buffer[0] == '\n')
	  {
		fgets(buffer,maxbufsize,in);
		if(strlen(buffer) == 0)
		  throw runtime_error("Invalid PNM image data");
	  }
	
	// Then there are the dimensions X Y

	int width, height;
	{
	  istringstream input(buffer);
	  input >> width >> height;
	  if(input.bad())
		throw runtime_error("Failed to read PNM dimensions");
	  if(width <= 0 || height <= 0)
		throw runtime_error("PNM dimensions must be positive");
	}

	// And the size of each color component (normally 255)

	int colorsize;
	{
	  fgets(buffer,maxbufsize,in);
	  istringstream input(buffer);
	  input >> colorsize;
	  if(input.bad())
		throw runtime_error("Failed to read PNM color size");
	  if(colorsize != 255)
		throw runtime_error("Only colorsize 255 is supported for PNM images (size="+NFmiStringTools::Convert(colorsize)+")");
	}

	// And then there is the raw data

	Allocate(width,height);

	for(int j=0; j<height; j++)
	  for(int i=0; i<width; i++)
		{
		  const int r = fgetc(in);
		  const int g = fgetc(in);
		  const int b = fgetc(in);
		  if(r == EOF || g == EOF || b == EOF)
			throw runtime_error("PNM data ends abruptly");
		  (*this)(i,j) = NFmiColorTools::MakeColor(r,g,b); 
		}

  }

  // ----------------------------------------------------------------------
  // Write ICE image
  // ----------------------------------------------------------------------

  void NFmiImage::WriteICE(FILE *out, const string & theFileName) const
  {

	//	ICEFileHeader h;
	ICEColortableHeader icec;

    image_header_jaa_tr iceh;

	string magic = "JAA1";
	magic.copy(iceh.header_id, 4);

	string datum = "041007";
	datum.copy(iceh.header_date, 6);

    cout << "size of header " << sizeof(iceh) << endl;

    iceh.file_len_1 = static_cast<long>((sizeof(iceh)+itsWidth*itsHeight*3) / 512);
	//    iceh.file_len_1 = static_cast<UWORD>(file_len & 0x0000FFFF);
	//    iceh.file_len_2 = static_cast<UWORD>(file_len & 0xFFFF0000);

    cout << "offset: " << iceh.file_len_1 << endl;

	iceh.compression = NOT_COMPRESSED;
	
	int stripped_pos = theFileName.rfind("/", 40);
    string pad_space;
    pad_space.assign(" ", 40);
    pad_space.copy(iceh.image_id, 40);
	theFileName.copy(iceh.image_id, theFileName.length(), stripped_pos+1);    // strip path
    iceh.image_id[39] = 0;    

	// write the image dimensions
	iceh.elements =  itsWidth;
    iceh.lines = itsHeight;
	iceh.bits_per_pixel = 8;

	string channel = "MODIS Truecolor ";
	channel.copy(iceh.channel_name,16);
   
	iceh.phys_switch = 1;

    iceh.unit = ALBEDO;
    iceh.min_pix_val = 0;
    iceh.min_phys_val_1 = 0;
    iceh.min_phys_val_2 = 0;
    iceh.max_pix_val = 255;
    iceh.max_phys_val = 1000;
    iceh.zerol_1 = static_cast<UWORD>(0.0);
    iceh.zerol_2 = static_cast<UWORD>(0.0);

	//    h.pixel_size_at_61_40 = 0x8e960908;
    NFmiMercatorArea area(itsTopleft,itsBottomright, NFmiPoint(0,0), NFmiPoint(itsWidth,itsHeight));
    area.Init(true);
    NFmiPoint topleft = area.LatLonToWorldXY(itsTopleft);
    NFmiPoint bottomright = area.LatLonToWorldXY(itsBottomright);

    iceh.ptype = MERCATOR;
    cout << itsTopleft << endl;

    long longi = static_cast<long>(itsTopleft.X() * 1000000);
    long lati = static_cast<long>(itsTopleft.Y() * 1000000);

    cout << longi << endl;
    cout << lati << endl;     

	iceh.longit_1 = static_cast<UWORD>(longi & 0x0000FFFF);
	iceh.longit_2 = static_cast<UWORD>(longi & 0xFFFF0000);

	iceh.latit_1 = static_cast<UWORD>(lati & 0x0000FFFF);
	iceh.latit_2 = static_cast<UWORD>(lati & 0xFFFF0000);
    
    cout << iceh.longit_1 << endl;
    cout << iceh.latit_1 << endl;

	iceh.zerol_1 = 0;
	iceh.zerol_2 = 0;

    iceh.orbit_no = 0;
    iceh.duration = 0;
    

    // first eccentricity of the ellipsoid:
    // e^2 = 2f-f^2, miss� f=1/298.257223563
    // => e = 0,081819191
    // 
    // mercator_scaling_factor:
    // m = sqrt(1-(e*sin(61'40))^2 / cos(61'40)
    // => m = 2,9797987
    //

    long resolution = static_cast<long>(((bottomright.X() - topleft.X()) / itsWidth) * 1000);
    cout << "resolution " << resolution << endl;
    // iceh.resol_1 = static_cast<UWORD>(resolution & 0x0000FFFF);
    // iceh.resol_2 = static_cast<UWORD>(resolution & 0xFFFF0000);

    string satellite = "Terra\0";
	satellite.copy(iceh.satellite, 6);
    iceh.null_value = 0;
    iceh.Pix_value_cnt = 256;
    iceh.LUT_info = -1;

    string date = "061208";
    date.copy(iceh.start_date, 6);
    string time = "084700";
    time.copy(iceh.start_time,6);

	// write header
	fwrite(&iceh, sizeof(iceh), 1, out);

    // write data
	for(int j=0; j<itsHeight; j++)
      for(int i=0; i<itsWidth; i++) {
	     const NFmiColorTools::Color color = (*this)(i,j);
	     const unsigned char red = NFmiColorTools::GetRed(color);
	     const unsigned char green = NFmiColorTools::GetGreen(color);
	     const unsigned char blue = NFmiColorTools::GetBlue(color);

		 fwrite(&red, sizeof(red), 1, out);
		 fwrite(&green, sizeof(green), 1, out);
		 fwrite(&blue, sizeof(blue), 1, out);
	  }

    // optional trailer
    string tag = "T1";
    tag.copy(icec.tag, 2);
    icec.next_record_offset = -1;
    icec.white_pixel_limit = 255;
    icec.black_pixel_limit = 0;
    icec.entries = 255;
    fwrite(&icec, sizeof(icec), 1, out);

    cout << "long " << sizeof(long) << endl;
    cout << "LONG " << sizeof(LONG) << endl;
    cout << "UWORD" << sizeof(UWORD) << endl;

    unsigned char triplet = 0;
    for(int i=0; i<256; i++) {
 	  fwrite(&triplet, sizeof(triplet), 1, out);
	  fwrite(&triplet, sizeof(triplet), 1, out);
	  fwrite(&triplet, sizeof(triplet), 1, out);
      triplet++;
	}    
    unsigned char tail[7] = {0x54,0x32,0x00,0x00,0x00,0x00,0x00};
    fwrite(&tail, sizeof(tail), 1, out);


  }


	
} // namespace Imagine
 


 
// ======================================================================
  
