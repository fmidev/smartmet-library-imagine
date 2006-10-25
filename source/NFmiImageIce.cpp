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
#include <vector>

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
	  long next_record_offset;
	  char white_pixel_limit;
	  char black_pixel_limit;
	  short entries;
	};


using namespace std;
namespace Imagine
{
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

	int version = 1;
    int bits_per_pixel = 8;

    ICEFileHeader h;
	ICEColortableHeader c;

	string magic = "JAA";
	magic.copy(h.magic, 3);

	if (version == 1) {
 	   h.version = 0x31;
	   h.length_or_offset = sizeof(h)+sizeof(c)+(itsWidth*itsHeight*bits_per_pixel);
	}
	else {
	  h.version = 0x32;
	  h.length_or_offset = sizeof(h)+sizeof(c);
	}
	string datum = "881012";
	datum.copy(h.datum, 6);

	h.compressiontype = 0;        // not compressed
	
	int stripped_pos = theFileName.rfind("/", 40);
	theFileName.copy(h.imagename, theFileName.length(), stripped_pos+1);    // strip path

	// write the image dimensions
	h.imagex =  itsWidth;
    h.imagey = itsHeight;
	h.bits_per_pixel = 8;

	string channel = "NOT USED\0\0\0\0\0\0\0\0";
	channel.copy(h.channel,16);

	h.calibrated = 0;

    h.minimum_pixel_value = 0;
    h.minimum_physical_value = 0;
    h.maximum_pixel_value = 255;
    h.maximum_physical_value = 2;
    h.pixel_size_at_61_40 = 0x8e960908;
    h.projection = 2;
	
	h.left_longitude = 0;
	h.north_latitude = 0;

	// write header
	fwrite(&h, sizeof(h), 1, out);

	// version 2 uses colortable header. pad nulls for version 1 
	if (version == 2) {
	   fwrite(&c, sizeof(c), 1, out);
	} else {
	  vector<unsigned char> null;
	  null.assign(sizeof(c), 0);
	  fwrite(&null, sizeof(c), 1, out);
	}

    // version 1 produces grayscale images  
	if (version == 1) {
		for(int j=0; j<itsHeight; j++)
		  for(int i=0; i<itsWidth; i++) {
			 int intensity = Imagine::NFmiColorTools::Intensity((*this)(i,j));
			 const unsigned char byte = intensity & 0xFF;
			 fwrite( &byte, sizeof(byte), 1, out);
		  }
	} else {
	// version 2 produces color images 
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
	}
	
	

  }

} // namespace Imagine
  
// ======================================================================
  
