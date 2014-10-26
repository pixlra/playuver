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
#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#endif
#ifdef USE_PIXFC
#include "pixfc-sse.h"
#endif

namespace plaYUVer
{

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
  adjustSelectedAreaDims( area , other->getPelFormat() );
  init( area.width(), area.height(), other->getPelFormat() );
  copyFrom( other, area.x(), area.y() );
}

PlaYUVerFrame::~PlaYUVerFrame()
{
  if( m_pppcInputPel )
    freeMem3ImageComponents<Pel>( m_pppcInputPel );

  if( m_pcRGBPelInterlaced )
    freeMem1D<UChar>( m_pcRGBPelInterlaced );

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
    if( posX%2 )
      area.setX(posX-1);
    if( posY%2 )
      area.setY(posY-1);

    if( (posX+width)%2 )
      area.setWidth((posX+width)-area.x()+1);
    else
      area.setWidth((posX+width)-area.x());

    if( (posY+height)%2 )
      area.setHeight((posY+height)-area.y()+1);
    else
      area.setHeight((posY+height)-area.y());

    break;
  case YUV444p:
    break;
  case YUV422p:
  case YUYV422:
    if( posX%2 )
      area.setX(posX-1);

    if( (posX+width)%2 )
      area.setWidth((posX+width)-area.x()+1);
    else
      area.setWidth((posX+width)-area.x());

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
  m_pcRGBPelInterlaced = NULL;
  m_uiWidth = width;
  m_uiHeight = height;
  m_iPixelFormat = pel_format;
  m_iNumberChannels = 3;
  Int max_bits_per_pel = sizeof(Pel) * 8;
  m_iBitsChannels = 8;

  if( m_uiWidth == 0 || m_uiHeight == 0 || m_iPixelFormat == -1 || m_iBitsChannels > max_bits_per_pel )
  {
    return;
  }

  m_bHasRGBPel = false;

  switch( m_iPixelFormat )
  {
  case YUV420p:
    getMem3ImageComponents<Pel>( &m_pppcInputPel, m_uiHeight, m_uiWidth, 2, 2 );
    break;
  case YUV444p:
    getMem3ImageComponents<Pel>( &m_pppcInputPel, m_uiHeight, m_uiWidth, 1, 1 );
    break;
  case YUV422p:
  case YUYV422:
    getMem3ImageComponents<Pel>( &m_pppcInputPel, m_uiHeight, m_uiWidth, 1, 2 );
    break;
  case GRAY:
    getMem3ImageComponents<Pel>( &m_pppcInputPel, m_uiHeight, m_uiWidth, 4, 4 );
    break;
  case RGB8:
    getMem3ImageComponents<Pel>( &m_pppcInputPel, m_uiHeight, m_uiWidth, 1, 1 );
    break;
  default:
    m_pppcInputPel = NULL;
    break;
  }
  getMem1D<Pel>( &m_pcRGBPelInterlaced, m_uiHeight * m_uiWidth * 3 );
  openPixfc();
}

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
    m_pcPixfc->convert(m_pcPixfc, m_pppcInputPel, m_pcRGBPelInterlaced);
    break;
    case YUV422p:
    break;
    case YUV444p:
    case YUYV422:
    m_pcPixfc->convert(m_pcPixfc, m_pppcInputPel, m_pcRGBPelInterlaced);
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

template<typename T>
Void yuvToRgb( T iY, T iU, T iV, T &iR, T &iG, T &iB )
{
  iR = iY + 1402 * iV / 1000;
  iG = iY - ( 101004 * iU + 209599 * iV ) / 293500;
  iB = iY + 1772 * iU / 1000;

  if( iR < 0 )
    iR = 0;
  if( iG < 0 )
    iG = 0;
  if( iB < 0 )
    iB = 0;

  if( iR > 255 )
    iR = 255;
  if( iG > 255 )
    iG = 255;
  if( iB > 255 )
    iB = 255;
}

template<typename T>
Void rgbToYuv( T iR, T iG, T iB, T &iY, T &iU, T &iV )
{
  iY = ( 299 * iR + 587 * iG + 114 * iB + 500 ) / 1000;
  iU = ( 1000 * ( iB - iY ) + 226816 ) / 1772;
  iV = ( 1000 * ( iR - iY ) + 179456 ) / 1402;
}

