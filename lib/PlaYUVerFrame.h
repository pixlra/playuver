/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2016  by Luis Lucas      (luisfrlucas@gmail.com)
 *                                Joao Carreira   (jfmcarreira@gmail.com)
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
 * \ingroup  PlaYUVerLib
 * \brief    Video Frame handling
 */

#ifndef __PLAYUVERFRAME_H__
#define __PLAYUVERFRAME_H__

#include <iostream>
#include <cstdio>
#include <cassert>
#include <vector>
#include "PlaYUVerDefs.h"
#include "PlaYUVerPixel.h"


namespace cv
{
class Mat;
}

namespace plaYUVer
{

struct PlaYUVerPixFmtDescriptor;

#define CHROMASHIFT( SIZE, SHIFT ) UInt( -( ( - ( Int( SIZE ) ) ) >> SHIFT ) )

/**
 * \class    PlaYUVerFrame
 * \ingroup  PlaYUVerLib PlaYUVerLib_Frame
 * \brief    Frame handling class
 */
class PlaYUVerFrame
//: public PlaYUVerFrameStats
{
public:

  /** ColorSpace Enum (deprecated)
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
    RGB24,
    BRG24,
    NUMBER_FORMATS
  };

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
   * Function that handles the supported color space
   * of PlaYUVerFrame
   * @return vector of strings with pixel formats names
   */
  static std::vector<std::string> supportedColorSpacesListNames();

  /**
   * Function that handles the supported pixel formats
   * of PlaYUVerFrame
   * @return vector of strings with pixel formats names
   */
  static std::vector<std::string> supportedPixelFormatListNames();
  static std::vector<std::string> supportedPixelFormatListNames( Int colorSpace );
  static Int findPixelFormat( std::string name );

  /**
   * Get number of bytes per frame of a specific
   * pixel format
   * @return number of bytes per frame
   */
  static UInt64 getBytesPerFrame( UInt uiWidth, UInt uiHeight, Int iPixelFormat, UInt bitsPixel );

  /**
   * Creates a new frame using the following configuration
   *
   * @param width width of the frame
   * @param height height of the frame
   * @param pel_format pixel format index (always use PixelFormats enum)
   *
   * @note this function might misbehave if the pixel format enum is not correct
   */
  PlaYUVerFrame( UInt width, UInt height, Int pelFormat = 0, Int bitsPixel = 8, Int endianness = -1 );

  /**
   * Creates and new frame with the configuration of an
   * existing one and copy its contents
   *
   * @param other existing frame to copy from
   */
  PlaYUVerFrame( PlaYUVerFrame *other, Bool copy = true );

  /**
   * Creates and new frame with the configuration of an
   * existing one and copy its contents.
   * This function only copies a specific region from the existing frame
   *
   * @param other existing frame to copy from
   * @param posX position X to crop from
   * @param posY position Y to crop from
   * @param areaWidth crop width
   * @param areaHeight crop height
   */
  PlaYUVerFrame( PlaYUVerFrame *other, UInt posX, UInt posY, UInt areaWidth, UInt areaHeight );

  ~PlaYUVerFrame();

  Void clear();

  UInt getWidth() const
  {
    return m_uiWidth;
  }
  UInt getHeight() const
  {
    return m_uiHeight;
  }
  Int getPelFormat() const
  {
    return m_iPixelFormat;
  }
  UInt getBitsPel() const
  {
    return m_uiBitsPel;
  }
  Int getEndianness() const
  {
    return m_iEndianness;
  }
  Pel*** getPelBufferYUV() const
  {
    return m_pppcInputPel;
  }
  Pel*** getPelBufferYUV()
  {
    m_bHasHistogram = false;
    m_bHasRGBPel = false;
    return m_pppcInputPel;
  }

  UChar* getRGBBuffer() const
  {
    if( m_bHasRGBPel )
    {
      return m_pcARGB32;
    }
    return NULL;
  }
  UChar* getRGBBuffer()
  {
    if( !m_bHasRGBPel )
    {
      fillRGBBuffer();
    }
    return m_pcARGB32;
  }

  /**
   * Get number of bytes per frame of an existing frame
   * @return number of bytes per frame
   */
  UInt64 getBytesPerFrame();

  Int getColorSpace() const;
  UInt getNumberChannels() const;

  UInt getPixels() const;
  UInt8 getChromaWidthRatio() const;
  UInt8 getChromaHeightRatio() const;
  UInt getChromaWidth() const;
  UInt getChromaHeight() const;
  UInt getChromaLength() const;
  UInt getChromaSize() const;

  PlaYUVerPixel getPixelValue( Int xPos, Int yPos );
  PlaYUVerPixel getPixelValue( Int xPos, Int yPos, PlaYUVerPixel::ColorSpace eColorSpace );
  Void setPixelValue( Int xPos, Int yPos, PlaYUVerPixel pixel );

  Void copyFrom( PlaYUVerFrame* );
  Void copyFrom( PlaYUVerFrame*, UInt, UInt );

  Void frameFromBuffer( Byte*, UInt64 );
  Void frameFromBuffer( Byte* );
  Void frameToBuffer( Byte* );

  Void fillRGBBuffer();

  Bool haveSameFmt( PlaYUVerFrame* other, UInt match = MATCH_ALL ) const;

  std::string getPelFmtName();

  /**
   * Histogram
   */
  Void calcHistogram();

  UInt getCount( Int channel, UInt start, UInt end );
  Double getMean( Int channel, UInt start, UInt end );
  Int getMedian( Int channel, UInt start, UInt end );
  Double getStdDev( Int channel, UInt start, UInt end );
  Double getHistogramValue( Int channel, UInt bin );
  Double getMaximum( Int channel );
  Int getHistogramSegment();

  Void setRunningFlag( Bool bFlag )
  {
    m_bRunningFlag = bFlag;
  }
  Bool getHasHistogram()
  {
    return m_bHasHistogram;
  }

  /**
   * Interface with OpenCV lib
   */
  cv::Mat* getCvMat( Bool convertToGray = false );
  Void fromCvMat( cv::Mat* );

  /**
   * @defgroup PlaYUVerLib_QualityMetrics Quality Metrics Interface
   * @{
   * \ingroup PlaYUVerLib
   * Quality metrics interface
   *
   * @}
   */

  //! @ingroup PlaYUVerLib_QualityMetrics
  //! @{
  enum QualityMetrics
  {
    NO_METRIC = -1,
    PSNR_METRIC = 0,
    MSE_METRIC,
    SSIM_METRIC,
    NUMBER_METRICS,
  };

  static std::vector<std::string> supportedQualityMetricsList()
  {
    std::vector<std::string> metrics;
    metrics.push_back( "PSNR" );
    metrics.push_back( "MSE" );
    metrics.push_back( "SSIM" );
    return metrics;
  }

  Double getQuality( Int Metric, PlaYUVerFrame* Org, Int component );
  Double getMSE( PlaYUVerFrame* Org, Int component );
  Double getPSNR( PlaYUVerFrame* Org, Int component );
  Double getSSIM( PlaYUVerFrame* Org, Int component );

  //! @}

private:

  Bool m_bInit;

  //! Struct with the pixel format description.
  PlaYUVerPixFmtDescriptor* m_pcPelFormat;
  std::string m_cPelFmtName;

  UInt m_uiWidth;  //!< Width of the frame
  UInt m_uiHeight;  //!< Height of the frame
  Int m_iPixelFormat;  //!< Pixel format number (it follows the list of supported pixel formats)
  Int m_iNumberChannels;  //!< Number of channels
  UInt m_uiBitsPel;  //!< Bits per pixel/channel
  Int m_iEndianness;  //!< Endiannes of bytes
  UInt m_uiHalfPelValue;  //!< Bits per pixel/channel

  Pel*** m_pppcInputPel;

  Bool m_bHasRGBPel;  //!< Flag indicating that the ARGB buffer was computed
  UChar* m_pcARGB32;  //!< Buffer with the ARGB pixels used in Qt libs

  /** The histogram data.*/
  UInt* m_puiHistogram;

  /** If the image is RGB and calcLuma is true, we have 1 more channel */
  UInt m_uiHistoChannels;

  /** Numbers of histogram segments depending of image bytes depth*/
  UInt m_uiHistoSegments;

  /** Used to stop thread during calculations.*/
  Bool m_bRunningFlag;

  Int getRealHistoChannel( Int channel );

  Bool m_bHasHistogram;

  /**
   * Common constructor function of a frame
   *
   * @param width width of the frame
   * @param height height of the frame
   * @param pel_format pixel format index (always use PixelFormats enum)
   *
   */
  Void init( UInt width, UInt height, Int pel_format, Int bitsPixel, Int endianness );

  Void FrametoRGB8Pixfc();
};

}  // NAMESPACE

#endif // __PLAYUVERFRAME_H__
