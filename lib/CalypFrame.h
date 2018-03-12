/*    This file is a part of Calyp project
 *    Copyright (C) 2014-2018  by Joao Carreira   (jfmcarreira@gmail.com)
 *                                Luis Lucas      (luisfrlucas@gmail.com)
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * \file     CalypFrame.h
 * \ingroup	 CalypFrameGrp
 * \brief    Video Frame handling
 */

#ifndef __CALYPFRAME_H__
#define __CALYPFRAME_H__

#include "CalypDefs.h"

namespace cv
{
class Mat;
}

#define CHROMASHIFT( SIZE, SHIFT ) (unsigned int)( -( ( -( (int)( SIZE ) ) ) >> ( SHIFT ) ) )

/**
 * \class    CalypPixel
 * \ingroup  CalypLibGrp CalypFrameGrp
 * \brief    Pixel handling class
 */
class CalypPixel
{
public:
  static int getMaxNumberOfComponents();

  CalypPixel( const int& CalypColorSpace = CLP_COLOR_INVALID );
  CalypPixel( const int& CalypColorSpace, const ClpPel& c0 );
  CalypPixel( const int& CalypColorSpace, const ClpPel& c0, const ClpPel& c1, const ClpPel& c2 );
  CalypPixel( const int& CalypColorSpace, const ClpPel& c0, const ClpPel& c1, const ClpPel& c2, const ClpPel& c3 );
  CalypPixel( const CalypPixel& other );
  ~CalypPixel();

  int colorSpace() const;

  ClpPel operator[]( const int& channel ) const;
  ClpPel& operator[]( const int& channel );

  CalypPixel operator=( const CalypPixel& );
  CalypPixel operator+( const CalypPixel& );
  CalypPixel operator+=( const CalypPixel& );
  CalypPixel operator-( const CalypPixel& );
  CalypPixel operator-=( const CalypPixel& );
  CalypPixel operator*( const double& );

  /**
	 * Convert a Pixel to a new color space
	 * @param inputPixel input pixel (CalypPixel)
	 * @param eOutputSpace output color space
	 * @return converted pixel
	 */
  CalypPixel convertPixel( CalypColorSpace eOutputSpace );

private:
  struct CalypPixelPrivate* d;
};

/**
 * \class    CalypFrame
 * \ingroup	 CalypLibGrp CalypFrameGrp
 * \brief    Frame handling class
 */
class CalypFrame
{
public:
  /**
	 * Function that handles the supported color space
	 * of CalypFrame
	 * @return vector of strings with pixel formats names
	 */
  static std::vector<ClpString> supportedColorSpacesListNames();

  /**
	 * Function that handles the supported pixel formats
	 * of CalypFrame
	 * @return vector of strings with pixel formats names
	 */
  static std::vector<ClpString> supportedPixelFormatListNames();
  static std::vector<ClpString> supportedPixelFormatListNames( int colorSpace );
  static int numberOfFormats();
  static int findPixelFormat( const ClpString& name );

  /**
	 * Creates a new frame using the following configuration
	 *
	 * @param width width of the frame
	 * @param height height of the frame
	 * @param pel_format pixel format index (always use PixelFormats enum)
	 *
	 * @note this function might misbehave if the pixel format enum is not correct
	 */
  CalypFrame( unsigned int width, unsigned int height, int pelFormat, int bitsPixel = 8 );

  /**
	 * Copy contructor
	 *
	 * @param other existing frame to copy from
	 */
  CalypFrame( const CalypFrame& other );
  CalypFrame( const CalypFrame* other );

  /**
	 * Creates and new frame with the configuration of an existing one and copy
	 * its contents. This function only copies a specific region from the existing
	 * frame
	 *
	 * @param other existing frame to copy from
	 * @param posX position X to crop from
	 * @param posY position Y to crop from
	 * @param areaWidth crop width
	 * @param areaHeight crop height
	 */
  CalypFrame( const CalypFrame& other, unsigned int x, unsigned int y, unsigned int width, unsigned int height );
  CalypFrame( const CalypFrame* other, unsigned int x, unsigned int y, unsigned int width, unsigned int height );

  ~CalypFrame();

  /** Format match opts
	 */
  enum FormatMatching
  {
    MATCH_COLOR_SPACE = 1,
    MATCH_RESOLUTION = 2,
    MATCH_PEL_FMT = 4,
    MATCH_BITS = 8,
    MATCH_COLOR_SPACE_IGNORE_GRAY = 16,
    MATCH_ALL = 0xFFFF,
  };

  /**
	 * Check if two CalypFrames have the same fmt
	 * @param other frame to compare with
	 * @param match matching conditions (use enum FormatMatching)
	 * @return true if format matches
	 */
  bool haveSameFmt( const CalypFrame& other, unsigned int match = MATCH_ALL ) const;
  bool haveSameFmt( const CalypFrame* other, unsigned int match = MATCH_ALL ) const;

  /**
	 * Get pixel format information
	 * @return pixel format index
	 */
  int getPelFormat() const;

  /**
	 * Get pixel format information
	 * @return pixel format name
	 */
  ClpString getPelFmtName();

  /**
	 * Get color space information
	 * @return get color space index
	 */
  int getColorSpace() const;

  /**
	 * Get the number of channels
	 * @return number of channels
	 */
  unsigned int getNumberChannels() const;

  /**
	 * Get width of the frame
	 * @param channel/component
	 * @return number of pixels
	 */
  unsigned int getWidth( int channel = 0 ) const;

