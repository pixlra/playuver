/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2015  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     PlaYUVerFrame.cpp
 * \ingroup  PlaYUVerLib
 * \brief    Video Frame handling
 */

#include "config.h"
#include <cstdio>
#include <cmath>
#include "LibMemory.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerFramePixelFormats.h"
#ifdef USE_FFMPEG
#include "LibAvContextHandle.h"
#endif
#ifdef USE_OPENCV
#include "LibOpenCVHandler.h"
#endif
#ifdef USE_PIXFC
#include "pixfc-sse.h"
#endif

namespace plaYUVer
{

std::vector<std::string> PlaYUVerFrame::supportedColorSpacesListNames()
{
  std::vector<std::string> colorSpaceList;
  colorSpaceList.push_back( "YUV" );
  colorSpaceList.push_back( "RGB" );
  colorSpaceList.push_back( "GRAY" );
  return colorSpaceList;
}

std::vector<std::string> PlaYUVerFrame::supportedPixelFormatListNames()
{
  std::vector<std::string> formatsList;
  Int numberFormats = PLAYUVER_NUMBER_FORMATS;
  for( Int i = 0; i < numberFormats; i++ )
  {
    formatsList.push_back( g_PlaYUVerPixFmtDescriptorsList[i].name );
  }
  return formatsList;
}

std::vector<std::string> PlaYUVerFrame::supportedPixelFormatListNames( Int colorSpace )
{
  std::vector<std::string> formatsList;
  Int numberFormats = PLAYUVER_NUMBER_FORMATS;
  for( Int i = 0; i < numberFormats; i++ )
  {
    if( g_PlaYUVerPixFmtDescriptorsList[i].colorSpace == colorSpace )
      formatsList.push_back( g_PlaYUVerPixFmtDescriptorsList[i].name );
  }
  return formatsList;
}

PlaYUVerFrame::Pixel PlaYUVerFrame::ConvertPixel( Pixel inputPixel, ColorSpace eOutputSpace )
{
  Int outA, outB, outC;
  PlaYUVerFrame::Pixel outPixel( COLOR_INVALID, 0, 0, 0 );

  if( inputPixel.ColorSpace() == eOutputSpace || eOutputSpace == COLOR_ARGB || eOutputSpace == COLOR_GRAY )
    return inputPixel;

  if( eOutputSpace == COLOR_RGB )
  {
    YUV2RGB( inputPixel.Y(), inputPixel.Cb(), inputPixel.Cr(), outA, outB, outC );
    outPixel.R() = outA;
    outPixel.G() = outB;
    outPixel.B() = outC;
  }
  if( eOutputSpace == COLOR_YUV )
  {
    rgbToYuv( inputPixel.R(), inputPixel.G(), inputPixel.B(), outA, outB, outC );
    outPixel.Y() = outA;
    outPixel.Cb() = outB;
    outPixel.Cr() = outC;
  }
  return outPixel;
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

PlaYUVerFrame::PlaYUVerFrame( PlaYUVerFrame *other, UInt posX, UInt posY, UInt areaWidth, UInt areaHeight )
{
  PlaYUVerPixFmtDescriptor* pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsList[other->getPelFormat()] );
  if( pcPelFormat->log2ChromaWidth )
  {
    if( posX % ( 1 << pcPelFormat->log2ChromaWidth ) )
      posX--;
    if( ( posX + areaWidth ) % ( 1 << pcPelFormat->log2ChromaWidth ) )
      areaWidth++;
  }

  if( pcPelFormat->log2ChromaHeight )
  {
    if( posY % ( 1 << pcPelFormat->log2ChromaHeight ) )
      posY--;

    if( ( posY + areaHeight ) % ( 1 << pcPelFormat->log2ChromaHeight ) )
      areaHeight++;
  }

  init( areaWidth, areaHeight, other->getPelFormat() );
  copyFrom( other, posX, posY );
}

PlaYUVerFrame::~PlaYUVerFrame()
{
  if( m_pppcInputPel )
    freeMem3ImageComponents<Pel>( m_pppcInputPel );

  if( m_pcARGB32 )
    freeMem1D( m_pcARGB32 );

  closePixfc();
}

static Int getMem3ImageComponents( Pel**** array3D, Int dim1, Int dim2, Int log2ChromaHeight, Int log2ChromaWidth )
{
  Int dim0 = 3;
  Int i;
  UInt64 total_mem_size = 0;
  UInt64 mem_size = ( dim1 * dim2 + CHROMASHIFT( dim1, log2ChromaHeight ) * CHROMASHIFT( dim2, log2ChromaWidth ) * 2 );

  total_mem_size += getMem2D<Pel*>( array3D, dim0, dim1 );

  if( ( ( *array3D )[0][0] = ( Pel* )xCallocMem( mem_size, sizeof(Pel) ) ) == NULL )
    printf( "getMem3DImageComponents: array1D" );

  total_mem_size += mem_size * sizeof(Pel);

  // Luma
  for( i = 1; i < dim1; i++ )
    ( *array3D )[0][i] = ( *array3D )[0][i - 1] + dim2;

  // Chroma
  ( *array3D )[1][0] = ( *array3D )[0][0] + dim2 * dim1;

  dim1 = CHROMASHIFT( dim1, log2ChromaHeight );
  dim2 = CHROMASHIFT( dim2, log2ChromaWidth );

  for( i = 1; i < dim1; i++ )
    ( *array3D )[1][i] = ( *array3D )[1][i - 1] + dim2;

  ( *array3D )[2][0] = ( *array3D )[1][0] + dim2 * dim1;
  for( i = 1; i < dim1; i++ )
    ( *array3D )[2][i] = ( *array3D )[2][i - 1] + dim2;

  return total_mem_size;
}

Void PlaYUVerFrame::init( UInt width, UInt height, Int pel_format )
{
  m_pppcInputPel = NULL;
  m_pcARGB32 = NULL;
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

  m_pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsList[pel_format] );

