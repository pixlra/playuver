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

PlaYUVerFrame::PlaYUVerFrame( UInt width, UInt height, Int pelFormat, Int bitsPixel )
{
  init( width, height, pelFormat, bitsPixel );
}

PlaYUVerFrame::PlaYUVerFrame( PlaYUVerFrame *other )
{
  init( other->getWidth(), other->getHeight(), other->getPelFormat(), other->getBitsPel() );
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

  init( areaWidth, areaHeight, other->getPelFormat(), other->getBitsPel() );
  copyFrom( other, posX, posY );
}

PlaYUVerFrame::~PlaYUVerFrame()
{
  if( m_puiHistogram )
  {
    freeMem1D( m_puiHistogram );
  }
  m_puiHistogram = NULL;
  m_bHasHistogram = false;

  if( m_pppcInputPel )
    freeMem3ImageComponents<Pel>( m_pppcInputPel );
  m_pppcInputPel = NULL;

  if( m_pcARGB32 )
    freeMem1D( m_pcARGB32 );
  m_pcARGB32 = NULL;

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

Void PlaYUVerFrame::init( UInt width, UInt height, Int pel_format, Int bitsPixel )
{
  m_pppcInputPel = NULL;
  m_pcARGB32 = NULL;
  m_uiWidth = width;
  m_uiHeight = height;
  m_iPixelFormat = pel_format;
  m_iNumberChannels = 3;
  m_iBitsPel = bitsPixel > 8 ? bitsPixel : 8;

  if( m_uiWidth == 0 || m_uiHeight == 0 || m_iPixelFormat == -1 || bitsPixel > 16 )
  {
    throw "Cannot create a PlYUVerFrame of this type";
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

  m_puiHistogram = NULL;
  m_bHasHistogram = false;

  m_uiHistoSegments = 1 << m_iBitsPel;

  if( m_pcPelFormat->colorSpace == PlaYUVerFrame::COLOR_RGB || m_pcPelFormat->colorSpace == PlaYUVerFrame::COLOR_ARGB )
    m_uiHistoChannels = m_pcPelFormat->numberChannels + 1;
  else
    m_uiHistoChannels = m_pcPelFormat->numberChannels;

  m_cPelFmtName = PlaYUVerFrame::supportedPixelFormatListNames()[m_iPixelFormat].c_str();
}

Void PlaYUVerFrame::clear()
{
  Pel* pPel;
  Pel uiValue = 128;
  Int ratioH, ratioW;
  UInt i;
  for( UInt ch = 0; ch < m_pcPelFormat->numberChannels; ch++ )
  {
    ratioW = ch > 0 ? m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = ch > 0 ? m_pcPelFormat->log2ChromaHeight : 0;
    pPel = m_pppcInputPel[ch][0];
    for( i = 0; i < CHROMASHIFT( m_uiHeight, ratioH ) * CHROMASHIFT( m_uiWidth, ratioW ); i++ )
    {
      *pPel++ = uiValue;
    }
  }
}

std::string PlaYUVerFrame::getPelFmtName()
{
  return m_pcPelFormat->name;
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
  return getBytesPerFrame( m_uiWidth, m_uiHeight, m_iPixelFormat, m_iBitsPel );
}

UInt64 PlaYUVerFrame::getBytesPerFrame( UInt uiWidth, UInt uiHeight, Int iPixelFormat, Int bitsPixel )
{
  PlaYUVerPixFmtDescriptor* pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsList[iPixelFormat] );
  UInt bytesPerPixel = ( bitsPixel - 1 ) / 8 + 1;
  UInt64 numberBytes = uiWidth * uiHeight;
  if( pcPelFormat->numberChannels > 1 )
  {
    UInt64 numberBytesChroma = CHROMASHIFT( uiWidth, pcPelFormat->log2ChromaWidth ) * CHROMASHIFT( uiHeight, pcPelFormat->log2ChromaHeight );
    numberBytes += ( pcPelFormat->numberChannels - 1 ) * numberBytesChroma;
  }
  return numberBytes * bytesPerPixel;
}

UInt PlaYUVerFrame::getPixels() const
{
  return ( m_uiWidth * m_uiHeight );
}

UInt8 PlaYUVerFrame::getChromaWidthRatio() const
{
  return m_pcPelFormat->log2ChromaWidth;
}

UInt8 PlaYUVerFrame::getChromaHeightRatio() const
{
  return m_pcPelFormat->log2ChromaHeight;
}

UInt PlaYUVerFrame::getChromaWidth() const
{
  if( m_pcPelFormat->colorSpace == GRAY )
    return 0;
  return CHROMASHIFT( m_uiWidth, m_pcPelFormat->log2ChromaWidth );
}

UInt PlaYUVerFrame::getChromaHeight() const
{
  if( m_pcPelFormat->colorSpace == GRAY )
    return 0;
  return CHROMASHIFT( m_uiHeight, m_pcPelFormat->log2ChromaHeight );
}

UInt PlaYUVerFrame::getChromaLength() const
{
  return getChromaWidth() * getChromaHeight();
}

UInt PlaYUVerFrame::getChromaSize() const
{
  return getChromaWidth() * getChromaHeight();
}

Void PlaYUVerFrame::frameFromBuffer( Byte *Buff, UInt64 uiBuffSize )
{
  if( uiBuffSize != getBytesPerFrame() )
    return;

  Byte* ppBuff[MAX_NUMBER_PLANES];
  Byte* pTmpBuff;
  Pel* pTmpPel;
  UInt bytesPixel = ( m_iBitsPel - 1 ) / 8 + 1;
  Int ratioH, ratioW, step;
  UInt i, ch, b;

  ppBuff[0] = Buff;
  for( i = 1; i < MAX_NUMBER_PLANES; i++ )
  {
    ratioW = i > 1 ? m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = i > 1 ? m_pcPelFormat->log2ChromaHeight : 0;
    ppBuff[i] = ppBuff[i - 1] + CHROMASHIFT( m_uiHeight, ratioH ) * CHROMASHIFT( m_uiWidth, ratioW ) * bytesPixel;
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
      *pTmpPel = *pTmpBuff;
      for( b = 1; b < bytesPixel; b++ )
      {
        pTmpBuff++;
        *pTmpPel |= ( *pTmpBuff << ( 8 * b ) );
      }
      pTmpPel++;
      pTmpBuff += step;
    }
  }
  m_bHasRGBPel = false;
  m_bHasHistogram = false;
}

