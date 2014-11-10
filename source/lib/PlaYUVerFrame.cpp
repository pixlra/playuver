/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
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
 * \file     PlaYUVerFrame.cpp
 * \brief    Video Frame handling
 */

#include "config.h"
#include <cstdio>
#include "LibMemory.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerFramePixelFormats.h"
#ifdef USE_FFMPEG
#include "LibAvContextHandle.h"
#endif
#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#endif
#ifdef USE_PIXFC
#include "pixfc-sse.h"
#endif

namespace plaYUVer
{

Int PlaYUVerFrame::isRGBorYUVorGray( Int pixel_format )
{
  switch( pixel_format )
  {
  case GRAY:
    return COLOR_GRAY;
    break;
  case YUV420p:
  case YUV444p:
  case YUV422p:
  case YUYV422:
    return COLOR_YUV;
    break;
  case RGB8:
    return COLOR_RGB;
    break;
  }
  return COLOR_INVALID;
}

QStringList PlaYUVerFrame::supportedPixelFormatListNames()
{
  QStringList formats;
  Int numberFormats = PLAYUVER_NUMBER_FORMATS;
  for( Int i = 0; i < numberFormats; i++ )
  {
    formats << g_PlaYUVerFramePelFormatsList[i].name;
  }
  return formats;
}

PlaYUVerFrame::PlaYUVerFrame( UInt width, UInt height, Int pel_format )
{
  init( width, height, pel_format );
}

PlaYUVerFrame::PlaYUVerFrame( PlaYUVerFrame *other )
{
  init( other->getWidth(), other->getHeight(), other->getPelFormat() );
  copyFrom( other );
}

PlaYUVerFrame::PlaYUVerFrame( PlaYUVerFrame *other, QRect area )
{
  adjustSelectedAreaDims( area, other->getPelFormat() );
  init( area.width(), area.height(), other->getPelFormat() );
  copyFrom( other, area.x(), area.y() );
}

PlaYUVerFrame::~PlaYUVerFrame()
{
  if( m_pppcInputPel )
    freeMem3ImageComponents<Pel>( m_pppcInputPel );

  if( m_pcRGB32 )
    freeMem1D( m_pcRGB32 );

  closePixfc();
}

Void PlaYUVerFrame::adjustSelectedAreaDims( QRect &area, Int pel_format )
{
  Int posX = area.x();
  Int posY = area.y();
  Int width = area.width();
  Int height = area.height();

  switch( pel_format )
  {
  case YUV420p:
    if( posX % 2 )
      area.setX( posX - 1 );
    if( posY % 2 )
      area.setY( posY - 1 );

    if( ( posX + width ) % 2 )
      area.setWidth( ( posX + width ) - area.x() + 1 );
    else
      area.setWidth( ( posX + width ) - area.x() );

    if( ( posY + height ) % 2 )
      area.setHeight( ( posY + height ) - area.y() + 1 );
    else
      area.setHeight( ( posY + height ) - area.y() );

    break;
  case YUV444p:
    break;
  case YUV422p:
  case YUYV422:
    if( posX % 2 )
      area.setX( posX - 1 );

    if( ( posX + width ) % 2 )
      area.setWidth( ( posX + width ) - area.x() + 1 );
    else
      area.setWidth( ( posX + width ) - area.x() );

    break;
  case GRAY:
    break;
  case RGB8:
    break;
  default:
    break;
  }
}

Void PlaYUVerFrame::init( UInt width, UInt height, Int pel_format )
{
  m_pppcInputPel = NULL;
  m_pcRGB32 = NULL;
  m_uiWidth = width;
  m_uiHeight = height;
  m_iPixelFormat = pel_format;
  m_iNumberChannels = 3;
  m_iBitsChannels = 8;

  Int max_bits_per_pel = sizeof(Pel) * 8;

  if( m_uiWidth == 0 || m_uiHeight == 0 || m_iPixelFormat == -1 || m_iBitsChannels > max_bits_per_pel )
  {
    return;
  }

  m_pcPelFormat = &( g_PlaYUVerFramePelFormatsList[pel_format] );

  m_bHasRGBPel = false;
  if( m_pcPelFormat->colorSpace == COLOR_GRAY )
  {
    getMem3ImageComponents<Pel>( &m_pppcInputPel, m_uiHeight, m_uiWidth, 1, 1 );
  }
  else
  {
    getMem3ImageComponents<Pel>( &m_pppcInputPel, m_uiHeight, m_uiWidth, m_pcPelFormat->ratioChromaHeight, m_pcPelFormat->ratioChromaWidth );
  }
  getMem1D( &m_pcRGB32, m_uiHeight * m_uiWidth * 4 );

  openPixfc();
}

Int PlaYUVerFrame::getColorSpace() const
{
  return m_pcPelFormat->colorSpace;
}

Int PlaYUVerFrame::getNumberChannels() const
  {
    return m_pcPelFormat->numberChannels;
  }

UInt64 PlaYUVerFrame::getBytesPerFrame()
{
  return getBytesPerFrame( m_uiWidth, m_uiHeight, m_iPixelFormat );
}

UInt64 PlaYUVerFrame::getBytesPerFrame( UInt uiWidth, UInt uiHeight, Int iPixelFormat )
{
  PlaYUVerFramePelFormat* pcPelFormat = &( g_PlaYUVerFramePelFormatsList[iPixelFormat] );
  UInt64 numberBytes = uiWidth * uiHeight;
  if( pcPelFormat->numberChannels > 1 )
  {
    numberBytes += ( pcPelFormat->numberChannels - 1 ) * ( uiWidth * uiHeight ) / ( pcPelFormat->ratioChromaWidth * pcPelFormat->ratioChromaHeight );
  }
  return numberBytes;
}

UInt PlaYUVerFrame::getChromaWidth() const
{
  return m_pcPelFormat->ratioChromaWidth > 0 ? m_uiWidth / m_pcPelFormat->ratioChromaWidth : 0;
}

UInt PlaYUVerFrame::getChromaHeight() const
{
  return m_pcPelFormat->ratioChromaHeight > 0 ? m_uiHeight / m_pcPelFormat->ratioChromaHeight : 0;
}

UInt PlaYUVerFrame::getChromaLength() const
{
  return getChromaWidth() * getChromaHeight();
}

Void PlaYUVerFrame::frameFromBuffer( Pel *Buff, UInt64 uiBuffSize )
{
  if( uiBuffSize != getBytesPerFrame() )
    return;
  m_pcPelFormat->frameFromBuffer( Buff, m_pppcInputPel, m_uiWidth, m_uiHeight );
  m_bHasRGBPel = false;
  fillRGBBuffer();
}

Void PlaYUVerFrame::frameToBuffer( Pel *output_buffer )
{
  m_pcPelFormat->bufferFromFrame( m_pppcInputPel, output_buffer, m_uiWidth, m_uiHeight );
}

Void PlaYUVerFrame::fillRGBBuffer()
{
  if( m_bHasRGBPel )
    return;
  m_pcPelFormat->fillRGBbuffer( m_pppcInputPel, m_pcRGB32, m_uiWidth, m_uiHeight );
  m_bHasRGBPel = true;
}

Void PlaYUVerFrame::copyFrom( PlaYUVerFrame* input_frame )
{
  if( m_iPixelFormat != input_frame->getPelFormat() && m_uiWidth == input_frame->getWidth() && m_uiHeight == input_frame->getHeight() )
    return;
  m_bHasRGBPel = false;
  memcpy( *m_pppcInputPel[LUMA], input_frame->getPelBufferYUV()[LUMA][0], getBytesPerFrame() * sizeof(Pel) );
}

Void PlaYUVerFrame::copyFrom( PlaYUVerFrame* input_frame, UInt xPos, UInt yPos )
{
  if( m_iPixelFormat != input_frame->getPelFormat() )
    return;
  m_bHasRGBPel = false;
  Pel ***pInput = input_frame->getPelBufferYUV();
  for( UInt ch = 0; ch < m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? m_pcPelFormat->ratioChromaHeight : 1;
    Int ratioW = ch > 0 ? m_pcPelFormat->ratioChromaWidth : 1;
    for( UInt i = 0; i < m_uiHeight / ratioH; i++ )
    {
      memcpy( m_pppcInputPel[ch][i], &( pInput[ch][yPos / ratioH + i][xPos / ratioW] ), m_uiWidth / ratioW * sizeof(Pel) );
    }
  }
}

PlaYUVerFrame::Pixel PlaYUVerFrame::getPixelValue( Int xPos, Int yPos, Int eColorSpace )
{
  PlaYUVerFrame::Pixel PixelValue( m_pcPelFormat->colorSpace, 0, 0, 0 );
  for( UInt ch = 0; ch < m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? m_pcPelFormat->ratioChromaHeight : 1;
    Int ratioW = ch > 0 ? m_pcPelFormat->ratioChromaWidth : 1;
    PixelValue.Components()[ch] = m_pppcInputPel[ch][yPos / ratioH][xPos / ratioW];
  }
  return ConvertPixel( PixelValue, eColorSpace );
}

PlaYUVerFrame::Pixel PlaYUVerFrame::ConvertPixel( PlaYUVerFrame::Pixel inputPixel, Int eOutputSpace )
{
  Int outA, outB, outC;
  PlaYUVerFrame::Pixel outPixel(COLOR_INVALID, 0, 0, 0);

  if( inputPixel.ColorSpace() == eOutputSpace || eOutputSpace == COLOR_ARGB || eOutputSpace == COLOR_GRAY )
    return inputPixel;

  if( eOutputSpace == COLOR_RGB )
  {
    yuvToRgb<Int>( inputPixel.Y(), inputPixel.Cb(), inputPixel.Cr(), outA, outB, outC );
    outPixel.R() = outA;
    outPixel.G() = outB;
    outPixel.B() = outC;
  }
  if( eOutputSpace == COLOR_YUV )
  {
    rgbToYuv<Int>( inputPixel.R(), inputPixel.G(), inputPixel.B(), outA, outB, outC );
    outPixel.Y() = outA;
    outPixel.Cb() = outB;
    outPixel.Cr() = outC;
  }
  return outPixel;
}

/*
 **************************************************************
 * Interface to other libs
 **************************************************************
 */

Void PlaYUVerFrame::openPixfc()
{
#ifdef USE_PIXFC
  m_pcPixfc = NULL;
  PixFcPixelFormat input_format = PixFcYUV420P;
  PixFcFlag flags = (PixFcFlag)(PixFcFlag_Default + PixFcFlag_NNbResamplingOnly);
  UInt input_row_size = m_uiWidth;
  if (create_pixfc(&m_pcPixfc, input_format, PixFcRGB24, m_uiWidth, m_uiHeight, input_row_size, m_uiWidth * 3, flags ) != 0)
  {
    fprintf(stderr, "Error creating struct pixfc\n");
    m_pcPixfc = NULL;
    return;
  }
#endif
}

Void PlaYUVerFrame::closePixfc()
{
#ifdef USE_PIXFC
  if( m_pcPixfc )
  destroy_pixfc(m_pcPixfc);
#endif
}

Void PlaYUVerFrame::FrametoRGB8Pixfc()
{
#ifdef USE_PIXFC
  switch( m_iPixelFormat )
  {
    case YUV420p:
    m_pcPixfc->convert(m_pcPixfc, m_pppcInputPel, m_pcRGB32);
    break;
    case YUV422p:
    break;
    case YUV444p:
    case YUYV422:
    m_pcPixfc->convert(m_pcPixfc, m_pppcInputPel, m_pcRGB32);
    break;
    case GRAY:
    break;
    case RGB8:
    break;
    default:
    // No action.
    break;
  }
  m_bHasRGBPel = true;
#endif
}

#ifdef USE_OPENCV
Void PlaYUVerFrame::getCvMat( cv::Mat** ppCvFrame )
{
  Int cvType = CV_8UC3;
  switch( m_iPixelFormat )
  {
  case YUV420p:
  case YUV444p:
  case YUV422p:
  case YUYV422:
    cvType = CV_8UC4;
    break;
  case GRAY:
    cvType = CV_8UC1;
    break;
  case RGB8:
    cvType = CV_8UC3;
    break;
  default:
    break;
  }
  cv::Mat *pcCvFrame = *ppCvFrame = new cv::Mat( m_uiHeight, m_uiWidth, cvType );

  if( m_iPixelFormat != GRAY )
  {
    fillRGBBuffer();
    memcpy( pcCvFrame->data, m_pcRGB32, m_uiWidth * m_uiHeight * 4 * sizeof(UChar) );
  }
  else
  {
    memcpy( pcCvFrame->data, &( m_pppcInputPel[LUMA][0][0] ), m_uiWidth * m_uiHeight * sizeof(Pel) );
  }
}

Void PlaYUVerFrame::copyFrom( cv::Mat* opencvFrame )
{
  if( m_iPixelFormat == NO_FMT )
  {
    switch( opencvFrame->channels() )
    {
    case 1:
      m_iPixelFormat = GRAY;
      break;
    case 4:
      m_iPixelFormat = RGB8;
      break;
    default:
      return;
    }
  }

  if( !isValid() )
  {
    init( opencvFrame->cols, opencvFrame->rows, m_iPixelFormat );
  }

  m_bHasRGBPel = false;

  if( m_iPixelFormat != GRAY )
  {
    Pel* pInputPelY = m_pppcInputPel[LUMA][0];
    Pel* pInputPelU = m_pppcInputPel[CHROMA_U][0];
    Pel* pInputPelV = m_pppcInputPel[CHROMA_V][0];
    Pel* pcRGBPelInterlaced = m_pcRGB32;
    memcpy( pcRGBPelInterlaced, opencvFrame->data, m_uiWidth * m_uiHeight * 4 * sizeof(Pel) );
    UInt* buff = (UInt*)pcRGBPelInterlaced;
    for( UInt i = 0; i < m_uiHeight * m_uiWidth; i++ )
    {
      *pInputPelY++ = pelRed( *buff );
      *pInputPelU++ = pelGreen( *buff );
      *pInputPelV++ = pelBlue( *buff++ );
    }
    m_bHasRGBPel = true;
  }
  else
  {
    memcpy( m_pppcInputPel[LUMA][0], opencvFrame->data, m_uiWidth * m_uiHeight * sizeof(Pel) );
  }
}
#endif


/*
 **************************************************************
 * Quality Related Function API
 **************************************************************
 */


Double PlaYUVerFrame::getQuality( Int Metric, PlaYUVerFrame* Org, Int component )
{
  switch( Metric )
  {
  case PSNR_METRIC:
    return getPSNR( Org, component );
    break;
  case MSE_METRIC:
    return getMSE( Org, component );
    break;
  case SSIM_METRIC:
    return getSSIM( Org, component );
    break;
  default:
    assert( 0 );
  };
  return 0;
}

Double PlaYUVerFrame::getMSE( PlaYUVerFrame* Org, Int component )
{
  Pel* pPelYUV = getPelBufferYUV()[component][0];
  Pel* pOrgPelYUV = Org->getPelBufferYUV()[component][0];

  Int aux_pel_1, aux_pel_2;
  Int diff = 0;
  Double ssd = 0;

  UInt numberOfPixels = 0;
  if( component == LUMA )
  {
    numberOfPixels = Org->getHeight() * Org->getWidth();
  }
  else
  {
    numberOfPixels = getChromaLength();
  }

  for( UInt i = 0; i < numberOfPixels; i++ )
  {
    aux_pel_1 = *pPelYUV++;
    aux_pel_2 = *pOrgPelYUV++;
    diff = aux_pel_1 - aux_pel_2;
    ssd += ( Double )( diff * diff );
  }
  if( ssd == 0.0 )
  {
    return 0.0;
  }
  return ssd / ( Org->getWidth() * Org->getHeight() );
}

Double PlaYUVerFrame::getPSNR( PlaYUVerFrame* Org, Int component )
{
  Double dPSNR = 100;
  Double dMSE = getMSE( Org, component );
  if( dMSE != 0 )
    dPSNR = 10 * log10( 65025 / dMSE );
  return dPSNR;
}

float compute_ssim( Pel **refImg, Pel **encImg, Int width, Int height, Int win_width, Int win_height, Int max_pel_value_comp, Int overlapSize )
{
  static const float K1 = 0.01f, K2 = 0.03f;
  float max_pix_value_sqd;
  float C1, C2;
  float win_pixels = ( float )( win_width * win_height );
#ifdef UNBIASED_VARIANCE
  float win_pixels_bias = win_pixels - 1;
#else
  float win_pixels_bias = win_pixels;
#endif
  float mb_ssim, meanOrg, meanEnc;
  float varOrg, varEnc, covOrgEnc;
  int imeanOrg, imeanEnc, ivarOrg, ivarEnc, icovOrgEnc;
  float cur_distortion = 0.0;
  int i, j, n, m, win_cnt = 0;

  max_pix_value_sqd = ( float )( max_pel_value_comp * max_pel_value_comp );
  C1 = K1 * K1 * max_pix_value_sqd;
  C2 = K2 * K2 * max_pix_value_sqd;

  for( j = 0; j <= height - win_height; j += overlapSize )
  {
    for( i = 0; i <= width - win_width; i += overlapSize )
    {
      imeanOrg = 0;
      imeanEnc = 0;
      ivarOrg = 0;
      ivarEnc = 0;
      icovOrgEnc = 0;

      for( n = j; n < j + win_height; n++ )
      {
        for( m = i; m < i + win_width; m++ )
        {
          imeanOrg += refImg[n][m];
          imeanEnc += encImg[n][m];
          ivarOrg += refImg[n][m] * refImg[n][m];
          ivarEnc += encImg[n][m] * encImg[n][m];
          icovOrgEnc += refImg[n][m] * encImg[n][m];
        }
      }

      meanOrg = ( float )imeanOrg / win_pixels;
      meanEnc = ( float )imeanEnc / win_pixels;

      varOrg = ( ( float )ivarOrg - ( ( float )imeanOrg ) * meanOrg ) / win_pixels_bias;
      varEnc = ( ( float )ivarEnc - ( ( float )imeanEnc ) * meanEnc ) / win_pixels_bias;
      covOrgEnc = ( ( float )icovOrgEnc - ( ( float )imeanOrg ) * meanEnc ) / win_pixels_bias;

      mb_ssim = ( float )( ( 2.0 * meanOrg * meanEnc + C1 ) * ( 2.0 * covOrgEnc + C2 ) );
      mb_ssim /= ( float )( meanOrg * meanOrg + meanEnc * meanEnc + C1 ) * ( varOrg + varEnc + C2 );

      cur_distortion += mb_ssim;
      win_cnt++;
    }
  }

  cur_distortion /= ( float )win_cnt;

  if( cur_distortion >= 1.0 && cur_distortion < 1.01 )  // avoid float accuracy problem at very low QP(e.g.2)
    cur_distortion = 1.0;

  return cur_distortion;
}

Double PlaYUVerFrame::getSSIM( PlaYUVerFrame* Org, Int component )
{
  Double dSSIM = 1;
  if( component == LUMA )
  {
    dSSIM = compute_ssim( m_pppcInputPel[component], Org->getPelBufferYUV()[component], m_uiWidth, m_uiHeight, 8, 8, 255, 8 );
  }
  else
  {
    dSSIM = compute_ssim( m_pppcInputPel[component], Org->getPelBufferYUV()[component], getChromaWidth(), getChromaHeight(), 4, 4, 255, 8 );
  }
  return dSSIM;
}

}  // NAMESPACE
