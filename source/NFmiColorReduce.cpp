// ======================================================================
/*!
 * \file
 * \brief Implementation of namespace Imagine::NFmiColorReduce
 */
// ======================================================================
/*!
 * \namespace Imagine::NFmiColorReduce
 *
 * \brief Tools for reducing the number of colors in an image
 *
 * The main idea in reducing colors in an image is to make it
 * smaller so that the image can be downloaded faster. Typically
 * one sets a maximum number of allowed colors, but often a better
 * alternative might be to reduce colors until the error would become
 * too great.
 *
 */
// ======================================================================

#include "NFmiColorReduce.h"
#include "NFmiImage.h"
#include "NFmiColorTools.h"
#include "NFmiNearTree.h"

#include <iostream>
#include <list>
#include <map>
#include <vector>

using namespace std;

// The actual public interfaces

namespace Imagine
{

  // ======================================================================
  // Local subroutines
  // ======================================================================

  namespace
  {

	//! Histogram information

	typedef map<NFmiColorTools::Color,int> Counter;

	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate the occurrance count of each color in the given image
	 *
	 * \param theImage The image
	 * \return The colormap with occurrance counts
	 */
	// ----------------------------------------------------------------------

	const Counter calc_counts(const NFmiImage & theImage)
	{
	  Counter counter;
	  Counter::iterator it = counter.end();
	  
	  for(int j=0; j<theImage.Height(); j++)
		for(int i=0; i<theImage.Width(); i++)
		  {
			NFmiColorTools::Color color = theImage(i,j);
			
			if(it == counter.end() || it->first != color)
			  {
				pair<Counter::iterator,bool> result = counter.insert(Counter::value_type(color,0));
				it = result.first;
			  }
			it->second++;
		  }
	  
	  return counter;

	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Initialize the given gamma correction table
	 */
	// ----------------------------------------------------------------------

	void init_gamma_table(vector<float> & theTable, bool & theFlag)
	{
	  const float gamma = 2.2;
	  const float coeff = 255/(pow(255,gamma));
	  
	  for(int i=0; i<256; i++)
		theTable[i] = coeff*pow(i,gamma);
	  
	  theFlag = true;

	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief A near-tree of colors and their occurrance counts
	 *
	 * The idea is to feed in colors in the order of their popularity
	 * and then use the near-tree maximum distance information
	 * to group colors for color reduction.
	 */
	// ----------------------------------------------------------------------

	class ColorTree
	{
	public:
	  typedef NFmiColorTools::Color value_type;

	  ColorTree();
	  void insert(value_type theColor);
	  void insert(value_type theColor, float theMaxError);

	  value_type nearest(value_type theColor);

	private:

	  ColorTree(const ColorTree & theTree);
	  ColorTree & operator=(const ColorTree & theTree);

	  static float distance(value_type theColor1, value_type theColor2);
	  bool nearest(value_type theColor, value_type & theNearest, float & theRadius) const;

	  auto_ptr<value_type> itsLeftObject;
	  auto_ptr<value_type> itsRightObject;
	  float itsMaxLeft;
	  float itsMaxRight;
	  auto_ptr<ColorTree> itsLeftBranch;
	  auto_ptr<ColorTree> itsRightBranch;

	};

	// ----------------------------------------------------------------------
	/*!
	 * \brief Default constructor
	 */
	// ----------------------------------------------------------------------

	ColorTree::ColorTree()
	  : itsLeftObject()
	  , itsRightObject()
	  , itsMaxLeft(-1.0)
	  , itsMaxRight(-1.0)
	  , itsLeftBranch()
	  , itsRightBranch()
	{
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Euclidian distance between two colors
	 *
	 * See http://www.compuphase.com/cmetric.htm
	 */
	// ----------------------------------------------------------------------

	float ColorTree::distance(ColorTree::value_type theColor1,
							  ColorTree::value_type theColor2)
	{

	  static vector<float> gamma(256,0);
	  static bool initialized = false;

	  if(!initialized) init_gamma_table(gamma,initialized);

	  const float r = (gamma[NFmiColorTools::GetRed(theColor1)] -
					   gamma[NFmiColorTools::GetRed(theColor2)]);
	  const float g = (gamma[NFmiColorTools::GetGreen(theColor1)] -
					   gamma[NFmiColorTools::GetGreen(theColor2)]);
	  const float b = (gamma[NFmiColorTools::GetBlue(theColor1)] -
					   gamma[NFmiColorTools::GetBlue(theColor2)]);
	  const float a = (NFmiColorTools::GetAlpha(theColor1) -
					   NFmiColorTools::GetAlpha(theColor2));

	  return sqrt(3.0*r*r+4.0*g*g+2.0*b*b+a*a);

	}


	// ----------------------------------------------------------------------
	/*!
	 * \brief Insert a color into the color tree
	 *
	 * Algorithm:
	 *
	 *  -# If left position is free, put the color there
	 *  -# If right position is free, put the color there
	 *  -# Insert into subtree whose root is closer to the color
	 */
	// ----------------------------------------------------------------------

	void ColorTree::insert(ColorTree::value_type theColor)
	{
	  if(itsLeftObject.get() == 0)
		itsLeftObject.reset(new value_type(theColor));

	  else if(itsRightObject.get() == 0)
		itsRightObject.reset(new value_type(theColor));

	  else
		{
		  const float dist_left = distance(theColor,*itsLeftObject);
		  const float dist_right = distance(theColor,*itsRightObject);

		  if(dist_left > dist_right)
			{
			  if(itsRightBranch.get() == 0)
				itsRightBranch.reset(new ColorTree);

			  // note that constructor sets itsMaxRight to be negative

			  itsMaxRight = max(itsMaxRight,dist_right);

			  itsRightBranch->insert(theColor);
			}
		  else
			{
			  if(itsLeftBranch.get() == 0)
				itsLeftBranch.reset(new ColorTree);

			  // note that constructor sets itsMaxLeft to be negative

			  itsMaxLeft = max(itsMaxLeft,dist_left);

			  itsLeftBranch->insert(theColor);

			}
		}
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Insert into colortree unless there already is a closeby color
	 *
	 * \param theColor The color to insert
	 * \param theMaxError The allowed maximum error
	 */
	// ----------------------------------------------------------------------

	void ColorTree::insert(ColorTree::value_type theColor,
						   float theMaxError)
	{
	  // Handle the trivial case quickly
	  if(theMaxError <= 0)
		insert(theColor);

	  else
		{
		  value_type color = nearest(theColor);
		  if(distance(color,theColor) >= theMaxError)
			insert(theColor);
		}
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Find the nearest color
	 *
	 * \param theColor The color for which to find the nearest color
	 * \return The nearest color
	 */
	// ----------------------------------------------------------------------

	ColorTree::value_type ColorTree::nearest(ColorTree::value_type theColor)
	{
	  value_type bestcolor;
	  float radius = -1;
	  if(!nearest(theColor,bestcolor,radius))
		throw runtime_error("Invalid use of color reduction tables");
	  return bestcolor;
	}
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Recursively find nearest color
	 *
	 * This is only intended to be used by the public nearest method
	 */
	// ----------------------------------------------------------------------

	bool ColorTree::nearest(ColorTree::value_type theColor,
							ColorTree::value_type & theNearest,
							float & theRadius) const
	{
	  double tmpradius;
	  bool found = false;

	  // first test each of the left and right positions to see if
	  // one holds a color nearer than the nearest so far discovered

	  if(itsLeftObject.get() != 0)
		{
		  tmpradius = distance(theColor,*itsLeftObject);
		  if(theRadius<0  || tmpradius<=theRadius)
			{
			  theRadius = tmpradius;
			  theNearest = *itsLeftObject;
			  found = true;
			}
		}

	  if(itsRightObject.get() != 0)
		{
		  tmpradius = distance(theColor,*itsRightObject);
		  if(theRadius<0  || tmpradius<=theRadius)
			{
			  theRadius = tmpradius;
			  theNearest = *itsRightObject;
			  found = true;
			}
		}

	  // if theRadius is negative at this point, the tree is empty
	  // on the other hand, if the radius is zero, we found a match

	  if(theRadius <= 0)
		return found;

	  // Now we test to see if the branches below might hold an object
	  // nearer than the best so far found. The triangle rule is used
	  // to test whether it's even necessary to descend

	  if((itsLeftBranch.get() != 0) &&
		 ((theRadius + itsMaxLeft) >= distance(theColor,*itsLeftObject)))
		found |= itsLeftBranch->nearest(theColor,theNearest,theRadius);

	  if((itsRightBranch.get() != 0) &&
		 ((theRadius + itsMaxRight) >= distance(theColor,*itsRightObject)))
		found |= itsRightBranch->nearest(theColor,theNearest,theRadius);

	  return found;
	  
	}

  } // namespace anonymous


  // ======================================================================
  // The actual public interfaces
  // ======================================================================

  namespace NFmiColorReduce
  {
	
	// ----------------------------------------------------------------------
	/*!
	 * \brief Calculate the occurrance count of each color in the given image
	 *
	 * \param theImage The image
	 * \return The histogram object
	 */
	// ----------------------------------------------------------------------
	
	const Histogram CalcHistogram(const NFmiImage & theImage)
	{
	  Counter counter = calc_counts(theImage);
	  
	  Histogram histogram;
	  
	  for(Counter::const_iterator it = counter.begin();
		  it!=counter.end();
		  ++it)
		{
		  histogram.insert(Histogram::value_type(it->second,it->first));
		}
	  
	  return histogram;
	
	}

	// ----------------------------------------------------------------------
	/*!
	 * \brief Reduce colors from the image adaptively
	 *
	 * Algorithm:
	 *
	 *  -# Calculate the histogram
	 *  -# In order of popularity, feed the colors into a color tree
	 *     which discards colors which are close enough to a color
	 *     chosen earlier.
	 *  -# For each color, ask the color tree which is the nearest
	 *     color for it.
	 *  -# Replace the original colors with the colors found in the previous
	 *     step.
	 *
	 * Note that this is very very close to the common popularity based
	 * algorithm. However, the twist is that we have an adaptive error
	 * criteria, we are not aiming for a fixed number of colors. Hence
	 * we get the benefit of the speed of the popularity based algorithm
	 * but will never get too big errors.
	 *
	 * An algorithm which might produce better quality results would
	 * be to aim for color diversity as follows:
	 *
	 *  -# Perform the insertion phase as in algorithm 1, but
	 *     with the error limit doubled.
	 *  -# Perform the insertion phase as in algorithm 1, now
	 *     with a normal error limit
	 *  -# Continue by asking the nearest color for all colors
	 *
	 * However, it is not clear that doubling the error in phase 1
	 * would be the ideal approach. The ideal multiplier may be
	 * less than or more than 2.
	 *
	 * \param theImage The image to modify
	 * \param theMaxError The allowed maximum error
	 */
	// ----------------------------------------------------------------------
	
	void AdaptiveReduce(NFmiImage & theImage, float theMaxError)
	{
	  using namespace Imagine::NFmiColorTools;
	  
	  // A sanity check on the maximum error
	  
	  if(theMaxError < 1)
		return;
	  
	  // Calculate the histogram
	  
	  Histogram histogram = CalcHistogram(theImage);

	  // Build a pruned tree of colors. All colors popular enough
	  // (more than 1%) are forced in, others have an error criterion

	  ColorTree tree;

	  {
		const float limit = 0.01*theImage.Width()*theImage.Height();

		for(Histogram::const_iterator it = histogram.begin();
			it != histogram.end();
			++it)
		  {
			if(it->first >= limit)
			  tree.insert(it->second);
			else
			  tree.insert(it->second,2*theMaxError);
		  }
	  }

	  {
		for(Histogram::const_iterator it = histogram.begin();
			it != histogram.end();
			++it)
		  {
			tree.insert(it->second,theMaxError);
		  }
	  }

	  // Then build the colormap transformation map

	  typedef map<Color,Color> ColorMap;
	  ColorMap colormap;

	  {
		for(Histogram::const_iterator it = histogram.begin();
			it != histogram.end();
			++it)
		  {
			Color color = tree.nearest(it->second);
			colormap.insert(ColorMap::value_type(it->second,color));
		  }
	  }

	  // Now perform the replacements. Since we expect to encounter
	  // sequences of color, we speed of the searches by caching
	  // the last replacement.

	  NFmiColorTools::Color last_color = NFmiColorTools::NoColor;
	  NFmiColorTools::Color last_choice = NFmiColorTools::NoColor;
	  
	  for(int j=0; j<theImage.Height(); j++)
		for(int i=0; i<theImage.Width(); i++)
		  {
			if(theImage(i,j) != last_color)
			  {
				last_color = theImage(i,j);

				ColorMap::const_iterator it = colormap.find(last_color);
				if(it == colormap.end())
				  throw runtime_error("Internal error in color reduction, failed to find color");
				last_choice = it->second;
			  }
			theImage(i,j) = last_choice;
		  }
	}
	
  } // namespace NFmiColorReduce
} // namespace Imagine

// ======================================================================

