// ======================================================================
//
// Generic map vector data (point,polyline,polygon) container and
// renderer.
//
// History:
//
// 25.09.2001 Mika Heiskanen
//
//	Implemented ESRI mapdata renderer based on libesri
//
// ======================================================================
#ifdef WIN32
#pragma warning(disable : 4786) // poistaa n kpl VC++ kääntäjän varoitusta
#endif

#include "NFmiGeoShape.h"
#include "NFmiEsriMultiPointZ.h"
#include "NFmiEsriPolyLineZ.h"
#include "NFmiEsriPolygonZ.h"
#include "NFmiEsriMultiPatch.h"

using namespace std;
// NFmiEsriPoint projector function object

class ProjectXYEsriPoint : public NFmiEsriProjector
{
public:
  ProjectXYEsriPoint(const NFmiArea *theArea) : itsArea(theArea) {}
  
  NFmiEsriPoint operator() (const NFmiEsriPoint & thePoint) const
  {
    NFmiPoint tmp(thePoint.X(),thePoint.Y());
    tmp = itsArea->ToXY(tmp);
    return NFmiEsriPoint(tmp.X(),tmp.Y());
  }
  
private:
  const NFmiArea	*itsArea;
};

// ----------------------------------------------------------------------
// Project the data to XY-coordinates
// ----------------------------------------------------------------------

void NFmiGeoShape::ProjectXY(const NFmiArea & theArea)
{
  switch(Type())
    {
	case kFmiGeoShapeEsri:
	  if(itsEsriShape!=NULL)
		itsEsriShape->Project(ProjectXYEsriPoint(&theArea));
	  break;
	case kFmiGeoShapeShoreLine:
	  cerr << "Error: NFmiGeoShape::Project() kFmiGeoShapeShoreLine not implemented" << endl;
	  exit(1);
	  break;
	case kFmiGeoShapeGMT:
	  cerr << "Error: NFmiGeoShape::Project() kFmiGeoShapeGMT not implemented" << endl;
	  exit(1);
	  break;
      
    }
}

// ----------------------------------------------------------------------
// Build a path from the map data. If the operation is not possible,
// an empty path is returned.
// ----------------------------------------------------------------------

NFmiPath NFmiGeoShape::Path(void) const
{
  NFmiPath out;
  switch(Type())
    {
	case kFmiGeoShapeEsri:
	  out = PathEsri();
	  break;
	case kFmiGeoShapeShoreLine:
	  cerr << "Error: NFmiGeoShape::Path() kFmiGeoShapeShoreLine not implemented" << endl;
	  exit(1);
	case kFmiGeoShapeGMT:
	  cerr << "Error: NFmiGeoShape::Path() kFmiGeoShapeGMT not implemented" << endl;
	  exit(1);
    }
  return out;
}

// ----------------------------------------------------------------------
// Add map data to a NFmiFillMap
// ----------------------------------------------------------------------

void NFmiGeoShape::Add(NFmiFillMap & theMap) const
{
  switch(Type())
    {
	case kFmiGeoShapeEsri:
	  //	return AddEsri(theMap); // MSVC kääntäjä ei hyväksynyt return void tapausta.
	  AddEsri(theMap);
	  return ;
	  break;
	case kFmiGeoShapeShoreLine:
	  cerr << "Error: NFmiGeoShape::Add() kFmiGeoShapeShoreLine not implemented" << endl;
	  exit(1);
	  break;
	case kFmiGeoShapeGMT:
	  cerr << "Error: NFmiGeoShape::Add() kFmiGeoShapeGMT not implemented" << endl;
	  exit(1);
	  break;
      
    }
}

// ----------------------------------------------------------------------
// Stroke map data
// ----------------------------------------------------------------------