Void PlaYUVerFrame::FrametoRGB8()
{
  if( m_bHasRGBPel )
    return;
#ifdef USE_PIXFC
  if( m_pcPixfc )
  {
    FrametoRGB8Pixfc();
  }
#endif
  {
    Int iY, iU, iV, iR, iG, iB;
    Pel** ppInputPelY = m_pppcInputPel[LUMA];
    Pel* pInputPelY = &( m_pppcInputPel[LUMA][0][0] );

    Pel *pInputPelU = NULL, *pInputPelV = NULL;
    if( m_pppcInputPel[CHROMA_U] )
    {
      pInputPelU = m_pppcInputPel[CHROMA_U][0];
      pInputPelV = m_pppcInputPel[CHROMA_V][0];
    }

    Pel* pcRGBPelInterlaced = m_pcRGBPelInterlaced;
    Pel* pcRGBPelInterlacedl1 = m_pcRGBPelInterlaced + m_uiWidth * 3;

    switch( m_iPixelFormat )
    {
    case YUV420p:
      for( UInt y = 0; y < m_uiHeight; y += 2 )
      {
        for( UInt x = 0; x < m_uiWidth; x += 2 )
        {
          // Pixel (x, y).
          iY = ppInputPelY[y][x];
          iU = *pInputPelU++ - 128;
          iV = *pInputPelV++ - 128;
          yuvToRgb<Int>( iY, iU, iV, iR, iG, iB );
          *pcRGBPelInterlaced++ = iR;
          *pcRGBPelInterlaced++ = iG;
          *pcRGBPelInterlaced++ = iB;
          // Pixel (x+1, y)
          iY = ppInputPelY[y][x + 1];
          yuvToRgb<Int>( iY, iU, iV, iR, iG, iB );
          *pcRGBPelInterlaced++ = iR;
          *pcRGBPelInterlaced++ = iG;
          *pcRGBPelInterlaced++ = iB;
          // Pixel (x, y+1)
          iY = ppInputPelY[y + 1][x];
          yuvToRgb<Int>( iY, iU, iV, iR, iG, iB );
          *pcRGBPelInterlacedl1++ = iR;
          *pcRGBPelInterlacedl1++ = iG;
          *pcRGBPelInterlacedl1++ = iB;
          // Pixel (x+1, y+1)
          iY = ppInputPelY[y + 1][x + 1];
          yuvToRgb<Int>( iY, iU, iV, iR, iG, iB );
          *pcRGBPelInterlacedl1++ = iR;
          *pcRGBPelInterlacedl1++ = iG;
          *pcRGBPelInterlacedl1++ = iB;
        }
        pcRGBPelInterlaced += m_uiWidth * 3;
        pcRGBPelInterlacedl1 += m_uiWidth * 3;
      }
      break;
    case YUV422p:
    case YUYV422:
      for( UInt y = 0; y < m_uiHeight; y += 1 )
        for( UInt x = 0; x < m_uiWidth; x += 2 )
        {
          // Pixel (x, y).
          iY = ppInputPelY[y][x];
          iU = *pInputPelU++ - 128;
          iV = *pInputPelV++ - 128;
          yuvToRgb<Int>( iY, iU, iV, iR, iG, iB );
          *pcRGBPelInterlaced++ = iR;
          *pcRGBPelInterlaced++ = iG;
          *pcRGBPelInterlaced++ = iB;
          // Pixel (x+1, y)
          iY = ppInputPelY[y][x + 1];
          yuvToRgb<Int>( iY, iU, iV, iR, iG, iB );
          *pcRGBPelInterlaced++ = iR;
          *pcRGBPelInterlaced++ = iG;
          *pcRGBPelInterlaced++ = iB;
        }
      break;
    case YUV444p:
      for( UInt y = 0; y < m_uiHeight; y++ )
        for( UInt x = 0; x < m_uiWidth; x++ )
        {
          iY = *pInputPelY++;
          iU = *pInputPelU++ - 128;
          iV = *pInputPelV++ - 128;
          yuvToRgb<Int>( iY, iU, iV, iR, iG, iB );
          *pcRGBPelInterlaced++ = iR;
          *pcRGBPelInterlaced++ = iG;
          *pcRGBPelInterlaced++ = iB;
        }
      break;
    case GRAY:
      for( UInt y = 0; y < m_uiHeight; y++ )
        for( UInt x = 0; x < m_uiWidth; x++ )
        {
          iY = *pInputPelY++;
          //yuvToRgb( iY, iY, iY, iR, iG, iB );
          iR = iG = iB = iY;
          *pcRGBPelInterlaced++ = iR;
          *pcRGBPelInterlaced++ = iG;
          *pcRGBPelInterlaced++ = iB;
        }
      break;
    case RGB8:
      break;
    default:
      // No action.
      break;
    }
    m_bHasRGBPel = true;
  }
}

