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

	  // Safety check

	  if(theImage.Height() * theImage.Width() == 0)
		return counter;

	  // Insert the first color so that we can initialize the iterator cache
	  // Note that we insert count 0, but the first loop will fix the number

	  counter.insert(Counter::value_type(theImage(0,0),0));

	  Counter::iterator last1 = counter.begin();
	  Counter::iterator last2 = counter.begin();
	  Counter::iterator last3 = counter.begin();
	  
	  for(int j=0; j<theImage.Height(); j++)
		for(int i=0; i<theImage.Width(); i++)
		  {
			NFmiColorTools::Color color = theImage(i,j);
			
			if(last1->first == color)
			  ++last1->second;
			else if(last2->first == color)
			  {
				++last2->second;
				swap(last1,last2);
			  }
			else if(last3->first == color)
			  {
				++last3->second;
				swap(last2,last3);
				swap(last1,last2);
			  }
			else
			  {
				pair<Counter::iterator,bool> result = counter.insert(Counter::value_type(color,0));
				last3 = last2;
				last2 = last1;
				last1 = result.first;
				++last1->second;
			  }
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

	  bool empty() const;
	  value_type nearest(value_type theColor);

	  static float distance(value_type theColor1, value_type theColor2);

	private:

	  ColorTree(const ColorTree & theTree);
	  ColorTree & operator=(const ColorTree & theTree);

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
	 * \brief Test whether the tree is empty
	 */
	// ----------------------------------------------------------------------

	bool ColorTree::empty() const
	{
	  return (itsLeftObject.get() != 0);
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
	  float left_dist = -1;
	  float right_dist = -1;
	  bool found = false;

	  // first test each of the left and right positions to see if
	  // one holds a color nearer than the nearest so far discovered

	  if(itsLeftObject.get() != 0)
		{
		  left_dist = distance(theColor,*itsLeftObject);
		  if(theRadius<0  || left_dist<=theRadius)
			{
			  theRadius = left_dist;
			  theNearest = *itsLeftObject;
			  found = true;
			}
		}

	  if(itsRightObject.get() != 0)
		{
		  right_dist = distance(theColor,*itsRightObject);
		  if(theRadius<0  || right_dist<=theRadius)
			{
			  theRadius = right_dist;
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
	  // to test whether it's even necessary to descend. We may be
	  // able to skip skanning both branches if we can guess which
	  // branch is most likely to contain the nearest match. We simply
	  // guess, that it is the branch which is nearer. Note that
	  // the first and third if-clauses are mutually exclusive,
	  // hence only parts 1-2 or 2-3 will be executed.

	  const bool left_closer = (left_dist < right_dist);

	  if(!left_closer &&
		 (itsRightBranch.get() != 0) &&
		 ((theRadius + itsMaxRight) >= right_dist))
		found |= itsRightBranch->nearest(theColor,theNearest,theRadius);

	  if((itsLeftBranch.get() != 0) &&
		 ((theRadius + itsMaxLeft) >= left_dist))
		found |= itsLeftBranch->nearest(theColor,theNearest,theRadius);

	  if(left_closer &&
		 (itsRightBranch.get() != 0) &&
		 ((theRadius + itsMaxRight) >= right_dist))
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
	 * However, we require best possible speed and thus do everything
	 * in a single pass. What we could do is to find a way to quickly
	 * reorder the histogram to produce more diversity in the colors.
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

	  typedef map<Color,Color> ColorMap;
	  ColorMap colormap;

	  ColorTree tree;

	  {
		const float limit = 0.01*theImage.Width()*theImage.Height();
		
		for(Histogram::const_iterator it = histogram.begin();
			it != histogram.end();
			++it)
		  {
			if(it->first >= limit || tree.empty())
			  {
				tree.insert(it->second);
				colormap.insert(ColorMap::value_type(it->second,it->second));
			  }
			else
			  {
				NFmiColorTools::Color nearest = tree.nearest(it->second);
				float dist = tree.distance(nearest,it->second);
				if(dist < theMaxError)
				  colormap.insert(ColorMap::value_type(it->second,nearest));
				else
				  {
					tree.insert(it->second);
					colormap.insert(ColorMap::value_type(it->second,it->second));
				  }
			  }
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
