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
#ifdef USE_FFMPEG
#include "LibAvContextHandle.h"
#endif

namespace plaYUVer
{

#ifdef USE_FFMPEG
#define ADD_FFMPEG_PEL_FMT( fmt ) fmt
#else
#define ADD_FFMPEG_PEL_FMT( fmt ) 0
#endif

/**
 * YUV Formats
 * Progressive and interlaces
 */

Void frameFromBufferYUVProgressive( Pel* in, Pel ***out, UInt width, UInt height, UInt ratioChroma )
{
  memcpy( &out[LUMA][0][0], in, width * height * sizeof(Pel) );
  memcpy( &out[CHROMA_U][0][0], in + width * height, width * height / ratioChroma * sizeof(Pel) );
  memcpy( &out[CHROMA_V][0][0], in + width * height + width * height / ratioChroma, width * height / ratioChroma * sizeof(Pel) );
}

Void bufferFromFrameYUVProgressive( Pel*** in, Pel *out, UInt width, UInt height, UInt ratioChroma )
{
  memcpy( out, &in[LUMA][0][0], width * height * sizeof(Pel) );
  memcpy( out + width * height, &in[CHROMA_U][0][0], width * height / ratioChroma * sizeof(Pel) );
  memcpy( out + width * height + width * height / ratioChroma, &in[CHROMA_V][0][0], width * height / ratioChroma * sizeof(Pel) );
}

Void fillRGBbufferYUV( Pel*** in, UChar* out, UInt width, UInt height, UInt ratioChromaHor, UInt ratioChromaVer )
{
  Pel** pY = in[LUMA];
  Pel** pU = in[CHROMA_U];
  Pel** pV = in[CHROMA_V];
  Int iY, iU, iV, iR, iG, iB;
  UInt* buff = ( UInt* )out;

  for( UInt y = 0; y < height; y++ )
  {
    for( UInt x = 0; x < width; x++ )
    {
      // Pixel (x, y).
      iY = pY[y][x];
      iU = pU[y >> ratioChromaVer][x >> ratioChromaHor] - 128;
      iV = pV[y >> ratioChromaVer][x >> ratioChromaHor] - 128;
      yuvToRgb<Int>( iY, iU, iV, iR, iG, iB );
      *buff = qRgb( iR, iG, iB );
      buff++;
    }
  }
}

/**
 * YUV420p 12 bits per pixel
 */
Pixel getPixelValueYUV420p( Pel ***Img, Int xPos, Int yPos )
{
  Pixel pixel_value;
  pixel_value.color_space = PlaYUVerFrame::COLOR_YUV;
  pixel_value.Luma = Img[LUMA][yPos][xPos];
  pixel_value.ChromaU = Img[CHROMA_U][yPos >> 1][xPos >> 1];
  pixel_value.ChromaV = Img[CHROMA_V][yPos >> 1][xPos >> 1];
  return pixel_value;
}
Void frameFromBufferYUV420p( Pel *in, Pel*** out, UInt width, UInt height )
{
  frameFromBufferYUVProgressive( in, out, width, height, 4 );
}
Void bufferFromFrameYUV420p( Pel ***in, Pel* out, UInt width, UInt height )
{
  bufferFromFrameYUVProgressive( in, out, width, height, 4 );
}
Void fillRGBbufferYUV420p( Pel*** in, UChar* out, UInt width, UInt height )
{
  fillRGBbufferYUV( in, out, width, height, 1, 1 );
}
PlaYUVerFramePelFormat yuv420p =
{
  "YUV420p",
  PlaYUVerFrame::COLOR_YUV,
  3,
  2,
  2,
  getPixelValueYUV420p,
  frameFromBufferYUV420p,
  bufferFromFrameYUV420p,
  fillRGBbufferYUV420p,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUV420P ), };

/**
 * YUV444p 24 bits per pixel
 */