Void PlaYUVerFrame::frameToBuffer( Byte *output_buffer )
{
  UInt bytesPixel = ( m_iBitsPel - 1 ) / 8 + 1;
  Byte* ppBuff[MAX_NUMBER_PLANES];
  Byte* pTmpBuff;
  Pel* pTmpPel;
  Int ratioH, ratioW, step;
  UInt i, ch, b;

  ppBuff[0] = output_buffer;
  for( Int i = 1; i < MAX_NUMBER_PLANES; i++ )
  {
    ratioW = i > 1 ? m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = i > 1 ? m_pcPelFormat->log2ChromaHeight : 0;
    ppBuff[i] = ppBuff[i - 1] + CHROMASHIFT( m_uiHeight, ratioH ) * CHROMASHIFT( m_uiWidth, ratioW ) * bytesPixel;
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
      *pTmpBuff = ( *pTmpPel & 0x00FF );
      for( b = 1; b < bytesPixel; b++ )
      {
        pTmpBuff++;
        *pTmpBuff = ( *pTmpPel >> ( 8 * b ) ) & 0x00FF;
      }
      pTmpPel++;
      pTmpBuff += step;
    }
  }

}

Void PlaYUVerFrame::fillRGBBuffer()
{
  if( m_bHasRGBPel )
    return;
  Int iR, iG, iB;
  Int shiftBits = m_iBitsPel - 8;
  UInt* pARGB = ( UInt* )m_pcARGB32;
  if( m_pcPelFormat->colorSpace == COLOR_GRAY )
  {
    Pel* pY = m_pppcInputPel[LUMA][0];
    Int iY;
    for( UInt i = 0; i < m_uiHeight * m_uiWidth; i++ )
    {
      iY = *pY++;
      iY >>= shiftBits;
      iR = iY;
      iG = iY;
      iB = iY;
      *pARGB++ = PEL_RGB( iR, iG, iB );
    }
  }
  else if( m_pcPelFormat->colorSpace == COLOR_RGB )
  {
    Pel* pR = m_pppcInputPel[COLOR_R][0];
    Pel* pG = m_pppcInputPel[COLOR_G][0];
    Pel* pB = m_pppcInputPel[COLOR_B][0];

    for( UInt i = 0; i < m_uiHeight * m_uiWidth; i++ )
    {
      iR = *pR++;
      iG = *pG++;
      iB = *pB++;
      *pARGB++ = PEL_RGB( ( iR >> shiftBits ), ( iG >> shiftBits ), ( iB >> shiftBits ) );
    }
  }
  else if( m_pcPelFormat->colorSpace == COLOR_YUV )
  {
    Pel* pLineY = m_pppcInputPel[LUMA][0];
    Pel* pLineU = m_pppcInputPel[CHROMA_U][0];
    Pel* pLineV = m_pppcInputPel[CHROMA_V][0];
    UInt uiChromaStride = CHROMASHIFT( m_uiWidth, m_pcPelFormat->log2ChromaWidth );
    Pel* pY;
    Pel* pU;
    Pel* pV;
    Int iY, iU, iV, iR, iG, iB;
    UInt* pARGBLine = pARGB;
    UInt* pARGB;

    UInt y, x;
    Int i, j;
    for( y = 0; y < m_uiHeight >> m_pcPelFormat->log2ChromaHeight; y++ )
    {
      for( i = 0; i < 1 << m_pcPelFormat->log2ChromaHeight; i++ )
      {
        pY = pLineY;
        pU = pLineU;
        pV = pLineV;
        pARGB = pARGBLine;
        for( x = 0; x < m_uiWidth >> m_pcPelFormat->log2ChromaWidth; x++ )
        {
          iU = *pU++;
          iU >>= shiftBits;
          iV = *pV++;
          iV >>= shiftBits;
          for( j = 0; j < ( 1 << m_pcPelFormat->log2ChromaWidth ); j++ )
          {
            iY = *pY++;
            iY >>= shiftBits;
            YUV2RGB( iY, iU, iV, iR, iG, iB );
            *pARGB++ = PEL_RGB( iR, iG, iB );
          }
        }
        pLineY += m_uiWidth;
        pARGBLine += m_uiWidth;
      }
      pLineU += uiChromaStride;
      pLineV += uiChromaStride;
    }
  }
  m_bHasRGBPel = true;
}