  m_bHasRGBPel = false;
  if( m_pcPelFormat->colorSpace == COLOR_GRAY )
  {
    getMem3ImageComponents( &m_pppcInputPel, m_uiHeight, m_uiWidth, 1, 1 );
  }
  else
  {
    getMem3ImageComponents( &m_pppcInputPel, m_uiHeight, m_uiWidth, m_pcPelFormat->log2ChromaHeight, m_pcPelFormat->log2ChromaWidth );
  }
  getMem1D( &m_pcARGB32, m_uiHeight * m_uiWidth * 4 );

  xSetupStatistics( this );
  openPixfc();
}

Int PlaYUVerFrame::getColorSpace() const
{
  return m_pcPelFormat->colorSpace;
}

UInt PlaYUVerFrame::getNumberChannels() const
{
  return m_pcPelFormat->numberChannels;
}

UInt64 PlaYUVerFrame::getBytesPerFrame()
{
  return getBytesPerFrame( m_uiWidth, m_uiHeight, m_iPixelFormat );
}

UInt64 PlaYUVerFrame::getBytesPerFrame( UInt uiWidth, UInt uiHeight, Int iPixelFormat )
{
  PlaYUVerPixFmtDescriptor* pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsList[iPixelFormat] );
  UInt64 numberBytes = uiWidth * uiHeight;
  if( pcPelFormat->numberChannels > 1 )
  {
    UInt64 numberBytesChroma = CHROMASHIFT( uiWidth, pcPelFormat->log2ChromaWidth ) * CHROMASHIFT( uiHeight, pcPelFormat->log2ChromaHeight );
    numberBytes += ( pcPelFormat->numberChannels - 1 ) * numberBytesChroma;
  }
  return numberBytes;
}

UInt PlaYUVerFrame::getChromaWidth() const
{
  return CHROMASHIFT( m_uiWidth, m_pcPelFormat->log2ChromaWidth );
}

UInt PlaYUVerFrame::getChromaHeight() const
{
  return CHROMASHIFT( m_uiHeight, m_pcPelFormat->log2ChromaHeight );
}

UInt PlaYUVerFrame::getChromaLength() const
{
  return getChromaWidth() * getChromaHeight();
}