void NFmiGeoShape::Stroke(NFmiImage & theImage,
						  NFmiColorTools::Color theColor,
						  NFmiColorTools::NFmiBlendRule theRule) const
{
  // Quick exit if color is not real
  
  if(theColor==NFmiColorTools::NoColor)
    return;
  
  switch(Type())
    {
	case kFmiGeoShapeEsri:
	  //	return StrokeEsri(theImage,theColor,theRule);
	  StrokeEsri(theImage,theColor,theRule);
	  return ;
	  break;
	case kFmiGeoShapeShoreLine:
	  cerr << "Error: NFmiGeoShape::Stroke() kFmiGeoShapeShoreLine not implemented" << endl;
	  exit(1);
	  break;
	case kFmiGeoShapeGMT:
	  cerr << "Error: NFmiGeoShape::Stroke() kFmiGeoShapeGMT not implemented" << endl;
	  exit(1);
	  break;
      
    }
}

// ----------------------------------------------------------------------
// Mark map data
// ----------------------------------------------------------------------

void NFmiGeoShape::Mark(NFmiImage & theImage,
						const NFmiImage & theMarker,
						NFmiColorTools::NFmiBlendRule theRule,
						NFmiAlignment theAlignment,
						float theAlpha) const
{
  switch(Type())
    {
	case kFmiGeoShapeEsri:
	  //	return MarkEsri(theImage,theMarker,theRule,theAlignment,theAlpha);
	  MarkEsri(theImage,theMarker,theRule,theAlignment,theAlpha);
	  return ;
	  break;
	case kFmiGeoShapeShoreLine:
	  cerr << "Error: NFmiGeoShape::Mark() kFmiGeoShapeShoreLine not implemented" << endl;
	  exit(1);
	  break;
	case kFmiGeoShapeGMT:
	  cerr << "Error: NFmiGeoShape::Mark() kFmiGeoShapeGMT not implemented" << endl;
	  exit(1);
	  break;
      
    }
}

// ----------------------------------------------------------------------
// Generate imagemap data to a file
// ----------------------------------------------------------------------

void NFmiGeoShape::WriteImageMap(std::ostream & os,
								 const string & theFieldName) const
{
  switch(Type())
    {
	case kFmiGeoShapeEsri:
	  //	return WriteImageMapEsri(os,theFieldName);
	  WriteImageMapEsri(os,theFieldName);
	  return ;
	  break;
	case kFmiGeoShapeShoreLine:
	  cerr << "Error: NFmiGeoShape::WriteImageMap() kFmiGeoShapeShoreLine not implemented" << endl;
	  exit(1);
	  break;
	case kFmiGeoShapeGMT:
	  cerr << "Error: NFmiGeoShape::WriteImageMap() kFmiGeoShapeGMT not implemented" << endl;
	  exit(1);
	  break;
      
    }
}

// ----------------------------------------------------------------------
// Build a path from ESRI shape data. Depending on the data element type,
// the returned data is:
//
// Null		[]
// Point	[moveto]
// PolyLine	[moveto [lineto]*]*
// Polygon	[moveto [lineto]*]*
// MultiPoint	[moveto]*
// MultiPatch	[moveto [lineto]*]*
//
// The same output is given for M and Z data types
//
// ----------------------------------------------------------------------