// statistics
Void PlaYUVerFrame::calcHistogram()
{
  if( m_bHasHistogram )
    return;

  if( !m_puiHistogram )
  {
    //m_puiHistogram = new UInt[m_uiHistoSegments * histoChannels];
    getMem1D<UInt>( &( m_puiHistogram ), m_uiHistoSegments * m_uiHistoChannels );
    if( !m_puiHistogram )
    {
      return;
    }
  }
  m_bRunningFlag = true;

  UInt i, j;
  UInt numberChannels = m_pcPelFormat->numberChannels;
  Int colorSpace = m_pcPelFormat->colorSpace;
  if( colorSpace == PlaYUVerFrame::COLOR_YUV )
  {
    const Pel *data[3];
    data[LUMA] = m_pppcInputPel[LUMA][0];
    data[CHROMA_U] = m_pppcInputPel[CHROMA_U][0];
    data[CHROMA_V] = m_pppcInputPel[CHROMA_V][0];
    for( i = 0; i < m_uiHeight * m_uiWidth && m_bRunningFlag; i++ )
    {
      m_puiHistogram[*( data[LUMA] ) + LUMA * m_uiHistoSegments]++;
      data[LUMA] += 1;
    }
    for( i = 0; ( i < getChromaSize() ) && m_bRunningFlag; i++ )
    {
      for( j = 1; j < numberChannels; j++ )
      {
        m_puiHistogram[*( data[j] ) + j * m_uiHistoSegments]++;
        data[j] += 1;
      }
    }
  }
  else
  {
    const Pel* data[3];
    Pel luma;
    data[COLOR_R] = m_pppcInputPel[COLOR_R][0];
    data[COLOR_G] = m_pppcInputPel[COLOR_G][0];
    data[COLOR_B] = m_pppcInputPel[COLOR_B][0];

    for( i = 0; ( i < m_uiHeight * m_uiWidth ) && m_bRunningFlag; i++ )
    {
      for( j = 0; j < numberChannels; j++ )
      {
        m_puiHistogram[*( data[j] ) + j * m_uiHistoSegments]++;
      }
      if( ( colorSpace == PlaYUVerFrame::COLOR_RGB || colorSpace == PlaYUVerFrame::COLOR_ARGB ) )
      {
        PlaYUVerFrame::Pixel currPixel( colorSpace, *( data[COLOR_R] ), *( data[COLOR_G] ), *( data[COLOR_B] ) );
        luma = PlaYUVerFrame::ConvertPixel( currPixel, PlaYUVerFrame::COLOR_YUV ).Y();
        m_puiHistogram[luma + j * m_uiHistoSegments]++;
      }
      for( j = 0; j < numberChannels; j++ )
        data[j] += 1;
    }
  }
  m_bHasHistogram = true;
  m_bRunningFlag = false;
}