Pixel getPixelValueYUV444p( Pel ***Img, Int xPos, Int yPos )
{
  Pixel pixel_value;
  pixel_value.color_space = PlaYUVerFrame::COLOR_YUV;
  pixel_value.Luma = Img[LUMA][yPos][xPos];
  pixel_value.ChromaU = Img[CHROMA_U][yPos][xPos];
  pixel_value.ChromaV = Img[CHROMA_V][yPos][xPos];
  return pixel_value;
}
Void frameFromBufferYUV444p( Pel *in, Pel*** out, UInt width, UInt height )
{
  frameFromBufferYUVProgressive( in, out, width, height, 1 );
}
Void bufferFromFrameYUV444p( Pel ***in, Pel* out, UInt width, UInt height )
{
  bufferFromFrameYUVProgressive( in, out, width, height, 1 );
}
Void fillRGBbufferYUV444p( Pel*** in, UChar* out, UInt width, UInt height )
{
  fillRGBbufferYUV( in, out, width, height, 0, 0 );
}
PlaYUVerFramePelFormat yuv444p =
{
  "YUV444p",
  PlaYUVerFrame::COLOR_YUV,
  3,
  1,
  1,
  getPixelValueYUV444p,
  frameFromBufferYUV444p,
  bufferFromFrameYUV444p,
  fillRGBbufferYUV444p,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUV444P ), };

/**
 * YUV422p 16 bits per pixel
 */
Pixel getPixelValueYUV422( Pel ***Img, Int xPos, Int yPos )
{
  Pixel pixel_value;
  pixel_value.color_space = PlaYUVerFrame::COLOR_YUV;
  pixel_value.Luma = Img[LUMA][yPos][xPos];
  pixel_value.ChromaU = Img[CHROMA_U][yPos][xPos >> 1];
  pixel_value.ChromaV = Img[CHROMA_V][yPos][xPos >> 1];
  return pixel_value;
}
Void frameFromBufferYUV422p( Pel *in, Pel*** out, UInt width, UInt height )
{
  frameFromBufferYUVProgressive( in, out, width, height, 2 );
}
Void bufferFromFrameYUV422p( Pel ***in, Pel* out, UInt width, UInt height )
{
  bufferFromFrameYUVProgressive( in, out, width, height, 2 );
}
Void fillRGBbufferYUV422( Pel*** in, UChar* out, UInt width, UInt height )
{
  fillRGBbufferYUV( in, out, width, height, 1, 0 );
}
PlaYUVerFramePelFormat yuv422p =
{
  "YUV422p",
  PlaYUVerFrame::COLOR_YUV,
  3,
  2,
  1,
  getPixelValueYUV422,
  frameFromBufferYUV422p,
  bufferFromFrameYUV422p,
  fillRGBbufferYUV422,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUV422P ), };

/**
 * YUYV422 16 bits per pixel
 */
Void frameFromBufferYUYV422( Pel *in, Pel*** out, UInt width, UInt height )
{
  Pel* pY = out[LUMA][0];
  Pel* pU = out[CHROMA_U][0];
  Pel* pV = out[CHROMA_V][0];
  for( UInt i = 0; i < width * height / 2; i++ )
  {
    *pY++ = *in++;
    *pU++ = *in++;
    *pY++ = *in++;
    *pV++ = *in++;
  }
}
Void bufferFromFrameYUYV422( Pel ***in, Pel* out, UInt width, UInt height )
{
  Pel* pY = in[LUMA][0];
  Pel* pU = in[CHROMA_U][0];
  Pel* pV = in[CHROMA_V][0];
  for( UInt i = 0; i < height * width / 2; i++ )
  {
    *out++ = *pY++;
    *out++ = *pU++;
    *out++ = *pY++;
    *out++ = *pV++;
  }
}

PlaYUVerFramePelFormat yuyv422 =
{
  "YUYV422",
  PlaYUVerFrame::COLOR_YUV,
  3,
  2,
  1,
  getPixelValueYUV422,
  frameFromBufferYUYV422,
  bufferFromFrameYUYV422,
  fillRGBbufferYUV422,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUYV422 ), };

/**
 * Gray Format
 */