NFmiPath NFmiGeoShape::PathEsri(void) const
{
  // The result is a single path containing all the moves
  
  NFmiPath outpath;
  
  // Just a safety, should not happen
  
  if(itsEsriShape==NULL)
    return outpath;
  
  // Iterate through all elements
  
  NFmiEsriShape::const_iterator iter = itsEsriShape->Elements().begin();
  
  for( ; iter != itsEsriShape->Elements().end(); ++iter)
    {
      // There may be deleted elements in the shape, which are to be ignored
	  
      if(*iter==NULL)
		continue;
	  
      switch((*iter)->Type())
		{
		case kFmiEsriNull:
		  break;
		  
		case kFmiEsriPoint:
		case kFmiEsriPointM:
		case kFmiEsriPointZ:
		  {
			outpath.LineTo((*iter)->X(),(*iter)->Y());
			break;
		  }
		  
		case kFmiEsriMultiPoint:
		case kFmiEsriMultiPointM:
		case kFmiEsriMultiPointZ:
		  {
			const NFmiEsriMultiPoint * elem = static_cast<const NFmiEsriMultiPoint *>(*iter);
			for(int i=0; i<elem->NumPoints(); i++)
			  outpath.LineTo(elem->Points()[i].X(), elem->Points()[i].Y());
			break;
		  }
		  
		case kFmiEsriPolyLine:
		case kFmiEsriPolyLineM:
		case kFmiEsriPolyLineZ:
		  {
			const NFmiEsriPolyLine * elem = static_cast<const NFmiEsriPolyLine *>(*iter);
			for(int part=0; part<elem->NumParts(); part++)
			  {
				int i1,i2;
				i1 = elem->Parts()[part];		// start of part
				if(part+1 == elem->NumParts())
				  i2 = elem->NumPoints()-1;		// end of part
				else
				  i2 = elem->Parts()[part+1]-1;		// end of part
				
				if(i2>=i1)
				  {
					outpath.MoveTo(elem->Points()[i1].X(),
								   elem->Points()[i1].Y());
					for(int i=i1+1; i<=i2; i++)
					  outpath.LineTo(elem->Points()[i].X(),
									 elem->Points()[i].Y());
				  }
			  }
			break;
		  }
		case kFmiEsriPolygon:
		case kFmiEsriPolygonM:
		case kFmiEsriPolygonZ:
		  {
			const NFmiEsriPolygon * elem = static_cast<const NFmiEsriPolygon *>(*iter);
			for(int part=0; part<elem->NumParts(); part++)
			  {
				int i1,i2;
				i1 = elem->Parts()[part];		// start of part
				if(part+1 == elem->NumParts())
				  i2 = elem->NumPoints()-1;		// end of part
				else
				  i2 = elem->Parts()[part+1]-1;	// end of part
				
				if(i2>=i1)
				  {
					outpath.MoveTo(elem->Points()[i1].X(),
								   elem->Points()[i1].Y());
					for(int i=i1+1; i<=i2; i++)
					  outpath.LineTo(elem->Points()[i].X(),
									 elem->Points()[i].Y());
				  }
			  }
			break;
		  }
		case kFmiEsriMultiPatch:
		  {
			const NFmiEsriMultiPatch * elem = static_cast<const NFmiEsriMultiPatch *>(*iter);
			for(int part=0; part<elem->NumParts(); part++)
			  {
				int i1,i2;
				i1 = elem->Parts()[part];		// start of part
				if(part+1 == elem->NumParts())
				  i2 = elem->NumPoints()-1;		// end of part
				else
				  i2 = elem->Parts()[part+1]-1;	// end of part
				
				if(i2>=i1)
				  {
					switch(elem->PartTypes()[part])
					  {
					  case kFmiEsriTriangleStrip:
						{
						  for(int i=i1+2; i<=i2; i+=3)
							{
							  outpath.MoveTo(elem->Points()[i-2].X(),
											 elem->Points()[i-2].Y());
							  outpath.LineTo(elem->Points()[i-1].X(),
											 elem->Points()[i-1].Y());
							  outpath.LineTo(elem->Points()[i-0].X(),
											 elem->Points()[i-0].Y());
							  outpath.LineTo(elem->Points()[i-2].X(),
											 elem->Points()[i-2].Y());
							}
						  break;
						}
						break;
					  case kFmiEsriTriangleFan:
						{
						  float x0 = elem->Points()[i1].X();
						  float y0 = elem->Points()[i1].Y();
						  for(int i=i1+2; i<=i2; i+=2)
							{
							  outpath.MoveTo(x0,y0);
							  outpath.LineTo(elem->Points()[i-1].X(),
											 elem->Points()[i-1].Y());
							  outpath.LineTo(elem->Points()[i-0].X(),
											 elem->Points()[i-0].Y());
							  outpath.LineTo(x0,y0);
							}
						  break;
						}
					  case kFmiEsriOuterRing:
					  case kFmiEsriInnerRing:
					  case kFmiEsriFirstRing:
					  case kFmiEsriRing:
						{
						  float x0 = elem->Points()[i1].X();
						  float y0 = elem->Points()[i1].Y();
						  outpath.MoveTo(x0,y0);
						  for(int i=i1+1; i<=i2; i++)
							outpath.LineTo(elem->Points()[i].X(),
										   elem->Points()[i].Y());
						  break;
						}
					  }
				  }
			  }
			break;
		  }
		}
    }
  return outpath;
}

