// ======================================================================
//
// NFmiImage addendum - GIF reading and writing
//
// History:
//
// 26.10.2001 Mika Heiskanen
//
//	Implemented writing based on ppmtogif.c and coders/gif.c
//
// ======================================================================
#ifdef WIN32
#pragma warning(disable : 4786) // poistaa n kpl VC++ k‰‰nt‰j‰n varoitusta
#endif

#include "NFmiImage.h"
#include "NFmiEsriBuffer.h"

#include <set>
#include <map>
#include <iostream> // 2.1.2002/Marko cerr vaatii t‰m‰n MSVC-k‰‰nt‰j‰ss‰.
#include <algorithm>

// Maximum number of colors allowed in a GIF file

const int kFmiMaxGifColors = 256;
using namespace std;
// ----------------------------------------------------------------------
// Read GIF image
// ----------------------------------------------------------------------

void NFmiImage::ReadGIF(FILE *out)
{
}

// ----------------------------------------------------------------------
// Write GIF image
// ----------------------------------------------------------------------

// ImageMagick defines

#define MaxCode(number_bits) ((1<<(number_bits))-1)
#define MaxHashTable 5003
#define MaxGIFBits 12
#define MaxGIFTable (1<<MaxGIFBits)
#define GIFOutputCode(code) \
{ \
  if (bitsnow > 0) \
    datum|=(static_cast<long>(code)<< bitsnow); \
  else \
    datum=static_cast<long>(code); \
  bitsnow+=number_bits; \
  while (bitsnow >= 8) \
  { \
    packet[byte_count++]=(datum & 0xff); \
    if (byte_count >= 254) \
      { \
	fputc(byte_count,out); \
	fwrite(packet.c_str(),1,byte_count,out); \
        byte_count=0; \
      } \
    datum>>=8; \
    bitsnow-=8; \
  } \
  if (free_code > max_code)  \
    { \
      number_bits++; \
      if (number_bits == MaxGIFBits) \
        max_code=MaxGIFTable; \
      else \
        max_code=MaxCode(number_bits); \
    } \
}