Void PlaYUVerFrame::FrameFromBuffer( Pel *input_buffer, Int pel_format )
{
  if( m_iPixelFormat != pel_format )
  {
    return;
  }
  Pel* pcRGBPelInterlaced = m_pcRGBPelInterlaced;
  Pel* pInputPelY = m_pppcInputPel[LUMA][0];

  Pel *pInputPelU = NULL, *pInputPelV = NULL;
  if( m_pppcInputPel[CHROMA_U] )
  {
    pInputPelU = m_pppcInputPel[CHROMA_U][0];
    pInputPelV = m_pppcInputPel[CHROMA_V][0];
  }

  m_bHasRGBPel = false;
  UInt64 frame_bytes_input = m_uiWidth * m_uiHeight;
  switch( m_iPixelFormat )
  {
  case YUV420p:
    memcpy( &m_pppcInputPel[LUMA][0][0], input_buffer + frame_bytes_input * 0, frame_bytes_input * sizeof(Pel) );
    memcpy( &m_pppcInputPel[CHROMA_U][0][0], input_buffer + frame_bytes_input * 1, frame_bytes_input / 4 * sizeof(Pel) );
    memcpy( &m_pppcInputPel[CHROMA_V][0][0], input_buffer + frame_bytes_input * 5 / 4, frame_bytes_input / 4 * sizeof(Pel) );
    break;
  case YUV444p:
    memcpy( &m_pppcInputPel[LUMA][0][0], input_buffer + frame_bytes_input * 0, frame_bytes_input * sizeof(Pel) );
    memcpy( &m_pppcInputPel[CHROMA_U][0][0], input_buffer + frame_bytes_input * 1, frame_bytes_input * sizeof(Pel) );
    memcpy( &m_pppcInputPel[CHROMA_V][0][0], input_buffer + frame_bytes_input * 2, frame_bytes_input * sizeof(Pel) );
    break;
  case YUV422p:
    memcpy( &m_pppcInputPel[LUMA][0][0], input_buffer + frame_bytes_input * 0, frame_bytes_input * sizeof(Pel) );
    memcpy( &m_pppcInputPel[CHROMA_U][0][0], input_buffer + frame_bytes_input * 1, frame_bytes_input / 2 * sizeof(Pel) );
    memcpy( &m_pppcInputPel[CHROMA_V][0][0], input_buffer + frame_bytes_input * 3 / 2, frame_bytes_input / 2 * sizeof(Pel) );
    break;
  case YUYV422:
    for( UInt i = 0; i < m_uiHeight * m_uiWidth / 2; i++ )
    {
      *pInputPelY++ = *input_buffer++;
      *pInputPelU++ = *input_buffer++;
      *pInputPelY++ = *input_buffer++;
      *pInputPelV++ = *input_buffer++;
    }
    break;
  case GRAY:
    memcpy( &m_pppcInputPel[LUMA][0][0], input_buffer + frame_bytes_input * 0, frame_bytes_input * sizeof(Pel) );
    break;
  case RGB8:
    memcpy( pcRGBPelInterlaced, input_buffer, frame_bytes_input * sizeof(Pel) * 3 );
    for( UInt i = 0; i < m_uiHeight * m_uiWidth; i++ )
    {
      *pInputPelY++ = *pcRGBPelInterlaced++;
      *pInputPelU++ = *pcRGBPelInterlaced++;
      *pInputPelV++ = *pcRGBPelInterlaced++;
    }
    m_bHasRGBPel = true;
    break;
  }
}

