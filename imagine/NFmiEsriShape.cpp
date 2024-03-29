// ======================================================================
//
// Definition of a shape class suitable for storing information in
// ESRI shape files (*.shp) and the respective dBASE files (*.dbf).
//
// See the header file for documentation.
//
// ESRI Shapefile File Structure:
// ------------------------------
//
//	File Header
//	Record Header	Record Contents
//	Record Header	Record Contents
//	Record Header	Record Contents
//	Record Header	Record Contents
//	...		...
//
// ESRI Shapefile File Header Structure:
// -------------------------------------
//
//	Pos		Field		Value	Type	Endian
//
//	Byte 0		Magic Number	9994	int	big
//	Byte 8		Unused		0	int	big
//	Byte 12		Unused		0	int	big
//	Byte 16		Unused		0	int	big
//	Byte 20		Unused		0	int	big
//	Byte 24		File Length	length	int	big
//	Byte 28		Version		1000	int	little
//	Byte 32		Shape Type	type	int	little
//	Byte 36		Bounding Box	Xmin	double	little
//	Byte 44		Bounding Box	Ymin	double	little
//	Byte 52		Bounding Box	Xmax	double	little
//	Byte 60		Bounding Box	Ymax	double	little
//	Byte 68*	Bounding Box	Zmin	double	little
//	Byte 76*	Bounding Box	Zmax	double	little
//	Byte 84*	Bounding Box	Mmin	double	little
//	Byte 92*	Bounding Box	Mmax	double	little
//
// (*) Unused, with value 0.0, if not Measured or Z type
//
//
// ESRI Shapefile Record Header Structure:
// ---------------------------------------
//
//
//	Pos		Field		Value	Type	Endian
//
//	Byte 0		Record Number	recnum	int	big
//	Byte 4		Content Length	length	int	big
//
// The length is the length of the record contents in 16-bit units.
//
//
// ESRI Shapefile Record Content Structure
// ---------------------------------------
//
//	[See the source file for the element in question.]
//
//
// dBASE File Structure
// --------------------
//
//	Header
//	Fields Description
//	Terminator 0Dh
//	Record 1
//	Record 2
//	...
//
// dBASE Header Structure
// ----------------------
//
// The essential fields are marked with a * character
//
//	Pos	Type	Endian	Value	Field
//
//	*  0	byte	-	03h	Signature - file without DBT
//	   1	ymd	?	YYMMDD	Date of last update
//	*  4	int	little	nrec	Number of records
//	*  8	short	big	hlen	Length of full header (+1 for flag)
//	* 10	short	big	rlen	Length of each record
//	  14    [various not useful reserved fields]
//
// 32 bytes in total
//
// Note: Size of a field is 32, hence number of fields is (hlen-33)/32,
//	 where 33 = 32+1 for the header above + the trailing flag.
//       However, judging by shapelib code the flag may be missing, hence
//       we'll use formula (hlen-32)/32 and let integer rounding handle
//       the problem.
//
// dBASE Fields Structure
// ----------------------
//
//	Field 1		32 bytes
//	Field 2		32 bytes
//	...
//	Field N
//
// The individual field structure is
//
//	Pos	Type	Endian	Value	Field
//
//	0	char *	-	fname	Field name in ASCII terminated by 00h
//	11	char	-	ftype	Field type in ASCII
//	12	int	?	faddr	FoxPro field data address in memory
//
//	16	byte	-	flen	Field length
//	17	byte	-	dlen	Decimal count
//
//	or
//
//	16	short	big	flen	Field length (non number)
//
//	18	[various not useful fields]
//
//
// Relevant field types:
//
//	N	Number as ASCII text 18 chars long
//	F	Floating point, 20 digits
//	C	Ascii text

// History:
//
// 30.08.2001 Mika Heiskanen
//
//	Implemented
//
// ======================================================================

#ifdef __GNUC__
#if __GNUC__ < 3
#define PERKELEEN_296
#endif
#endif

#include "NFmiEsriShape.h"