void NFmiImage::WriteGIF(FILE *out) const
{
  int i,j;
  
  // Establish whether a palette version can be made
  
  set<NFmiColorTools::Color> theColors;
  
  // Opaque threshold must be > 0, we have binary transparency
  
  int opaquethreshold = itsAlphaLimit;
  if(opaquethreshold<0) opaquethreshold = NFmiColorTools::MaxAlpha/2;
  
  bool ignorealpha = !itsSaveAlphaFlag;
  
  bool overflow = AddColors(theColors,kFmiMaxGifColors,opaquethreshold,ignorealpha);
  
  // If overflow occurred, we must quantize the image
  
  if(overflow)
	{
	  cerr << "Error: Quantization not implemented in WriteGIF" << endl;
	  return;
	}
  
  // Build the palette
  
  vector<NFmiColorTools::Color> colors;
  map<NFmiColorTools::Color,int> colormap;
  set<NFmiColorTools::Color>::const_iterator iter;
  
  bool hastransparent = false;
  int num_colors = 0;
  
  for(iter=theColors.begin(); iter!=theColors.end(); iter++)
	{
	  if(NFmiColorTools::GetAlpha(*iter)>0)
		hastransparent = true;
	  else
		{
		  colormap.insert(make_pair(*iter,num_colors++));
		  colors.push_back(*iter);
		}
	}
  
  int total_colors = num_colors;
  if(hastransparent)
	total_colors++;
  
  // The number of bits needed per pixel. Note that we must use
  // total_colors instead of num_colors, in case there is
  // a transparent color index.
  
  int bits;
  for(bits=1; bits<8; bits++)
	if(1<<bits >= total_colors) break;
  
  // Write the magic number
  
  string header = "";
  header.reserve(1000);	// An overestimated header size (3*256+n)
  
  if(hastransparent)
	header += "GIF89a";
  else
	header += "GIF87a";
  
  // Image width and height
  
  header += NFmiEsriBuffer::LittleEndianShort(itsWidth);
  header += NFmiEsriBuffer::LittleEndianShort(itsHeight);
  
  // Mode flags
  
  unsigned char mode = 0x80;	// Global color map on
  mode |= (8-1)<<4;		// Resolution
  mode |= bits-1;		// Bits per pixel
  
  header += mode;
  
  // Background color chosen to be color 0
  
  header += '\0';
  
  // Future expansion byte
  
  header += '\0';
  
  // The global color map
  
  unsigned char r,g,b;
  
  for(i=0; i<num_colors; i++)
	{
	  r = static_cast<unsigned char>(NFmiColorTools::GetRed(colors[i]));
	  g = static_cast<unsigned char>(NFmiColorTools::GetGreen(colors[i]));
	  b = static_cast<unsigned char>(NFmiColorTools::GetBlue(colors[i]));
	  header += r;
	  header += g;
	  header += b;
	}
  
  // Pad with zeros to get 2^bits colors. Note that using
  // header += "\x00\x00\x00" would not work, since it is
  // interpreted as a c-string and thus terminates immediately.
  // Using string("\x00\x00\x00") would not work either.
  
  const int mapsize = 1 << bits;
  for(i=num_colors; i<mapsize; i++)
	for(j=0; j<3; j++)
	  header += '\x00';		// This wouldn't work: += "\x00\x00\x00
  
  // Write GIF89a extension if necessary
  
  if(hastransparent)
	{
	  header += '\x21';
	  header += '\xf9';
	  header += '\x04';
	  header += '\x01';					// matte flag
	  header += NFmiEsriBuffer::LittleEndianShort(0);		// delay=0
	  header += static_cast<unsigned char>(num_colors);	// trans. idx
	  header += '\x00';
	}
  
  // Comment block would go here - we don't output one
  
  // Initial code size
  
  unsigned char initcodesize = std::max(bits,2);
  
  header +=  ',';					// image separator
  header += NFmiEsriBuffer::LittleEndianShort(0);	// left offset
  header += NFmiEsriBuffer::LittleEndianShort(0);	// top offset
  header += NFmiEsriBuffer::LittleEndianShort(itsWidth);
  header += NFmiEsriBuffer::LittleEndianShort(itsHeight);
  header += '\x00';
  header += initcodesize;				// initial code size
  
  fwrite(header.c_str(),1,header.size(),out);
  
  // Write the raster itself
  
  // Allocate encoder tables
  
  string packet(256,'\0');
  string hash_suffix(MaxHashTable,'\0');
  
  vector<short> hash_code(MaxHashTable,0);
  vector<short> hash_prefix(MaxHashTable,0);
  
  // Initialize GIF encoder
  
  const short data_size = std::max(bits,2)+1;
  const short clear_code = 1<<(data_size-1);
  const short end_code = clear_code+1;
  
  int number_bits = data_size;
  short max_code = MaxCode(number_bits);
  short free_code = clear_code+2;
  int byte_count=0;
  long datum=0;
  int bitsnow=0;
  
  GIFOutputCode(clear_code);
  
  // Encode pixels
  
  short waiting_code=0;
  
  for(int pixpos=0; pixpos<itsWidth*itsHeight; pixpos++)
	{
	  // Get the color, simplify it
	  
	  NFmiColorTools::Color c = itsPixels[pixpos];
	  c = NFmiColorTools::Simplify(c,opaquethreshold,ignorealpha);
	  
	  // Convert to colormap index
	  
	  int a = NFmiColorTools::GetAlpha(c);
	  short index = (a==0 ? colormap[c] : num_colors);
	  
	  // Probe hash table
	  
	  int k = (static_cast<int>(index) << (MaxGIFBits-8))+waiting_code;
	  
	  if(k>=MaxHashTable)
		k-=MaxHashTable;
	  
	  bool next_pixel = false;
	  int displacement=1;
	  
	  if(hash_code[k]>0)
		{
		  if((hash_prefix[k]==waiting_code) && (hash_suffix[k]==index))
			{
			  waiting_code = hash_code[k];
			  continue;
			}
		  if(k!=0)
			displacement = MaxHashTable-k;
		  for( ; ; )
			{
			  k -= displacement;
			  if(k<0)
				k+=MaxHashTable;
			  if(hash_code[k]==0)
				break;
			  if((hash_prefix[k]==waiting_code) && (hash_suffix[k]==index))
				{
				  waiting_code = hash_code[k];
				  next_pixel = true;
				  break;
				}
			}
		  if(next_pixel)
			continue;
		}
	  GIFOutputCode(waiting_code);
	  if(free_code<MaxGIFTable)
		{
		  hash_code[k] = free_code++;
		  hash_prefix[k] = waiting_code;
		  hash_suffix[k] = index;
		}
	  else
		{
		  for (k=0; k < MaxHashTable; k++)
			hash_code[k]=0;
		  free_code = clear_code+2;
		  GIFOutputCode(clear_code);
		  number_bits = data_size;
		  max_code = MaxCode(number_bits);
		}
	  waiting_code = index;
	}
  
  // Flush out the buffered code
  
  GIFOutputCode(waiting_code);
  GIFOutputCode(end_code);
  if (bitsnow > 0)
	packet[byte_count++]=(datum & 0xff);
  
  // Flush accumulated data.
  
  if (byte_count > 0)
	{
	  fputc(byte_count,out);
	  fwrite(packet.c_str(),1,byte_count,out);
	  byte_count=0;
	}
  
  // End GIF writing
  
  fputc(0,out);	// zero-length packet to end data
  fputc(';',out);	// GIF terminator
}

// ======================================================================