// ----------------------------------------------------------------------
// Add ESRI shape data to a fill map.
// Depending on the data element type, the data added is:
//
// Null		[]
// Point	[]
// PolyLine	[]
// Polygon	[moveto [lineto]*]*
// MultiPoint	[]
// MultiPatch	[moveto [lineto]*]*
//
// The same output is given for M and Z data types
//
// ----------------------------------------------------------------------

void NFmiGeoShape::AddEsri(NFmiFillMap & theMap) const
{
  // Just a safety, should not happen
  
  if(itsEsriShape==NULL)
    return;
  
  // Iterate through all elements
  
  NFmiEsriShape::const_iterator iter = itsEsriShape->Elements().begin();
  
  for( ; iter != itsEsriShape->Elements().end(); ++iter)
    {
      // There may be deleted elements in the shape, which are to be ignored
	  
      if(*iter==NULL)
		continue;
	  
      switch((*iter)->Type())
		{
		case kFmiEsriNull:
		case kFmiEsriPoint:
		case kFmiEsriPointM:
		case kFmiEsriPointZ:
		case kFmiEsriMultiPoint:
		case kFmiEsriMultiPointM:
		case kFmiEsriMultiPointZ:
		case kFmiEsriPolyLine:
		case kFmiEsriPolyLineM:
		case kFmiEsriPolyLineZ:
		  break;
		  
		case kFmiEsriPolygon:
		case kFmiEsriPolygonM:
		case kFmiEsriPolygonZ:
		  {
			const NFmiEsriPolygon * elem = static_cast<const NFmiEsriPolygon *>(*iter);
			for(int part=0; part<elem->NumParts(); part++)
			  {
				int i1,i2;
				i1 = elem->Parts()[part];		// start of part
				if(part+1 == elem->NumParts())
				  i2 = elem->NumPoints()-1;		// end of part
				else
				  i2 = elem->Parts()[part+1]-1;	// end of part
				
				for(int i=i1+1; i<=i2; i++)
				  theMap.Add(elem->Points()[i-1].X(),
							 elem->Points()[i-1].Y(),
							 elem->Points()[i].X(),
							 elem->Points()[i].Y());
			  }
			break;
		  }
		case kFmiEsriMultiPatch:
		  {
			const NFmiEsriMultiPatch * elem = static_cast<const NFmiEsriMultiPatch *>(*iter);
			for(int part=0; part<elem->NumParts(); part++)
			  {
				int i1,i2;
				i1 = elem->Parts()[part];		// start of part
				if(part+1 == elem->NumParts())
				  i2 = elem->NumPoints()-1;		// end of part
				else
				  i2 = elem->Parts()[part+1]-1;	// end of part
				
				switch(elem->PartTypes()[part])
				  {
				  case kFmiEsriTriangleStrip:
					{
					  for(int i=i1+2; i<=i2; i+=3)
						{
						  theMap.Add(elem->Points()[i-2].X(),
									 elem->Points()[i-2].Y(),
									 elem->Points()[i-1].X(),
									 elem->Points()[i-1].Y());
						  theMap.Add(elem->Points()[i-1].X(),
									 elem->Points()[i-1].Y(),
									 elem->Points()[i-0].X(),
									 elem->Points()[i-0].Y());
						  theMap.Add(elem->Points()[i-2].X(),
									 elem->Points()[i-2].Y(),
									 elem->Points()[i-0].X(),
									 elem->Points()[i-0].Y());
						}
					  break;
					}
					break;
				  case kFmiEsriTriangleFan:
					{
					  float x0 = elem->Points()[i1].X();
					  float y0 = elem->Points()[i1].Y();
					  for(int i=i1+2; i<=i2; i+=2)
						{
						  theMap.Add(x0,y0,
									 elem->Points()[i-1].X(),
									 elem->Points()[i-1].Y());
						  theMap.Add(x0,y0,
									 elem->Points()[i].X(),
									 elem->Points()[i].Y());
						  theMap.Add(elem->Points()[i-1].X(),
									 elem->Points()[i-1].Y(),
									 elem->Points()[i].X(),
									 elem->Points()[i].Y());
						}
					  break;
					}
				  case kFmiEsriOuterRing:
				  case kFmiEsriInnerRing:
				  case kFmiEsriFirstRing:
				  case kFmiEsriRing:
					{
					  for(int i=i1+1; i<=i2; i++)
						theMap.Add(elem->Points()[i-1].X(),
								   elem->Points()[i-1].Y(),
								   elem->Points()[i].X(),
								   elem->Points()[i].Y());
					  break;
					}
				  }
			  }
			break;
		  }
		}
    }
}

