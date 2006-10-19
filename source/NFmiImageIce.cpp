// ======================================================================
//
// NFmiImage addendum - ICE reading and writing
// ======================================================================

#include "NFmiImage.h"
#include "NFmiStringTools.h"

#include <set>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;


namespace Imagine
{
  // ----------------------------------------------------------------------
  // Read ICE image
  // ----------------------------------------------------------------------
  /*
  void NFmiImage::ReadICE(FILE *in)
  {
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
  */  
  // ----------------------------------------------------------------------
  // Write ICE image
  // ----------------------------------------------------------------------

  void NFmiImage::WriteICE(FILE *out) const
  {

	struct ICEFileHeader {
 	  char magic[3];
	  char version;
	  char datum[6];
      long colortable;
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
	  long next_record_offset;
	  char white_pixel_limit;
	  char black_pixel_limit;
	  short entries;
	};


    ICEFileHeader h;
	ICEColortableHeader c;


    strcpy(h.magic, "JAA");
	strcpy(&h.version, "2");
    strcpy(h.datum, "881012");
	h.compressiontype = 0;        // not compressed
	strcpy(h.imagename, "imagename");

	// write the image dimensions
	h.imagex =  itsWidth;
    h.imagey = itsHeight;
	h.bits_per_pixel = 8;
	strcpy(h.channel, "XXYYZZ");
	h.calibrated = 0;

	/*
	memcpy(&tail,  {0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0xf5,0x3d,0x9d,0x00,0x47,
							  0xbf,0xc3,0x03,0x6d,0x78,0x01,0x00,0x00,0x00,0x00,0x00,0x52,0x41,0x44,0x41,0x52,0x53,0x00,0x00,0x00,0x00,0x73,
							  0x74,0x64,0x61,0x74,0x65,0x73,0x74,0x74,0x69,0x6d,0x65,0x0f,0x00,0x00,0x00,0xff,0x00,0xff,0xff};
*/

	fwrite(&h, sizeof(h), 1, out);
	fwrite(&c, sizeof(h), 1, out);

	// Then write the image data itself

	for(int j=0; j<itsHeight; j++)
	  for(int i=0; i<itsWidth; i++)
		{
		  const NFmiColorTools::Color color = (*this)(i,j);
		  const unsigned char red = NFmiColorTools::GetRed(color);
		  const unsigned char green = NFmiColorTools::GetGreen(color);
		  const unsigned char blue = NFmiColorTools::GetBlue(color);
		  fwrite(&red, sizeof(red), 1, out);
		  fwrite(&green, sizeof(red), 1, out);
		  fwrite(&blue, sizeof(red), 1, out);
		}
  }

} // namespace Imagine
  
// ======================================================================
  