  /**
	 * Get height of the frame
	 * @param channel/component
	 * @return number of pixels
	 */
  unsigned int getHeight( int channel = 0 ) const;

  /**
	 * Get number of pixels of the frame
	 * @param channel/component
	 * @return number of pixels
	 */
  unsigned int getPixels( int channel = 0 ) const;

  /**
	 * Get chroma width ratio
	 * @return ratio multiple of 2
	 */
  unsigned char getChromaWidthRatio() const;

  /**
	 * Get chroma height ratio
	 * @return ratio multiple of 2
	 */
  unsigned char getChromaHeightRatio() const;

  /**
	 * Get number of pixels in each chroma channel
	 * @return number of pixels
	 */
  unsigned int getChromaLength() const;

  /**
	 * Get number of bits per pixel
	 * @return number of bits
	 */
  unsigned int getBitsPel() const;

  /**
	 * Get number of bytes per frame of an existing frame
	 * @return number of bytes per frame
	 */
  unsigned long getBytesPerFrame();

  /**
	 * Get number of bytes per frame of a specific pixel format
	 * @return number of bytes per frame
	 */
  static unsigned long getBytesPerFrame( unsigned int uiWidth, unsigned int uiHeight, int iPixelFormat, unsigned int bitsPixel );

  /**
	 * Reset frame pixels to zero
	 */
  void reset();

  ClpPel*** getPelBufferYUV() const;
  ClpPel*** getPelBufferYUV();

  unsigned char* getRGBBuffer() const;

  /**
	 * Get pixel value at coordinates
	 * @param ch frame channel
	 * @param xPos position in X axis
	 * @param yPos position in Y axis
	 * @return pixel value
	 */
  ClpPel operator()( unsigned int ch, unsigned int xPos, unsigned int yPos );

  /**
	 * Get pixel value at coordinates
	 * @param xPos position in X axis
	 * @param yPos position in Y axis
	 * @return pixel value
	 */
  CalypPixel operator()( unsigned int xPos, unsigned int yPos );

  /**
	 * Get pixel value at coordinates
	 * @param xPos position in X axis
	 * @param yPos position in Y axis
	 * @return pixel value
	 */
  CalypPixel getPixel( unsigned int xPos, unsigned int yPos );

  /**
	 * Get pixel value at coordinates
	 * in the desired color space
	 * @param xPos position in X axis
	 * @param yPos position in Y axis
	 * @param eColorSpace desired color space
	 * @return pixel value
	 */
  CalypPixel getPixel( unsigned int xPos, unsigned int yPos, CalypColorSpace eColorSpace );

  /**
	 * Set pixel value at coordinates to a given value
	 * @param xPos position in X axis
	 * @param yPos position in Y axis
	 */
  void setPixel( unsigned int xPos, unsigned int yPos, CalypPixel pixel );

  void copyFrom( const CalypFrame& );
  void copyFrom( const CalypFrame* );
  void copyFrom( const CalypFrame&, unsigned int, unsigned int );
  void copyFrom( const CalypFrame*, unsigned int, unsigned int );

  void frameFromBuffer( ClpByte*, int, unsigned long );
  void frameFromBuffer( ClpByte*, int );
  void frameToBuffer( ClpByte*, int );

  void fillRGBBuffer();

  /**
	 * Histogram
	 */
  enum HistogramChannels
  {
    HIST_CHAN_ONE = 0,
    HIST_CHAN_TWO,
    HIST_CHAN_THREE,
    HIST_CHAN_FOUR,
    HIST_LUMA = 10,
    HIST_CHROMA_U,
    HIST_CHROMA_V,
    HIST_COLOR_R = 20,
    HIST_COLOR_G,
    HIST_COLOR_B,
    HIST_COLOR_A,
    HIST_ALL_CHANNELS = 254,
    HISTOGRAM_MAX = 255,
  };
  void calcHistogram();

  unsigned int getMinimumPelValue( int channel );
  unsigned int getMaximumPelValue( int channel );

  unsigned int getMaximum( int channel );
  unsigned int getNumPixelsRange( int channel, unsigned int start, unsigned int end );
  double getMean( int channel, unsigned int start, unsigned int end );
  int getMedian( int channel, unsigned int start, unsigned int end );
  double getStdDev( int channel, unsigned int start, unsigned int end );
  double getHistogramValue( int channel, unsigned int bin );
  int getNumHistogramSegment();

  /**
	 * interface with OpenCV lib
	 */
  bool toMat( cv::Mat& cvMat, bool convertToGray = false );
  bool fromMat( cv::Mat& cvMat );

  /**
	 * \ingroup	 CalypFrameGrp
	 * @defgroup CalypFrameQualityMetricsGrp Calyp Frame Quality Metrics interface
	 * @{
	 * Quality metrics interface
	 *
	 */

  enum QualityMetrics
  {
    NO_METRIC = -1,
    PSNR_METRIC = 0,
    MSE_METRIC,
    SSIM_METRIC,
    NUMBER_METRICS,
  };

  static std::vector<ClpString> supportedQualityMetricsList();
  double getQuality( int Metric, CalypFrame* Org, int component );
  double getMSE( CalypFrame* Org, int component );
  double getPSNR( CalypFrame* Org, int component );
  double getSSIM( CalypFrame* Org, int component );

  /** @} */

private:
  struct CalypFramePrivate* d;
};

#endif  // __CALYPFRAME_H__