Void PlaYUVerFrame::FrameToBuffer( Pel *output_buffer )
{

  Pel* pcRGBPelInterlaced = m_pcRGBPelInterlaced;
  Pel* pInputPelY = m_pppcInputPel[LUMA][0];

  Pel *pInputPelU = NULL, *pInputPelV = NULL;
  if( m_pppcInputPel[CHROMA_U] )
  {
    pInputPelU = m_pppcInputPel[CHROMA_U][0];
    pInputPelV = m_pppcInputPel[CHROMA_V][0];
  }

  m_bHasRGBPel = false;
  UInt64 frame_bytes_input = m_uiWidth * m_uiHeight;
  switch( m_iPixelFormat )
  {
  case YUV420p:
    memcpy( output_buffer + frame_bytes_input * 0, &m_pppcInputPel[LUMA][0][0], frame_bytes_input * sizeof(Pel) );
    memcpy( output_buffer + frame_bytes_input * 1, &m_pppcInputPel[CHROMA_U][0][0], frame_bytes_input / 4 * sizeof(Pel) );
    memcpy( output_buffer + frame_bytes_input * 5 / 4, &m_pppcInputPel[CHROMA_V][0][0], frame_bytes_input / 4 * sizeof(Pel) );
    break;
  case YUV444p:
    memcpy( output_buffer + frame_bytes_input * 0, &m_pppcInputPel[LUMA][0][0], frame_bytes_input * sizeof(Pel) );
    memcpy( output_buffer + frame_bytes_input * 1, &m_pppcInputPel[CHROMA_U][0][0], frame_bytes_input * sizeof(Pel) );
    memcpy( output_buffer + frame_bytes_input * 2, &m_pppcInputPel[CHROMA_V][0][0], frame_bytes_input * sizeof(Pel) );
    break;
  case YUV422p:
    memcpy( output_buffer + frame_bytes_input * 0, &m_pppcInputPel[LUMA][0][0], frame_bytes_input * sizeof(Pel) );
    memcpy( output_buffer + frame_bytes_input * 1, &m_pppcInputPel[CHROMA_U][0][0], frame_bytes_input / 2 * sizeof(Pel) );
    memcpy( output_buffer + frame_bytes_input * 3 / 2, &m_pppcInputPel[CHROMA_V][0][0], frame_bytes_input / 2 * sizeof(Pel) );
    break;
  case YUYV422:
    for( UInt i = 0; i < m_uiHeight * m_uiWidth / 2; i++ )
    {
      *output_buffer++ = *pInputPelY++;
      *output_buffer++ = *pInputPelU++;
      *output_buffer++ = *pInputPelY++;
      *output_buffer++ = *pInputPelV++;
    }
    break;
  case GRAY:
    memcpy( output_buffer + frame_bytes_input * 0, &m_pppcInputPel[LUMA][0][0], frame_bytes_input * sizeof(Pel) );
    break;
  case RGB8:
    FrametoRGB8();
    memcpy( output_buffer, pcRGBPelInterlaced, frame_bytes_input * sizeof(Pel) * 3 );
    break;
  }
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

  switch( m_iPixelFormat )
  {
  case YUV420p:
    for( UInt i = 0; i < m_uiHeight / 2; i++ )
    {
      memcpy( m_pppcInputPel[LUMA][(i<<1)], &( input_frame->getPelBufferYUV()[LUMA][yPos + ( i << 1 )][xPos] ), m_uiWidth * sizeof(Pel) );
      memcpy( m_pppcInputPel[LUMA][(i<<1) + 1], &( input_frame->getPelBufferYUV()[LUMA][yPos + ( i << 1 ) + 1][xPos] ), m_uiWidth * sizeof(Pel) );
      memcpy( m_pppcInputPel[CHROMA_U][i], &( input_frame->getPelBufferYUV()[CHROMA_U][yPos / 2 + i][xPos / 2] ), m_uiWidth / 2 * sizeof(Pel) );
      memcpy( m_pppcInputPel[CHROMA_V][i], &( input_frame->getPelBufferYUV()[CHROMA_V][yPos / 2 + i][xPos / 2] ), m_uiWidth / 2 * sizeof(Pel) );
    }
    break;
  case YUV444p:
    for( UInt i = 0; i < m_uiHeight; i++ )
    {
      memcpy( m_pppcInputPel[LUMA][i], &( input_frame->getPelBufferYUV()[LUMA][yPos + i][xPos] ), m_uiWidth * sizeof(Pel) );
      memcpy( m_pppcInputPel[CHROMA_U][i], &( input_frame->getPelBufferYUV()[CHROMA_U][yPos + i][xPos] ), m_uiWidth * sizeof(Pel) );
      memcpy( m_pppcInputPel[CHROMA_V][i], &( input_frame->getPelBufferYUV()[CHROMA_V][yPos + i][xPos] ), m_uiWidth * sizeof(Pel) );
    }
    break;
  case YUV422p:
  case YUYV422:
    for( UInt i = 0; i < m_uiHeight; i++ )
    {
      memcpy( m_pppcInputPel[LUMA][i], &( input_frame->getPelBufferYUV()[LUMA][yPos + i][xPos] ), m_uiWidth * sizeof(Pel) );
      memcpy( m_pppcInputPel[CHROMA_U][i], &( input_frame->getPelBufferYUV()[CHROMA_U][yPos + i][xPos / 2] ), m_uiWidth / 2 * sizeof(Pel) );
      memcpy( m_pppcInputPel[CHROMA_V][i], &( input_frame->getPelBufferYUV()[CHROMA_V][yPos + i][xPos / 2] ), m_uiWidth / 2 * sizeof(Pel) );
    }
    break;
  case GRAY:
    for( UInt i = 0; i < m_uiHeight; i++ )
    {
      memcpy( m_pppcInputPel[LUMA][i], &( input_frame->getPelBufferYUV()[LUMA][yPos + i][xPos] ), m_uiWidth * sizeof(Pel) );
    }
    break;
  case RGB8:
    for( UInt i = 0; i < m_uiHeight; i++ )
    {
      memcpy( m_pppcInputPel[COLOR_R][i], &( input_frame->getPelBufferYUV()[LUMA][yPos + i][xPos] ), m_uiWidth * sizeof(Pel) );
      memcpy( m_pppcInputPel[COLOR_G][i], &( input_frame->getPelBufferYUV()[CHROMA_U][yPos + i][xPos] ), m_uiWidth * sizeof(Pel) );
      memcpy( m_pppcInputPel[COLOR_B][i], &( input_frame->getPelBufferYUV()[CHROMA_V][yPos + i][xPos] ), m_uiWidth * sizeof(Pel) );
    }
    break;
  default:
    Q_ASSERT( 0 );
  }
}