// ----------------------------------------------------------------------
// Stroke ESRI shape data onto given image
//
// Null		[]
// Point	[moveto]
// PolyLine	[moveto [lineto]*]*
// Polygon	[moveto [lineto]*]*
// MultiPoint	[moveto]*
// MultiPatch	[moveto [lineto]*]*
//
// ----------------------------------------------------------------------

void NFmiGeoShape::StrokeEsri(NFmiImage & theImage,
							  NFmiColorTools::Color theColor,
							  NFmiColorTools::NFmiBlendRule theRule) const
{
  // We don't want to handle polyline stroking in several places,
  // it's easier just to create a path and let path stroker handle
  // all the tricky special cases. Not optimal, but it'll work.
  // Note that filling has no such special cases, hence we did optimize
  // Add(NFmiFillMap) above.
  
  NFmiPath path = Path();
  path.Stroke(theImage,theColor,theRule);
}

// ----------------------------------------------------------------------
// Mark ESRI shapefile map data
// Note that we consider multipatches invalid in this context
// ----------------------------------------------------------------------

void NFmiGeoShape::MarkEsri(NFmiImage & theImage,
							const NFmiImage & theMarker,
							NFmiColorTools::NFmiBlendRule theRule,
							NFmiAlignment theAlignment,
							float theAlpha) const
{
  // Just a safety, should not happen
  
  if(itsEsriShape==NULL)
    return;
  
  // Iterate through all elements
  
  NFmiEsriShape::const_iterator iter = itsEsriShape->Elements().begin();
  
  for( ; iter != itsEsriShape->Elements().end(); ++iter)
    {
      // There may be deleted elements in the shape, which are to be ignored
	  
      if(*iter==NULL)
		continue;
	  
      switch((*iter)->Type())
		{
		case kFmiEsriNull:
		case kFmiEsriMultiPatch:
		  break;
		  
		case kFmiEsriPoint:
		case kFmiEsriPointM:
		case kFmiEsriPointZ:
		  {
			int x = static_cast<int>((*iter)->X());
			int y = static_cast<int>((*iter)->Y());
			theImage.Composite(theMarker,theRule,theAlignment,x,y,theAlpha);
			break;
		  }
		  
		case kFmiEsriMultiPoint:
		case kFmiEsriMultiPointM:
		case kFmiEsriMultiPointZ:
		  {
			const NFmiEsriMultiPoint * elem = static_cast<const NFmiEsriMultiPoint *>(*iter);
			for(int i=0; i<elem->NumPoints(); i++)
			  {
				int x = static_cast<int>(elem->Points()[i].X());
				int y = static_cast<int>(elem->Points()[i].Y());
				theImage.Composite(theMarker,theRule,theAlignment,x,y,theAlpha);
			  }
			break;
		  }
		  
		case kFmiEsriPolyLine:
		case kFmiEsriPolyLineM:
		case kFmiEsriPolyLineZ:
		  {
			const NFmiEsriPolyLine * elem = static_cast<const NFmiEsriPolyLine *>(*iter);
			for(int i=0; i<elem->NumPoints(); i++)
			  {
				int x = static_cast<int>(elem->Points()[i].X());
				int y = static_cast<int>(elem->Points()[i].Y());
				theImage.Composite(theMarker,theRule,theAlignment,x,y,theAlpha);
			  }
			break;
		  }
		case kFmiEsriPolygon:
		case kFmiEsriPolygonM:
		case kFmiEsriPolygonZ:
		  {
			const NFmiEsriPolygon * elem = static_cast<const NFmiEsriPolygon *>(*iter);
			// Note that the last one is ignored, it is equal to the first one!
			for(int i=0; i<elem->NumPoints()-1; i++)
			  {
				int x = static_cast<int>(elem->Points()[i].X());
				int y = static_cast<int>(elem->Points()[i].Y());
				theImage.Composite(theMarker,theRule,theAlignment,x,y,theAlpha);
			  }
			break;
		  }
		}
    }
}