Void PlaYUVerFrame::frameFromBuffer( Pel *Buff, UInt64 uiBuffSize )
{
  if( uiBuffSize != getBytesPerFrame() )
    return;

  Pel* ppBuff[MAX_NUMBER_PLANES];
  Pel* pTmpPel, *pTmpBuff;
  Int ratioH, ratioW, step;
  UInt i, ch;

  ppBuff[0] = Buff;
  for( Int i = 1; i < MAX_NUMBER_PLANES; i++ )
  {
    ratioW = i > 1 ? m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = i > 1 ? m_pcPelFormat->log2ChromaHeight : 0;
    ppBuff[i] = ppBuff[i - 1] + CHROMASHIFT( m_uiHeight, ratioH ) * CHROMASHIFT( m_uiWidth, ratioW );
  }

  for( ch = 0; ch < m_pcPelFormat->numberChannels; ch++ )
  {
    ratioW = ch > 0 ? m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = ch > 0 ? m_pcPelFormat->log2ChromaHeight : 0;
    step = m_pcPelFormat->comp[ch].step_minus1 + 1;

    pTmpPel = m_pppcInputPel[ch][0];
    pTmpBuff = ppBuff[m_pcPelFormat->comp[ch].plane] + ( m_pcPelFormat->comp[ch].offset_plus1 - 1 );

    for( i = 0; i < CHROMASHIFT( m_uiHeight, ratioH ) * CHROMASHIFT( m_uiWidth, ratioW ); i++ )
    {
      *pTmpPel++ = *pTmpBuff;
      pTmpBuff += step;
    }
  }

  m_bHasRGBPel = false;
  m_bHasHistogram = false;
}

Void PlaYUVerFrame::frameToBuffer( Pel *output_buffer )
{
  Pel* ppBuff[MAX_NUMBER_PLANES];
  Pel* pTmpPel, *pTmpBuff;
  Int ratioH, ratioW, step;
  UInt i, ch;

  ppBuff[0] = output_buffer;
  for( Int i = 1; i < MAX_NUMBER_PLANES; i++ )
  {
    ratioW = i > 1 ? m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = i > 1 ? m_pcPelFormat->log2ChromaHeight : 0;
    ppBuff[i] = ppBuff[i - 1] + CHROMASHIFT( m_uiHeight, ratioH ) * CHROMASHIFT( m_uiWidth, ratioW );
  }

  for( ch = 0; ch < m_pcPelFormat->numberChannels; ch++ )
  {
    ratioW = ch > 0 ? m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = ch > 0 ? m_pcPelFormat->log2ChromaHeight : 0;
    step = m_pcPelFormat->comp[ch].step_minus1 + 1;

    pTmpPel = m_pppcInputPel[ch][0];
    pTmpBuff = ppBuff[m_pcPelFormat->comp[ch].plane];

    for( i = 0; i < CHROMASHIFT( m_uiHeight, ratioH ) * CHROMASHIFT( m_uiWidth, ratioW ); i++ )
    {
      *pTmpBuff = *pTmpPel++;
      pTmpBuff += step;
    }
  }

}

Void PlaYUVerFrame::fillRGBBuffer()
{
  if( m_bHasRGBPel )
    return;
  if( m_pcPelFormat->colorSpace == COLOR_RGB )
  {
    Pel* pR = m_pppcInputPel[COLOR_R][0];
    Pel* pG = m_pppcInputPel[COLOR_G][0];
    Pel* pB = m_pppcInputPel[COLOR_B][0];
    Int iR, iG, iB;
    UInt* pARGB = ( UInt* )m_pcARGB32;
    for( UInt i = 0; i < m_uiHeight * m_uiWidth; i++ )
    {
      iR = *pR++;
      iG = *pG++;
      iB = *pB++;
      *pARGB++ = PEL_RGB( iR, iG, iB );
    }
  }
  else
  {
    m_pcPelFormat->fillARGB32buffer( m_pppcInputPel, m_pcARGB32, m_uiWidth, m_uiHeight );
  }
  m_bHasRGBPel = true;
}

