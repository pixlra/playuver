/*    This file is a part of PlaYUVer project
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
 * \file     PlaYUVerFrame.h
 * \brief    Video Frame handling
 */

#ifndef __PLAYUVERFRAME_H__
#define __PLAYUVERFRAME_H__

#include "PlaYUVerDefs.h"
#include "PlaYUVerPixel.h"

namespace cv
{
class Mat;
}

struct PlaYUVerPixFmtDescriptor;
struct PlaYUVerFramePrivate;

#define CHROMASHIFT( SIZE, SHIFT ) UInt( -( ( -( Int( SIZE ) ) ) >> ( SHIFT ) ) )

/**
 * \class    PlaYUVerFrame
 * \ingroup  PlaYUVerLib PlaYUVerLib_Frame
 * \brief    Frame handling class
 */
class PlaYUVerFrame
{
public:
  /** ColorSpace Enum
   * List of supported pixel formats - do not change order
   */
  enum PixelFormats
  {
    NO_FMT = -1,
    YUV420p = 0,
    YUV444p,
    YUV422p,
    YUYV422,
    GRAY,
    RGBp,
    RGB24,
    BGR24,
    NUMBER_PEL_FORMATS
  };

  /**
   * Function that handles the supported color space
   * of PlaYUVerFrame
   * @return vector of strings with pixel formats names
   */
  static std::vector<String> supportedColorSpacesListNames();

  /**
   * Function that handles the supported pixel formats
   * of PlaYUVerFrame
   * @return vector of strings with pixel formats names
   */
  static std::vector<String> supportedPixelFormatListNames();
  static std::vector<String> supportedPixelFormatListNames( Int colorSpace );
  static Int findPixelFormat( const String& name );

  /**
   * Creates a new frame using the following configuration
   *
   * @param width width of the frame
   * @param height height of the frame
   * @param pel_format pixel format index (always use PixelFormats enum)
   *
   * @note this function might misbehave if the pixel format enum is not correct
   */
  PlaYUVerFrame( UInt width, UInt height, Int pelFormat, Int bitsPixel = 8 );

  /**
   * Copy contructor
   *
   * @param other existing frame to copy from
   */
  PlaYUVerFrame( const PlaYUVerFrame& other );
  PlaYUVerFrame( const PlaYUVerFrame* other );

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
  PlaYUVerFrame( const PlaYUVerFrame& other, UInt x, UInt y, UInt width, UInt height );
  PlaYUVerFrame( const PlaYUVerFrame* other, UInt x, UInt y, UInt width, UInt height );

  ~PlaYUVerFrame();

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
   * Check if two PlaYUVerFrames have the same fmt
   * @param other frame to compare with
   * @param match matching conditions (use enum FormatMatching)
   * @return true if format matches
   */
  Bool haveSameFmt( const PlaYUVerFrame& other, UInt match = MATCH_ALL ) const;
  Bool haveSameFmt( const PlaYUVerFrame* other, UInt match = MATCH_ALL ) const;

  /**
   * Get pixel format information
   * @return pixel format index
   */
  Int getPelFormat() const;

  /**
   * Get pixel format information
   * @return pixel format name
   */
  String getPelFmtName();

  /**
   * Get color space information
   * @return get color space index
   */
  Int getColorSpace() const;

  /**
   * Get the number of channels
   * @return number of channels
   */
  UInt getNumberChannels() const;

  /**
   * Get width of the frame
   * @param channel/component
   * @return number of pixels
   */
  UInt getWidth( Int channel = 0 ) const;

  /**
   * Get height of the frame
   * @param channel/component
   * @return number of pixels
   */
  UInt getHeight( Int channel = 0 ) const;

  /**
   * Get number of pixels of the frame
   * @param channel/component
   * @return number of pixels
   */
  UInt getPixels( Int channel = 0 ) const;

  UInt8 getChromaWidthRatio() const;
  UInt8 getChromaHeightRatio() const;
  UInt getChromaWidth() const;
  UInt getChromaHeight() const;
  UInt getChromaLength() const;
  UInt getChromaSize() const;

  /**
   * Get number of bits per pixel
   * @return number of bits
   */
  UInt getBitsPel() const;

  /**
   * Get number of bytes per frame of an existing frame
   * @return number of bytes per frame
   */
  UInt64 getBytesPerFrame();

  /**
   * Get number of bytes per frame of a specific pixel format
   * @return number of bytes per frame
   */
  static UInt64 getBytesPerFrame( UInt uiWidth, UInt uiHeight, Int iPixelFormat, UInt bitsPixel );

  /**
   * Reset frame pixels to zero
   */
  Void clear();

  Pel*** getPelBufferYUV() const;
  Pel*** getPelBufferYUV();
  UChar* getRGBBuffer() const;
  UChar* getRGBBuffer();

  PlaYUVerPixel getPixelValue( Int xPos, Int yPos );
  PlaYUVerPixel getPixelValue( Int xPos, Int yPos, PlaYUVerPixel::ColorSpace eColorSpace );
  Void setPixelValue( Int xPos, Int yPos, PlaYUVerPixel pixel );

  Void copyFrom( const PlaYUVerFrame& );
  Void copyFrom( const PlaYUVerFrame* );
  Void copyFrom( const PlaYUVerFrame&, UInt, UInt );
  Void copyFrom( const PlaYUVerFrame*, UInt, UInt );

  Void frameFromBuffer( Byte*, Int, UInt64 );
  Void frameFromBuffer( Byte*, Int );
  Void frameToBuffer( Byte*, Int );

  Void fillRGBBuffer();

  /**
   * Histogram
   */
  Void calcHistogram();

  UInt getMin( Int channel );
  UInt getMax( Int channel );

  UInt getCount( Int channel, UInt start, UInt end );
  Double getMean( Int channel, UInt start, UInt end );
  Int getMedian( Int channel, UInt start, UInt end );
  Double getStdDev( Int channel, UInt start, UInt end );
  Double getHistogramValue( Int channel, UInt bin );
  Double getMaximum( Int channel );
  Int getHistogramSegment();

  /**
   * Interface with OpenCV lib
   */
  Bool toMat( cv::Mat& cvMat, Bool convertToGray = false );
  Bool fromMat( cv::Mat& cvMat );

  /**
   * \ingroup  PlaYUVerLib_Frame
   * @defgroup PlaYUVerLib_Frame_QualityMetrics Quality Metrics Interface
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

  static std::vector<String> supportedQualityMetricsList();
  Double getQuality( Int Metric, PlaYUVerFrame* Org, Int component );
  Double getMSE( PlaYUVerFrame* Org, Int component );
  Double getPSNR( PlaYUVerFrame* Org, Int component );
  Double getSSIM( PlaYUVerFrame* Org, Int component );

  /** @} */

private:
  PlaYUVerFramePrivate* d;
};

#endif  // __PLAYUVERFRAME_H__