Pixel getPixelValueGray( Pel ***Img, Int xPos, Int yPos )
{
  Pixel pixel_value;
  pixel_value.color_space = PlaYUVerFrame::COLOR_YUV;
  pixel_value.Luma = Img[LUMA][yPos][xPos];
  pixel_value.ChromaU = 0;
  pixel_value.ChromaV = 0;
  return pixel_value;
}
Void frameFromBufferGray( Pel *in, Pel*** out, UInt width, UInt height )
{
  memcpy( &out[LUMA][0][0], in, width * height * sizeof(Pel) );
}
Void bufferFromFrameGray( Pel ***in, Pel* out, UInt width, UInt height )
{
  memcpy( out, &in[LUMA][0][0], width * height * sizeof(Pel) );
}
Void fillRGBbufferGray( Pel*** in, UChar* out, UInt width, UInt height )
{
  Pel *inPel = in[0][0];
  UInt* buff = ( UInt* )out;
  for( UInt i = 0; i < height * width; i++ )
  {
    *buff++ = qRgb( *inPel, *inPel, *inPel++ );
  }
}
PlaYUVerFramePelFormat gray =
{
  "GRAY",
  PlaYUVerFrame::COLOR_GRAY,
  1,
  0,
  0,
  getPixelValueGray,
  frameFromBufferGray,
  bufferFromFrameGray,
  fillRGBbufferGray,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_GRAY8 ), };

/**
 * RGB formats
 * Interlaced
 */

/** RGB 8 bits per pixel / 3 channels */
Pixel getPixelValueRGB24( Pel ***Img, Int xPos, Int yPos )
{
  Pixel pixel_value;
  pixel_value.color_space = PlaYUVerFrame::COLOR_RGB;
  pixel_value.ColorR = Img[COLOR_R][yPos][xPos];
  pixel_value.ColorG = Img[COLOR_G][yPos][xPos];
  pixel_value.ColorB = Img[COLOR_B][yPos][xPos];
  return pixel_value;
}
Void frameFromBufferRGB24( Pel *in, Pel*** out, UInt width, UInt height )
{
  Pel* pR = out[COLOR_R][0];
  Pel* pG = out[COLOR_G][0];
  Pel* pB = out[COLOR_B][0];
  for( UInt i = 0; i < width * height; i++ )
  {
    *pR++ = *in++;
    *pG++ = *in++;
    *pB++ = *in++;
  }
}
Void bufferFromFrameRGB24( Pel ***in, Pel* out, UInt width, UInt height )
{
  Pel* pR = in[COLOR_R][0];
  Pel* pG = in[COLOR_G][0];
  Pel* pB = in[COLOR_B][0];
  for( UInt i = 0; i < width * height; i++ )
  {
    *out++ = *pR++;
    *out++ = *pG++;
    *out++ = *pB++;
  }
}
Void fillRGBbufferRGB24( Pel*** in, UChar* out, UInt width, UInt height )
{
  Pel* pR = in[COLOR_R][0];
  Pel* pG = in[COLOR_G][0];
  Pel* pB = in[COLOR_B][0];
  Int iR, iG, iB;
  UInt* buff = ( UInt* )out;
  for( UInt i = 0; i < height * width; i++ )
  {
    iR = *pR++;
    iG = *pG++;
    iB = *pB++;
    *buff++ = qRgb( iR, iG, iB );
  }
}
PlaYUVerFramePelFormat rgb24 =
{
  "RGB8",
  PlaYUVerFrame::COLOR_RGB,
  3,
  1,
  1,
  getPixelValueRGB24,
  frameFromBufferRGB24,
  bufferFromFrameRGB24,
  fillRGBbufferRGB24,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_RGB24 ), };

extern PlaYUVerFramePelFormat g_PlaYUVerFramePelFormatsList[PLAYUVER_NUMBER_FORMATS] =
{
  yuv420p,
  yuv444p,
  yuv422p,
  yuyv422,
  gray,
  rgb24, };

}  // NAMESPACE