// ----------------------------------------------------------------------
// Generate imagemap data to a file
//
// Only the following elements are valid in this context:
//
//  points       - a small circle is generated
//  multipoints  - small circles are generated
//  polygons     - possibly multiple closed paths are generated
//
// ----------------------------------------------------------------------

void NFmiGeoShape::WriteImageMapEsri(std::ostream & os,
									 const string & theFieldName) const
{
  // Just a safety, should not happen
  
  if(itsEsriShape==NULL)
    return;
  
  // Make sure the attribute exists
  
  const NFmiEsriAttributeName * attribute = itsEsriShape->AttributeName(theFieldName);
  
  // Return if the shape has no such field. Maybe should error instead?
  
  if(attribute==NULL)
    return;
  
  // Iterate through all elements
  
  NFmiEsriShape::const_iterator iter = itsEsriShape->Elements().begin();
  
  for( ; iter != itsEsriShape->Elements().end(); ++iter)
    {
      // There may be deleted elements in the shape, which are to be ignored
	  
      if(*iter==NULL)
		continue;
	  
      // The attribute value
	  
      string fieldvalue = (*iter)->GetString(theFieldName);
	  
      switch((*iter)->Type())
		{
		case kFmiEsriNull:
		case kFmiEsriPolyLine:
		case kFmiEsriPolyLineM:
		case kFmiEsriPolyLineZ:
		case kFmiEsriMultiPatch:
		  break;
		  
		case kFmiEsriPoint:
		case kFmiEsriPointM:
		case kFmiEsriPointZ:
		  {
			int x = static_cast<int>((*iter)->X());
			int y = static_cast<int>((*iter)->Y());
			
			os << '"' << fieldvalue << '"' << "=> " << '"'
			   << x << ',' << y << '"'
			   << endl;
			break;
		  }
		  
		case kFmiEsriMultiPoint:
		case kFmiEsriMultiPointM:
		case kFmiEsriMultiPointZ:
		  {
			const NFmiEsriMultiPoint * elem = static_cast<const NFmiEsriMultiPoint *>(*iter);
			for(int i=0; i<elem->NumPoints(); i++)
			  {
				int x = static_cast<int>(elem->Points()[i].X());
				int y = static_cast<int>(elem->Points()[i].Y());
				os << '"' << fieldvalue << '"' << " => " << '"'
				   << x << ',' << y << '"'
				   << endl;
			  }
			break;
		  }
		  
		case kFmiEsriPolygon:
		case kFmiEsriPolygonM:
		case kFmiEsriPolygonZ:
		  {
			const NFmiEsriPolygon * elem = static_cast<const NFmiEsriPolygon *>(*iter);
			for(int part=0; part<elem->NumParts(); part++)
			  {
				int i1,i2;
				i1 = elem->Parts()[part];		// start of part
				if(part+1 == elem->NumParts())
				  i2 = elem->NumPoints()-1;		// end of part
				else
				  i2 = elem->Parts()[part+1]-1;	// end of part
				
				if(i2>=i1)
				  {
					int lastx = 0;
					int lasty = 0;
					
					os << '"' << fieldvalue << '"' << " => " << '"';
					for(int i=i1; i<=i2; i++)
					  {
						int x = static_cast<int>(elem->Points()[i].X());
						int y = static_cast<int>(elem->Points()[i].Y());
						
						// Do not output point if no move in pixel resolution
						
						if(i!=i1 && x==lastx && y==lasty)
						  continue;
						lastx = x;
						lasty = y;
						os << " " << x << "," << y;
					  }
					os << '"' << endl;
				  }
			  }
			break;
		  }
		}
    }
}


// ======================================================================