Int PlaYUVerFrame::getHistogramSegment()
{
  return m_uiHistoSegments;
}

Int PlaYUVerFrame::getRealHistoChannel( Int channel )
{
  Int histoChannel;

  switch( getColorSpace() )
  {
  case PlaYUVerFrame::COLOR_GRAY:
    if( channel != LUMA )
    {
      return -1;
    }
    histoChannel = 0;
    break;
  case PlaYUVerFrame::COLOR_RGB:
    if( channel != COLOR_R && channel != COLOR_G && channel != COLOR_B && channel != LUMA )
    {
      return -1;
    }
    else
    {
      if( channel == LUMA )
      {
        histoChannel = 3;
      }
      else
      {
        histoChannel = channel;
      }
    }
    break;
  case PlaYUVerFrame::COLOR_ARGB:
    if( channel != COLOR_R && channel != COLOR_G && channel != COLOR_B && channel != COLOR_A && channel != LUMA )
    {
      return -1;
    }
    else
    {
      if( channel == LUMA )
      {
        histoChannel = 4;
      }
      else
      {
        histoChannel = channel;
      }
    }
    break;
  case PlaYUVerFrame::COLOR_YUV:
    if( channel != LUMA && channel != CHROMA_U && channel != CHROMA_V )
    {
      return -1;
    }
    else
    {
      histoChannel = channel;
    }
    break;
  default:
    histoChannel = -1;
  }
  return histoChannel;
}