#include "NFmiEsriMultiPatch.h"
#include "NFmiEsriMultiPointZ.h"
#include "NFmiEsriNull.h"
#include "NFmiEsriPointZ.h"
#include "NFmiEsriPolyLineZ.h"
#include "NFmiEsriPolygonZ.h"

#include <fmt/format.h>
#include <macgyver/Exception.h>
#include <newbase/NFmiFileSystem.h>
#include <newbase/NFmiSettings.h>
#include <newbase/NFmiTime.h>

#include <fstream>
#include <iomanip>

using namespace Imagine::NFmiEsriBuffer;  // Conversion tools
using namespace std;

namespace Imagine
{
// ----------------------------------------------------------------------
// Initialize shape data:
//
//	Destroy attributename pointees, clear the list
//	Destroy element pointees, clear the vector
//	Initialize bounding box
//	Initialize type
// ----------------------------------------------------------------------

void NFmiEsriShape::Init(void)
{
  try
  {
    // Destroy attributename pointees

    attributes_type::iterator aiter = itsAttributeNames.begin();
    for (; aiter != itsAttributeNames.end(); ++aiter)
      delete *aiter;

    // and clear the list

    itsAttributeNames.clear();

    // Destroy element pointees

    NFmiEsriShape::iterator eiter = itsElements.begin();
    for (; eiter != itsElements.end(); ++eiter)
      delete *eiter;

    // and clear the vector

    itsElements.clear();

    // Initialize bounding box

    itsBox.Init();

    // Initialize type

    itsShapeType = kFmiEsriNull;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Add an element to the shape
// ----------------------------------------------------------------------

void NFmiEsriShape::Add(NFmiEsriElement *theElement)
{
  try
  {
    itsElements.push_back(theElement);
    theElement->Update(itsBox);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Add an attribute name to the shape
// ----------------------------------------------------------------------

void NFmiEsriShape::Add(NFmiEsriAttributeName *theAttributeName)
{
  try
  {
    itsAttributeNames.push_back(theAttributeName);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Reading a shapefile and a database file. Returns TRUE if reading
// was succesful, false otherwise. Note that we read the entire
// shape file, hence we do not need the shx file, which is required
// only for speeding up random access queries.
// ----------------------------------------------------------------------

bool NFmiEsriShape::Read(const string &theFilename, bool fDBF)
{
  try
  {
    // Default search path for shape files
    const string shapes_path = NFmiSettings::Optional<string>("imagine::shapes_path", string("."));

    // Derived file names

    string shpfilename = NFmiFileSystem::FileComplete(theFilename + ".shp", shapes_path);
    string dbffilename = NFmiFileSystem::FileComplete(theFilename + ".dbf", shapes_path);

    // Try upper case suffix if necessary

    if (!NFmiFileSystem::FileExists(shpfilename))
      shpfilename = NFmiFileSystem::FileComplete(theFilename + ".SHP", shapes_path);
    if (!NFmiFileSystem::FileExists(dbffilename))
      dbffilename = NFmiFileSystem::FileComplete(theFilename + ".DBF", shapes_path);

    // Delete old contents if there are any

    if (!itsElements.empty())
      Init();

    // Open the shp file for reading

    ifstream shpfile(shpfilename.c_str(), ios::in | ios::binary);
    if (!shpfile)
      return false;

    // Read the 100-byte header

    string shpheader;
    if (!EsriRead(shpfile, shpheader, kFmiEsriHeaderSize))
    {
      shpfile.close();
      return false;
    }

    // Check the magic number

    int shpmagic = BigEndianInt(shpheader, kFmiEsriPosMagic);
    if (shpmagic != kFmiEsriMagicNumber)
    {
      shpfile.close();
      return false;
    }

    // Read header elements

    unsigned int shpsize = BigEndianInt(shpheader, kFmiEsriPosFileLength);
    // int shpversion = LittleEndianInt(shpheader,kFmiEsriPosVersion);
    int shptype = LittleEndianInt(shpheader, kFmiEsriPosType);

    // We ignore the bbox in the file, instead we recalculate it
    // while reading the elements. Same thing for the bounding
    // boxes of the elements themselves.
    //
    // double shpxmin = LittleEndianDouble(shpheader,kFmiEsriPosXmin);
    // double shpxmax = LittleEndianDouble(shpheader,kFmiEsriPosXmax);
    // double shpymin = LittleEndianDouble(shpheader,kFmiEsriPosYmin);
    // double shpymax = LittleEndianDouble(shpheader,kFmiEsriPosYmax);
    // double shpzmin = LittleEndianDouble(shpheader,kFmiEsriPosZmin);
    // double shpzmax = LittleEndianDouble(shpheader,kFmiEsriPosZmax);
    // double shpmmin = LittleEndianDouble(shpheader,kFmiEsriPosMmin);
    // double shpmmax = LittleEndianDouble(shpheader,kFmiEsriPosMmax);

    itsShapeType = static_cast<NFmiEsriElementType>(shptype);

    // Read all the records in the shape file into one big string

    int remainingsize = shpsize - kFmiEsriHeaderSize / 2;

    string shpdata;

    if (!EsriRead(shpfile, shpdata, remainingsize * 2))  // 16-bit units!!
    {
      shpfile.close();
      return false;
    }

    shpfile.close();

    // Count the number of elements in the buffer and reserve space
    // for that many elements. This doesn't save much though, since
    // the elements are pointers.

    itsElements.reserve(CountRecords(shpdata));

    // Then process all records in the file

    unsigned int pos = 0;
    while (pos < shpdata.size())
    {
      // Record header

      int recnum = BigEndianInt(shpdata, pos + kFmiEsriRecordHeaderPosNum);
      int reclen = BigEndianInt(shpdata, pos + kFmiEsriRecordHeaderPosLen);

      pos += kFmiEsriRecordHeaderSize;

      // Interpret the data element

      bool ok = Add(recnum, shpdata, pos);

      // Abort if data seems strange

      if (!ok)
      {
        return false;
      }

      // Safety check
      if (reclen < 0)
        break;

      pos += reclen * 2;  // Esri sizes are in 16-bit units!
    }

    // We do not parse .proj files. We assume WGS84, unless one of the following files exists:
    string fmifilename = NFmiFileSystem::FileComplete(theFilename + ".fmi", shapes_path);
    string ykjfilename = NFmiFileSystem::FileComplete(theFilename + ".ykj", shapes_path);

    std::string proj4 = "WGS84";
    if (NFmiFileSystem::FileExists(fmifilename))
      proj4 = fmt::format("+proj=longlat +R={:.0f} +over +no_defs +towgs84=0,0,0", kRearth);
    else if (NFmiFileSystem::FileExists(ykjfilename))
      proj4 =
          "+proj=tmerc +lat_0=0 +lon_0=27 +k=1 +x_0=3500000 +y_0=0 +ellps=intl +units=m +wktext "
          "+towgs84=-96.0617,-82.4278,-121.7535,4.80107,0.34543,-1.37646,1.4964 +no_defs";

    auto err = itsSpatialReference.SetFromUserInput(proj4.c_str());

    if (err != OGRERR_NONE)
      throw Fmi::Exception(BCP, "Failed to create spatial reference from '" + proj4 + "'");

    // We're done if the DBF file is not desired

    if (!fDBF)
      return true;

    //! If the file does not exist, or is of zero size, we're done

    if (NFmiFileSystem::FileEmpty(dbffilename))
      return true;

    // Otherwise open the file for reading

    ifstream dbffile(dbffilename.c_str(), ios::in | ios::binary);
    if (!dbffile)
      return false;  // failed to open!!

    // Read the header

    string dbfheader;
    if (!EsriRead(dbffile, dbfheader, kFmixBaseHeaderSize))
    {
      dbffile.close();
      return false;
    }

    // Check proper type

    if (dbfheader[0] != 3)
    {
      dbffile.close();
      return false;
    }

    // We're interested only in the following fields

    int numrecords = LittleEndianInt(dbfheader, kFmixBaseNumRecordsPos);
    int headerlength = LittleEndianShort(dbfheader, kFmixBaseHeaderLengthPos);
    int recordlength = LittleEndianShort(dbfheader, kFmixBaseRecordLengthPos);

    int numfields = (headerlength - kFmixBaseHeaderSize) / kFmixBaseFieldSize;

    if (numrecords > static_cast<int>(Elements().size()))
    {
      cerr << "Warning: DBF contains more records than SHP" << endl;
      numrecords = Elements().size();
    }

    // Read all the fields in

    string dbffields;
    if (!EsriRead(dbffile, dbffields, numfields * kFmixBaseFieldSize))
    {
      dbffile.close();
      return false;
    }

    // Loop over all the fields, adding them into the attribute names
    // list one by one

    // These are just utility vectors to aid reading the data part:

    vector<int> fieldoffsets;                  // offset to field
    vector<int> fieldsizes;                    // size of field in bytes
    vector<NFmiEsriAttributeType> fieldtypes;  // field type

    for (int num = 0; num < numfields; num++)
    {
      int fieldpos = num * kFmixBaseFieldSize;
      string fname = string(dbffields.c_str() + fieldpos + kFmixBaseFieldNamePos);
      char ftype = dbffields[fieldpos + kFmixBaseFieldTypePos];

      // See the docs in the beginning, slen and flen,dlen are mutually
      // exclusive fields, the one to be used depends in the field type

      int flen = dbffields[fieldpos + kFmixBaseFieldLengthPos];
      int dlen = dbffields[fieldpos + kFmixBaseFieldDecimalPos];

      int slen = LittleEndianShort(dbffields, fieldpos + kFmixBaseFieldLengthPos);

      if (ftype != 'N' && ftype != 'F' && ftype != 'C' && ftype != 'D')
        throw Fmi::Exception(BCP, string("Unrecognized shape value type '") + ftype + "'");

      if (ftype == 'N' || ftype == 'F')
      {
        if (dlen == 0)
        {
          Add(new NFmiEsriAttributeName(fname, kFmiEsriInteger, flen, dlen, -1));
          fieldtypes.push_back(kFmiEsriInteger);
        }
        else
        {
          Add(new NFmiEsriAttributeName(fname, kFmiEsriDouble, flen, dlen, -1));
          fieldtypes.push_back(kFmiEsriDouble);
        }
        fieldsizes.push_back(flen);
      }
      else if (ftype == 'C')
      {
        Add(new NFmiEsriAttributeName(fname, kFmiEsriString, -1, -1, slen));
        fieldsizes.push_back(slen);
        fieldtypes.push_back(kFmiEsriString);
      }
      else  // ftype=D for date in YYYYMMDD form
      {
        Add(new NFmiEsriAttributeName(fname, kFmiEsriDate, 8, 8, -1));
        fieldtypes.push_back(kFmiEsriDate);
        fieldsizes.push_back(8);
      }

      if (num == 0)
        fieldoffsets.push_back(1);  // size of leading delete flag is 1
      else
        fieldoffsets.push_back(fieldoffsets[num - 1] + fieldsizes[num - 1]);
    }

    // Read in the attributes themselves. By ESRI shapefile definition the
    // record order is the exact same in the shp file and the dbf file, hence
    // we need not worry about matching records.

    // The first shape to get an attribute

    NFmiEsriShape::iterator elementiter = itsElements.begin();

    // Read all the records at once, including the leading terminator byte

    string dbfrecord;
    if (!EsriRead(dbffile, dbfrecord, 1 + numrecords * recordlength))
    {
      dbffile.close();
      return false;
    }

    // And loop over it, extracting the records and fields

    for (int rec = 1; rec <= numrecords; rec++, ++elementiter)
    {
      // Offset to the record (including the leading 1 byte terminator)

      int recpos = 1 + (rec - 1) * recordlength;

      attributes_type::iterator nameiter = itsAttributeNames.begin();
      for (int fieldnum = 0; fieldnum < numfields; ++fieldnum, ++nameiter)
      {
        // Position and size of the field

        int offset = recpos + fieldoffsets[fieldnum];
        int size = fieldsizes[fieldnum];

        // Extract the field data

        switch (fieldtypes[fieldnum])
        {
          case kFmiEsriString:
          {
            string value = dbfrecord.substr(offset, size);
            NFmiStringTools::TrimL(value);
            NFmiStringTools::TrimR(value);
            (*elementiter)->Add(NFmiEsriAttribute(value, *nameiter));
            break;
          }
          case kFmiEsriInteger:
          {
            int value = atoi(dbfrecord.substr(offset, size).c_str());
            (*elementiter)->Add(NFmiEsriAttribute(value, *nameiter));
            break;
          }
          case kFmiEsriDouble:
          {
            double value = atof(dbfrecord.substr(offset, size).c_str());
            (*elementiter)->Add(NFmiEsriAttribute(value, *nameiter));
            break;
          }
          case kFmiEsriDate:
          {
            long value = atol(dbfrecord.substr(offset, size).c_str());
            int yyyy = value / 10000;
            int mm = (value / 100) % 100;
            int dd = value % 100;
            NFmiMetTime t(yyyy, mm, dd);
            (*elementiter)->Add(NFmiEsriAttribute(t, *nameiter));
            break;
          }
        }
      }
    }

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Add a new element in stringed form (read as string from a file)
// ----------------------------------------------------------------------

bool NFmiEsriShape::Add(int theRecordNumber, const string &theBuffer, int thePos)
{
  try
  {
    // Establish the type

    int shptype = LittleEndianInt(theBuffer, thePos);

    switch (shptype)
    {
      case kFmiEsriNull:
        Add(new NFmiEsriNull());
        break;

      case kFmiEsriPoint:
        Add(new NFmiEsriPoint(theBuffer, thePos, theRecordNumber));
        break;
      case kFmiEsriMultiPoint:
        Add(new NFmiEsriMultiPoint(theBuffer, thePos, theRecordNumber));
        break;
      case kFmiEsriPolyLine:
        Add(new NFmiEsriPolyLine(theBuffer, thePos, theRecordNumber));
        break;
      case kFmiEsriPolygon:
        Add(new NFmiEsriPolygon(theBuffer, thePos, theRecordNumber));
        break;

      case kFmiEsriPointM:
        Add(new NFmiEsriPointM(theBuffer, thePos, theRecordNumber));
        break;
      case kFmiEsriMultiPointM:
        Add(new NFmiEsriMultiPointM(theBuffer, thePos, theRecordNumber));
        break;
      case kFmiEsriPolyLineM:
        Add(new NFmiEsriPolyLineM(theBuffer, thePos, theRecordNumber));
        break;
      case kFmiEsriPolygonM:
        Add(new NFmiEsriPolygonM(theBuffer, thePos, theRecordNumber));
        break;

      case kFmiEsriPointZ:
        Add(new NFmiEsriPointZ(theBuffer, thePos, theRecordNumber));
        break;
      case kFmiEsriMultiPointZ:
        Add(new NFmiEsriMultiPointZ(theBuffer, thePos, theRecordNumber));
        break;
      case kFmiEsriPolyLineZ:
        Add(new NFmiEsriPolyLineZ(theBuffer, thePos, theRecordNumber));
        break;
      case kFmiEsriPolygonZ:
        Add(new NFmiEsriPolygonZ(theBuffer, thePos, theRecordNumber));
        break;

        // Return false for unknown types

      default:
        return false;
    }

    return true;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Count the number of records in a string buffer of elements
// ----------------------------------------------------------------------

int NFmiEsriShape::CountRecords(const string &theBuffer) const
{
  try
  {
    int num = 0;
    unsigned int pos = 0;
    while (pos < theBuffer.size())
    {
      ++num;
      int reclen = BigEndianInt(theBuffer, pos + kFmiEsriRecordHeaderPosLen);
      pos += kFmiEsriRecordHeaderSize + reclen * 2;
    }
    return num;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Writing a shapefile, shapefile index and a database file.
// Returns false if any fails, true for success.
// ----------------------------------------------------------------------

bool NFmiEsriShape::Write(const string &theFilename, bool fDBF, bool fSHX) const
{
  try
  {
    // Derived file names

    const string shpfilename = theFilename + ".shp";
    const string dbffilename = theFilename + ".dbf";
    const string shxfilename = theFilename + ".shx";

    bool ok = true;

    // Write the shape file

    ok |= WriteSHP(shpfilename);

    // Write the index file

    if (fSHX)
      ok |= WriteSHX(shxfilename);

    // Write the attribute file

    if (fDBF)
      ok |= WriteDBF(dbffilename);

    return ok;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Writing a shapefile
// Returns false if any fails, true for success.
// ----------------------------------------------------------------------

bool NFmiEsriShape::WriteSHP(const string &theFilename) const
{
  try
  {
    // Calculate the size the body will require

    int bodysize = 0;

    NFmiEsriShape::const_iterator iter;

    for (iter = Elements().begin(); iter != Elements().end(); ++iter)
      bodysize += kFmiEsriRecordHeaderSize + (*iter)->StringSize();

    // The full file size in 16-bit units is then

    int filesize = (bodysize + kFmiEsriHeaderSize) / 2;

    // Start writing the shp file

    ofstream shpfile(theFilename.c_str(), ios::out | ios::binary);
    if (!shpfile)
      return false;

    // Write the header

    WriteHeader(shpfile, filesize);

    // Then write the body, one record at a time
    //
    // Note: ArcView seems to start from record number 1, we use it too so
    //       that we can do simple read/write comparisons to check proper
    //       implementation.
    //
    // Note: We allow null pointers in the list, we interpret it to
    //       mean the respective element has been deleted. This may
    //       prove to be useful when speed is required.

    int recnum = 1;

    for (iter = Elements().begin(); iter != Elements().end(); ++iter)
    {
      if (*iter != nullptr)
      {
        shpfile << BigEndianInt(recnum) << BigEndianInt(((*iter)->StringSize()) / 2) << **iter;
        ++recnum;
      }
    }

    // Done

    shpfile.close();

    return (!shpfile.fail());
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Writing a shapefile index
// Returns false if any fails, true for success.
// ----------------------------------------------------------------------

bool NFmiEsriShape::WriteSHX(const string &theFilename) const
{
  try
  {
    // Open output file

    ofstream shxfile(theFilename.c_str(), ios::out | ios::binary);
    if (!shxfile)
      return false;

    // Write the header

    int shxlength = (kFmiEsriHeaderSize + 8 * Elements().size()) / 2;

    WriteHeader(shxfile, shxlength);

    // The body consists of offsets to each record in the main file,
    // plus the length of the record as in the record header of the
    // main file

    // The first offset is always this, the position right after
    // the header (in 16-bit units)

    int curoffset = kFmiEsriHeaderSize / 2;

    NFmiEsriShape::const_iterator iter;

    for (iter = Elements().begin(); iter != Elements().end(); ++iter)
    {
      if (*iter != nullptr)
      {
        int recsize = ((*iter)->StringSize()) / 2;
        shxfile << BigEndianInt(curoffset) << BigEndianInt(recsize);
        curoffset += recsize + kFmiEsriRecordHeaderSize / 2;
      }
    }

    shxfile.close();

    return (!shxfile.fail());
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Writing a dBASE file
// Returns false if any fails, true for success.
// ----------------------------------------------------------------------

bool NFmiEsriShape::WriteDBF(const string &theFilename) const
{
  try
  {
    const int header_size = 32;
    const int field_size = 32;
    const int header_zeros = 20;
    const int field_zeros = 10;
    const int name_size = 11;

    const int element_count = static_cast<int>(itsElements.size());

    // Open output file

    ofstream dbffile(theFilename.c_str(), ios::out | ios::binary);
    if (!dbffile)
      return false;

    // Handle the special case of no attributes by writing
    // a dummy database.

    NFmiTime date;

    dbffile << '\x03'  // plain dBaseIII+ file without extras
            << static_cast<unsigned char>(date.GetYear() - 1900)
            << static_cast<unsigned char>(date.GetMonth())
            << static_cast<unsigned char>(date.GetDay()) << LittleEndianInt(element_count);

    if (itsAttributeNames.empty())
    {
      int i;
      const int field_length = 7;
      dbffile << LittleEndianShort(header_size + 1 * field_size + 1)
              << LittleEndianShort(field_length);
      for (i = 0; i < header_zeros; i++)
        dbffile << '\0';

      dbffile << "INDEX";
      for (i = 0; i < name_size - 5; i++)
        dbffile << '\0';
      dbffile << 'N' << LittleEndianInt(0) << '\x06' << '\x00';
      dbffile << LittleEndianInt(0);
      for (i = 0; i < field_zeros; i++)
        dbffile << '\0';
      dbffile << '\x0d';  // header record terminator

      // Note: We assume the field width is wide enough
      // so that the first character is always a space
      // character indicating a "not-deleted" field
      for (i = 1; i <= element_count; i++)
        dbffile << setw(field_length) << setfill(' ') << i;
    }
    else
    {
      const int field_count = itsAttributeNames.size();

      int field_length = 1;  // 1 for the delete marker
      int f = 0;
      for (f = 0; f < field_count; f++)
      {
        const NFmiEsriAttributeName *attribute = itsAttributeNames[f];
        switch (attribute->Type())
        {
          case kFmiEsriString:
            field_length += attribute->Length();
            break;
          case kFmiEsriInteger:
          case kFmiEsriDouble:
          case kFmiEsriDate:
            field_length += attribute->FieldLength();
            break;
        }
      }

      int i;
      dbffile << LittleEndianShort(header_size + field_count * field_size + 1)
              << LittleEndianShort(field_length);
      for (i = 0; i < header_zeros; i++)
        dbffile << '\0';

      for (f = 0; f < field_count; f++)
      {
        const NFmiEsriAttributeName *attribute = itsAttributeNames[f];
        string name = attribute->Name();
        dbffile << name;
        for (i = 0; i < name_size - static_cast<int>(name.size()); i++)
          dbffile << '\0';

        switch (attribute->Type())
        {
          case kFmiEsriString:
            dbffile << kFmixBaseFieldChar << LittleEndianInt(0)
                    << LittleEndianShort(attribute->Length());
            break;
          case kFmiEsriInteger:
            dbffile << kFmixBaseFieldNumber << LittleEndianInt(0)
                    << static_cast<unsigned char>(attribute->FieldLength())
                    << static_cast<unsigned char>(attribute->DecimalCount());
            break;
          case kFmiEsriDouble:
            dbffile << kFmixBaseFieldFloat << LittleEndianInt(0)
                    << static_cast<unsigned char>(attribute->FieldLength())
                    << static_cast<unsigned char>(attribute->DecimalCount());
            break;
          case kFmiEsriDate:
            throw Fmi::Exception(BCP, "Writing dates not supported yet");
        }
        dbffile << LittleEndianInt(0);
        for (i = 0; i < field_zeros; i++)
          dbffile << '\0';
      }
      dbffile << '\x0d';  // header record terminator

      for (const_iterator it = itsElements.begin(); it != itsElements.end(); ++it)
      {
        const elements_type::value_type element = *it;

        dbffile << ' ';  // not deleted, "*" would indicate deleted

        for (attributes_type::const_iterator jt = itsAttributeNames.begin();
             jt != itsAttributeNames.end();
             ++jt)
        {
          const string name = (*jt)->Name();
          switch ((*jt)->Type())
          {
            case kFmiEsriString:
            {
              const string value = element->GetString(name);
              const int length = (*jt)->Length();
              dbffile << value;
              for (i = 0; i < length - static_cast<int>(value.size()); i++)
                dbffile << ' ';
              break;
            }
            case kFmiEsriInteger:
            {
              const int value = element->GetInteger(name);
              const int length = (*jt)->FieldLength();
              dbffile << setw(length) << setfill(' ') << value;
              break;
            }
            case kFmiEsriDouble:
            {
              const double value = element->GetDouble(name);
              const int length = (*jt)->FieldLength();
              const int decimals = (*jt)->DecimalCount();
#ifdef PERKELEEN_296
              // Perkeleen g++ v2.96, std::fixed puuttuu
              NFmiString str = NFmiValueString::GetStringWithMaxDecimalsSmartWay(value, decimals);
              dbffile << setw(length) << setfill(' ') << str.CharPtr();
#else
              dbffile << setw(length) << setfill(' ') << fixed << setprecision(decimals) << value;
#endif
              break;
            }
            case kFmiEsriDate:
              throw Fmi::Exception(BCP, "Writing dates not supported yet");
          }
        }
      }
    }

    dbffile << '\x1a';

    dbffile.close();

    return (!dbffile.fail());
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Header writing utility for SHP and SHX files
// ----------------------------------------------------------------------

void NFmiEsriShape::WriteHeader(ostream &os, int theFileLength) const
{
  try
  {
    // Output the header

    os << BigEndianInt(kFmiEsriMagicNumber)  // 0:magic number
       << BigEndianInt(0)                    // 4:unused
       << BigEndianInt(0)                    // 8:unused
       << BigEndianInt(0)                    // 12:unused
       << BigEndianInt(0)                    // 16:unused
       << BigEndianInt(0)                    // 20:unused
       << BigEndianInt(theFileLength)        // 24:file length
       << LittleEndianInt(kFmiEsriVersion)   // 28:version
       << LittleEndianInt(Type());           // 32:type

    // Technical specs: Missing value must be written as zero
    // The order of limits is xmin,ymin,xmax,ymax,zmin,zmax,mmin,mmax

    os << LittleEndianDouble(Box().IsValid() ? Box().Xmin() : 0.0)
       << LittleEndianDouble(Box().IsValid() ? Box().Ymin() : 0.0)
       << LittleEndianDouble(Box().IsValid() ? Box().Xmax() : 0.0)
       << LittleEndianDouble(Box().IsValid() ? Box().Ymax() : 0.0)
       << LittleEndianDouble(Box().IsValid() ? Box().Zmin() : 0.0)
       << LittleEndianDouble(Box().IsValid() ? Box().Zmax() : 0.0)
       << LittleEndianDouble(Box().IsValid() ? Box().Mmin() : 0.0)
       << LittleEndianDouble(Box().IsValid() ? Box().Mmax() : 0.0);
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Data element projector
// ----------------------------------------------------------------------

void NFmiEsriShape::Project(const NFmiEsriProjector &theProjector)
{
  try
  {
    // Invalidate the bounding box X-Y limits first
    // Note that we do not initialize the M or Z parts

    static_cast<NFmiEsriBox>(itsBox).Init();

    NFmiEsriShape::iterator iter = itsElements.begin();

    for (; iter != itsElements.end(); ++iter)
    {
      if (*iter != nullptr)
      {
        (*iter)->Project(theProjector);
        (*iter)->Update(itsBox);
      }
    }
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

// ----------------------------------------------------------------------
// Return desired attributename based on the field name
// ----------------------------------------------------------------------

NFmiEsriAttributeName *NFmiEsriShape::AttributeName(const string &theFieldName) const
{
  try
  {
    attributes_type::const_iterator begin = itsAttributeNames.begin();
    attributes_type::const_iterator end = itsAttributeNames.end();
    attributes_type::const_iterator iter;

    NFmiEsriAttributeName *out = nullptr;

    for (iter = begin; iter != end; ++iter)
    {
      // Just safety, should never happen

      if (*iter == nullptr)
        continue;

      // If found match, exit immediately with pointer to the name

      if ((*iter)->Name() == theFieldName)
      {
        out = *iter;
        break;
      }
    }

    return out;
  }
  catch (...)
  {
    throw Fmi::Exception::Trace(BCP, "Operation failed!");
  }
}

}  // namespace Imagine

// ======================================================================