UInt64 PlaYUVerFrame::getBytesPerFrame()
{
  return getBytesPerFrame( m_uiWidth, m_uiHeight, m_iPixelFormat );
}

UInt64 PlaYUVerFrame::getBytesPerFrame( UInt uiWidth, UInt uiHeight, Int iPixelFormat )
{
  switch( iPixelFormat )
  {
  case YUV420p:
    return uiWidth * uiHeight * 1.5;
    break;
  case YUV444p:
    return uiWidth * uiHeight * 3;
    break;
  case YUV422p:
  case YUYV422:
    return uiWidth * uiHeight * 2;
    break;
  case GRAY:
    return uiWidth * uiHeight;
    break;
  case RGB8:
    return uiWidth * uiHeight * 3;
    break;
  default:
    return 0;
  }
  return 0;
}

UInt PlaYUVerFrame::getChromaWidth() const
{
  switch( m_iPixelFormat )
  {
  case YUV420p:
    return m_uiWidth / 2;
    break;
  case YUV444p:
  case YUV422p:
  case YUYV422:
  case RGB8:
    return m_uiWidth;
    break;
  case GRAY:
    return 0;
    break;
  default:
    return 0;
  }
  return 0;
}

UInt PlaYUVerFrame::getChromaHeight() const
{
  switch( m_iPixelFormat )
  {
  case YUV420p:
  case YUV422p:
  case YUYV422:
    return m_uiHeight / 2;
    break;
  case YUV444p:
  case RGB8:
    return m_uiHeight;
  case GRAY:
    return 0;
    break;
  default:
    return 0;
  }
  return 0;
}

UInt PlaYUVerFrame::getChromaLength() const
{
  switch( m_iPixelFormat )
  {
  case YUV420p:
    return m_uiWidth * m_uiHeight / 4;
    break;
  case YUV444p:
    return m_uiWidth * m_uiHeight;
    break;
  case YUV422p:
  case YUYV422:
    return m_uiWidth * m_uiHeight / 2;
    break;
  case GRAY:
    return 0;
    break;
  case RGB8:
    return m_uiWidth * m_uiHeight;
    break;
  default:
    return 0;
  }
  return 0;
}