Double PlaYUVerFrame::getCount( Int channel, UInt start, UInt end )
{
  UInt i;
  Int histoChannel;
  Int indexStart;
  Double count = 0.0;

  if( !m_bHasHistogram || start < 0 || end > m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * m_uiHistoSegments;

  for( i = start; i <= end; i++ )
  {
    count += m_puiHistogram[indexStart + i];
  }
  return count;
}

Double PlaYUVerFrame::getMean( Int channel, UInt start, UInt end )
{
  Int indexStart;
  Int histoChannel;
  Double mean = 0.0;
  Double count;

  if( !m_bHasHistogram || start < 0 || end > m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * m_uiHistoSegments;

  for( UInt i = start; i <= end; i++ )
  {
    mean += i * m_puiHistogram[indexStart + i];
  }

  count = getCount( channel, start, end );

  if( count > 0.0 )
  {
    return mean / count;
  }

  return mean;
}

Int PlaYUVerFrame::getMedian( Int channel, UInt start, UInt end )
{
  Int histoChannel;
  Int indexStart;
  Double sum = 0.0;
  Double count;

  if( !m_bHasHistogram || start < 0 || end > m_uiHistoSegments - 1 || start > end )
  {
    return 0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0;
  }

  indexStart = histoChannel * m_uiHistoSegments;

  count = getCount( channel, start, end );

  for( UInt i = start; i <= end; i++ )
  {
    sum += m_puiHistogram[indexStart + i];
    if( sum * 2 > count )
      return i;
  }

  return 0;
}

Double PlaYUVerFrame::getStdDev( Int channel, UInt start, UInt end )
{
  Int histoChannel;
  Int indexStart;
  Double dev = 0.0;
  Double count;
  Double mean;

  if( !m_bHasHistogram || start < 0 || end > m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * m_uiHistoSegments;
  mean = getMean( channel, start, end );
  count = getCount( channel, start, end );

  if( count == 0.0 )
    count = 1.0;

  /*------------ original

   for ( i = start ; i <= end ; i++ )
   {
   dev += ( i - mean ) * ( i - mean ) * m_puiHistogram[indexStart +i];
   }

   return sqrt( dev / count );

   -----------------------*/

  for( UInt i = start; i <= end; i++ )
  {
    dev += ( i * i ) * m_puiHistogram[indexStart + i];
  }

  return sqrt( ( dev - count * mean * mean ) / ( count - 1 ) );

}

Double PlaYUVerFrame::getHistogramValue( Int channel, UInt bin )
{
  Double value;
  Int histoChannel;
  Int indexStart;

  if( !m_bHasHistogram || bin < 0 || bin > m_uiHistoSegments - 1 )
    return 0.0;

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * m_uiHistoSegments;

  value = m_puiHistogram[indexStart + bin];

  return value;
}

Double PlaYUVerFrame::getMaximum( Int channel )
{
  Double max = 0.0;

  Int histoChannel;
  Int indexStart;

  if( !m_bHasHistogram )
    return 0.0;

  histoChannel = getRealHistoChannel( channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * m_uiHistoSegments;

  for( UInt x = 0; x < m_uiHistoSegments; x++ )
  {
    if( m_puiHistogram[indexStart + x] > max )
    {
      max = m_puiHistogram[indexStart + x];
    }
  }

  return max;
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

cv::Mat* PlaYUVerFrame::getCvMat( Bool convertToGray )
{
#ifdef USE_OPENCV
  if( convertToGray && !( m_pcPelFormat->colorSpace == PlaYUVerFrame::COLOR_YUV || m_pcPelFormat->colorSpace == PlaYUVerFrame::COLOR_GRAY ) )
  {
    return NULL;
  }
  Int cvType = CV_MAKETYPE( CV_8U, convertToGray ? 1 : m_pcPelFormat->numberChannels );
  cv::Mat *pcCvFrame = new cv::Mat( m_uiHeight, m_uiWidth, cvType );
  UChar* pCvPel = pcCvFrame->data;
  if( !convertToGray && ( m_pcPelFormat->colorSpace == PlaYUVerFrame::COLOR_YUV || m_pcPelFormat->colorSpace == PlaYUVerFrame::COLOR_RGB ) )
  {
    fillRGBBuffer();
    UChar* pARGB = m_pcARGB32;
    for( UInt i = 0; i < m_uiWidth * m_uiHeight; i++ )
    {
      *pCvPel++ = *pARGB++;
      *pCvPel++ = *pARGB++;
      *pCvPel++ = *pARGB++;
      pARGB++;
    }
  }
  else if( convertToGray || m_pcPelFormat->colorSpace == PlaYUVerFrame::COLOR_GRAY )
  {
    Pel* pPel = m_pppcInputPel[LUMA][0];
    for( UInt i = 0; i < m_uiWidth * m_uiHeight; i++ )
    {
      *pCvPel++ = *pPel++;
    }
  }
  else
  {
    if( pcCvFrame )
      delete pcCvFrame;
    pcCvFrame = NULL;
  }
  return pcCvFrame;
#else
  return NULL;
#endif
}

Void PlaYUVerFrame::fromCvMat( cv::Mat* pcCvFrame )
{
#ifdef USE_OPENCV
  if( m_iPixelFormat == NO_FMT )
  {
    switch( pcCvFrame->channels() )
    {
    case 1:
      m_iPixelFormat = GRAY;
      break;
    case 3:
      m_iPixelFormat = RGB24;
      break;
    default:
      return;
    }
  }

  if( !isValid() )
  {
    init( pcCvFrame->cols, pcCvFrame->rows, m_iPixelFormat, 8 );
  }

  m_bHasRGBPel = false;
  m_bHasHistogram = false;

  UChar* pCvPel = pcCvFrame->data;
  if( m_iPixelFormat != GRAY )
  {
    Pel* pInputPelR = m_pppcInputPel[COLOR_R][0];
    Pel* pInputPelG = m_pppcInputPel[COLOR_G][0];
    Pel* pInputPelB = m_pppcInputPel[COLOR_B][0];
    for( UInt i = 0; i < m_uiHeight * m_uiWidth; i++ )
    {
      *pInputPelR++ = pelRed( *pCvPel++ );
      *pInputPelG++ = pelGreen( *pCvPel++ );
      *pInputPelB++ = pelBlue( *pCvPel++ );
    }
  }
  else
  {
    Pel* pPel = m_pppcInputPel[LUMA][0];
    for( UInt i = 0; i < m_uiWidth * m_uiHeight; i++ )
    {
      *pPel++ = *pCvPel++;
    }
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