Void PlaYUVerFrame::copyFrom( PlaYUVerFrame* input_frame )
{
  if( m_iPixelFormat != input_frame->getPelFormat() && m_uiWidth == input_frame->getWidth() && m_uiHeight == input_frame->getHeight() )
    return;
  m_bHasRGBPel = false;
  m_bHasHistogram = false;
  memcpy( *m_pppcInputPel[LUMA], input_frame->getPelBufferYUV()[LUMA][0], getBytesPerFrame() * sizeof(Pel) );
}

Void PlaYUVerFrame::copyFrom( PlaYUVerFrame* input_frame, UInt xPos, UInt yPos )
{
  if( m_iPixelFormat != input_frame->getPelFormat() )
    return;
  Pel ***pInput = input_frame->getPelBufferYUV();
  for( UInt ch = 0; ch < m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? m_pcPelFormat->log2ChromaWidth : 0;
    Int ratioW = ch > 0 ? m_pcPelFormat->log2ChromaHeight : 0;
    for( UInt i = 0; i < CHROMASHIFT( m_uiHeight, ratioH ); i++ )
    {
      memcpy( m_pppcInputPel[ch][i], &( pInput[ch][( yPos >> ratioH ) + i][( xPos >> ratioW )] ), ( m_uiWidth >> ratioW ) * sizeof(Pel) );
    }
  }
  m_bHasRGBPel = false;
  m_bHasHistogram = false;
}

PlaYUVerFrame::Pixel PlaYUVerFrame::getPixelValue( Int xPos, Int yPos, ColorSpace eColorSpace )
{
  PlaYUVerFrame::Pixel PixelValue( m_pcPelFormat->colorSpace, 0, 0, 0 );
  for( UInt ch = 0; ch < m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? m_pcPelFormat->log2ChromaWidth : 0;
    Int ratioW = ch > 0 ? m_pcPelFormat->log2ChromaHeight : 0;
    PixelValue.Components()[ch] = m_pppcInputPel[ch][( yPos >> ratioH )][( xPos >> ratioW )];
  }
  if( eColorSpace != COLOR_INVALID )
  {
    PixelValue = ConvertPixel( PixelValue, eColorSpace );
  }
  return PixelValue;
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

Void PlaYUVerFrame::getCvMat( Void** ppCvFrame )
{
#ifdef USE_OPENCV
  Int cvType = CV_8UC3;
  switch( m_pcPelFormat->numberChannels )
  {
  case 3:
    cvType = CV_8UC4;
    break;
  case 1:
    cvType = CV_8UC1;
    break;
  default:
    break;
  }
  cv::Mat *pcCvFrame = new cv::Mat( m_uiHeight, m_uiWidth, cvType );
  if( m_pcPelFormat->colorSpace != PlaYUVerFrame::COLOR_GRAY )
  {
    fillRGBBuffer();
    memcpy( pcCvFrame->data, m_pcARGB32, m_uiWidth * m_uiHeight * 4 * sizeof(UChar) );
  }
  else
  {
    memcpy( pcCvFrame->data, &( m_pppcInputPel[LUMA][0][0] ), m_uiWidth * m_uiHeight * sizeof(Pel) );
  }
  *ppCvFrame = pcCvFrame;
#endif
}

Void PlaYUVerFrame::fromCvMat( Void* voidFrame )
{
#ifdef USE_OPENCV
  cv::Mat* opencvFrame = ( cv::Mat* )voidFrame;
  if( m_iPixelFormat == NO_FMT )
  {
    switch( opencvFrame->channels() )
    {
    case 1:
      m_iPixelFormat = GRAY;
      break;
    case 4:
      m_iPixelFormat = RGB24;
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
  m_bHasHistogram = false;

  if( m_iPixelFormat != GRAY )
  {
    Pel* pInputPelY = m_pppcInputPel[LUMA][0];
    Pel* pInputPelU = m_pppcInputPel[CHROMA_U][0];
    Pel* pInputPelV = m_pppcInputPel[CHROMA_V][0];
    Pel* pcRGBPelInterlaced = m_pcARGB32;
    memcpy( pcRGBPelInterlaced, opencvFrame->data, m_uiWidth * m_uiHeight * 4 * sizeof(Pel) );
    UInt* buff = ( UInt* )pcRGBPelInterlaced;
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
#endif
}

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