Pixel PlaYUVerFrame::getPixelValue( const QPoint &pos, Int color_space )
{
  Pixel pixel_value;
  switch( m_iPixelFormat )
  {
  case YUV420p:
    pixel_value.color_space = COLOR_YUV;
    pixel_value.Luma = m_pppcInputPel[LUMA][pos.y()][pos.x()];
    pixel_value.ChromaU = m_pppcInputPel[CHROMA_U][pos.y() >> 1][pos.x() >> 1];
    pixel_value.ChromaV = m_pppcInputPel[CHROMA_V][pos.y() >> 1][pos.x() >> 1];
    break;
  case YUV444p:
    pixel_value.Luma = m_pppcInputPel[LUMA][pos.y()][pos.x()];
    pixel_value.ChromaU = m_pppcInputPel[CHROMA_U][pos.y()][pos.x()];
    pixel_value.ChromaV = m_pppcInputPel[CHROMA_V][pos.y()][pos.x()];
    break;
  case YUV422p:
  case YUYV422:
    pixel_value.color_space = COLOR_YUV;
    pixel_value.Luma = m_pppcInputPel[LUMA][pos.y()][pos.x()];
    pixel_value.ChromaU = m_pppcInputPel[CHROMA_U][pos.y()][pos.x() >> 1];
    pixel_value.ChromaV = m_pppcInputPel[CHROMA_V][pos.y()][pos.x() >> 1];
    break;
  case GRAY:
    pixel_value.color_space = COLOR_YUV;
    pixel_value.Luma = m_pppcInputPel[LUMA][pos.y()][pos.x()];
    pixel_value.ChromaU = 0;
    pixel_value.ChromaV = 0;
    break;
  case RGB8:
    pixel_value.color_space = COLOR_RGB;
    pixel_value.ColorR = m_pppcInputPel[COLOR_R][pos.y()][pos.x()];
    pixel_value.ColorG = m_pppcInputPel[COLOR_G][pos.y()][pos.x()];
    pixel_value.ColorB = m_pppcInputPel[COLOR_B][pos.y()][pos.x()];
    break;
  default:
    break;
  }
  ConvertPixel( pixel_value, color_space );
  return pixel_value;
}

Pixel PlaYUVerFrame::ConvertPixel( Pixel sInputPixel, Int eOutputSpace )
{
  Int outA, outB, outC;
  Pixel sOutputPixel =
  {
    COLOR_INVALID,
    0,
    0,
    0 };

  if( sInputPixel.color_space == eOutputSpace )
    return sInputPixel;

  if( eOutputSpace == COLOR_RGB )
  {
    yuvToRgb<Int>( sInputPixel.Luma, sInputPixel.ChromaU, sInputPixel.ChromaV, outA, outB, outC );
    sOutputPixel.ColorR = outA;
    sOutputPixel.ColorG = outB;
    sOutputPixel.ColorB = outC;
  }
  if( eOutputSpace == COLOR_YUV )
  {
    rgbToYuv<Int>( sInputPixel.ColorR, sInputPixel.ColorG, sInputPixel.ColorB, outA, outB, outC );
    sOutputPixel.Luma = outA;
    sOutputPixel.ChromaU = outB;
    sOutputPixel.ChromaV = outC;
  }
  return sOutputPixel;
}

#ifdef USE_OPENCV
cv::Mat PlaYUVerFrame::getCvMat()
{
  Int cvType = CV_8UC3;
  switch( m_iPixelFormat )
  {
  case YUV420p:
  case YUV444p:
  case YUV422p:
  case YUYV422:
    cvType = CV_8UC3;
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
  cv::Mat opencvFrame( m_uiHeight, m_uiWidth, cvType );
  if( m_iPixelFormat != GRAY )
  {
    FrametoRGB8();
    memcpy( opencvFrame.data, m_pcRGBPelInterlaced, m_uiWidth * m_uiHeight * 3 * sizeof(Pel) );
  }
  else
  {
    memcpy( opencvFrame.data, &( m_pppcInputPel[LUMA][0][0] ), m_uiWidth * m_uiHeight * sizeof(Pel) );
  }
  return opencvFrame;
}
#endif

#ifdef USE_OPENCV
Void PlaYUVerFrame::copyFrom( cv::Mat* opencvFrame )
{
  if( m_iPixelFormat == NO_FMT )
  {
    switch( opencvFrame->channels() )
    {
    case 1:
      m_iPixelFormat = GRAY;
      break;
    case 3:
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
    Pel* pcRGBPelInterlaced = m_pcRGBPelInterlaced;
    memcpy( pcRGBPelInterlaced, opencvFrame->data, m_uiWidth * m_uiHeight * 3 * sizeof(Pel) );
    for( UInt i = 0; i < m_uiHeight * m_uiWidth; i++ )
    {
      *pInputPelY++ = *pcRGBPelInterlaced++;
      *pInputPelU++ = *pcRGBPelInterlaced++;
      *pInputPelV++ = *pcRGBPelInterlaced++;
    }
    m_bHasRGBPel = true;
  }
  else
  {
    memcpy( m_pppcInputPel[LUMA][0], opencvFrame->data, m_uiWidth * m_uiHeight * sizeof(Pel) );
  }
}
#endif

/**
 * Quality Related Function API
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
