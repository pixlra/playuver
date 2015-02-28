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
 * \file     PlaYUVerFramePixelFormats.cpp
 * \brief    Handling the pixel formats definition
 */

#include "config.h"
#include <cstdio>
#include "LibMemory.h"
#ifdef USE_SSE
#include <emmintrin.h>
#include <smmintrin.h>
#include <limits.h>
#endif
#ifdef USE_FFMPEG
#include "LibAvContextHandle.h"
#endif
#include "PlaYUVerFramePixelFormats.h"

namespace plaYUVer
{

/*
 **************************************************************
 * Handle different pel formats
 * Interface to generalise into one struct the pixel formats
 **************************************************************
 */

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
  Pel* pY = in[LUMA][0];
  Pel* pLineU = in[CHROMA_U][0];
  Pel* pLineV = in[CHROMA_V][0];
  UInt uiChromaStride = width >> ratioChromaHor;
  Pel* pU;
  Pel* pV;
  Int iY, iU, iV, iR, iG, iB;
  UInt* buff = ( UInt* )out;

  for( UInt y = 0; y < height >> ratioChromaVer; y++ )
  {
    for( Int i = 0; i < 1 << ratioChromaVer; i++ )
    {
      pU = pLineU;
      pV = pLineV;
      for( UInt x = 0; x < width >> ratioChromaHor; x++ )
      {
        iU = *pU++;
        iV = *pV++;
        for( Int j = 0; j < 1 << ratioChromaHor; j++ )
        {
          iY = *pY++;
          YUV2RGB( iY, iU, iV, iR, iG, iB );
          *buff = PEL_RGB( iR, iG, iB );
          buff++;
        }
      }
    }
    pLineU += uiChromaStride;
    pLineV += uiChromaStride;
  }
}

/**
 * YUV420p 12 bits per pixel
 */

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
#ifndef USE_SSE
  fillRGBbufferYUV( in, out, width, height, 1, 1 );
#else
  uint8_t *yp = in[LUMA][0];
  uint8_t *up = in[CHROMA_U][0];
  uint8_t *vp = in[CHROMA_V][0];
  uint32_t suv = width >> 1;
  uint32_t *rgb = ( uint32_t* )out;

  __m128i y0r0, y0r1, u0, v0;
  __m128i y00r0, y01r0, y00r1, y01r1;
  __m128i u00, u01, v00, v01;
  __m128i rv00, rv01, gu00, gu01, gv00, gv01, bu00, bu01;
  __m128i r00, r01, g00, g01, b00, b01;
  __m128i rgb0123, rgb4567, rgb89ab, rgbcdef;
  __m128i gbgb;
  __m128i ysub, uvsub;
  __m128i zero, facy, facrv, facgu, facgv, facbu;
  __m128i *srcy128r0, *srcy128r1;
  __m128i *dstrgb128r0, *dstrgb128r1;
  __m64 *srcu64, *srcv64;
  UInt x, y;

  ysub = _mm_set1_epi32( 0x00100010 );
  uvsub = _mm_set1_epi32( 0x00800080 );

  facy = _mm_set1_epi32( 0x004a004a );
  facrv = _mm_set1_epi32( 0x00660066 );
  facgu = _mm_set1_epi32( 0x00190019 );
  facgv = _mm_set1_epi32( 0x00340034 );
  facbu = _mm_set1_epi32( 0x00810081 );

  zero = _mm_set1_epi32( 0x00000000 );

  for( y = 0; y < height; y += 2 )
  {

    srcy128r0 = ( __m128i * )( yp + width * y );
    srcy128r1 = ( __m128i * )( yp + width * y + width );
    srcu64 = ( __m64 * )( up + suv * ( y / 2 ) );
    srcv64 = ( __m64 * )( vp + suv * ( y / 2 ) );

    dstrgb128r0 = ( __m128i * )( rgb + width * y );
    dstrgb128r1 = ( __m128i * )( rgb + width * y + width );

    for( x = 0; x < width; x += 16 )
    {

      u0 = _mm_loadl_epi64( ( __m128i * )srcu64 );
      srcu64++;
      v0 = _mm_loadl_epi64( ( __m128i * )srcv64 );
      srcv64++;

      y0r0 = _mm_load_si128( srcy128r0++ );
      y0r1 = _mm_load_si128( srcy128r1++ );

      // constant y factors
      y00r0 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpacklo_epi8( y0r0, zero ), ysub ), facy );
      y01r0 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpackhi_epi8( y0r0, zero ), ysub ), facy );
      y00r1 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpacklo_epi8( y0r1, zero ), ysub ), facy );
      y01r1 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpackhi_epi8( y0r1, zero ), ysub ), facy );

      // expand u and v so they're aligned with y values
      u0 = _mm_unpacklo_epi8( u0, zero );
      u00 = _mm_sub_epi16( _mm_unpacklo_epi16( u0, u0 ), uvsub );
      u01 = _mm_sub_epi16( _mm_unpackhi_epi16( u0, u0 ), uvsub );

      v0 = _mm_unpacklo_epi8( v0, zero );
      v00 = _mm_sub_epi16( _mm_unpacklo_epi16( v0, v0 ), uvsub );
      v01 = _mm_sub_epi16( _mm_unpackhi_epi16( v0, v0 ), uvsub );

      // common factors on both rows.
      rv00 = _mm_mullo_epi16( facrv, v00 );
      rv01 = _mm_mullo_epi16( facrv, v01 );
      gu00 = _mm_mullo_epi16( facgu, u00 );
      gu01 = _mm_mullo_epi16( facgu, u01 );
      gv00 = _mm_mullo_epi16( facgv, v00 );
      gv01 = _mm_mullo_epi16( facgv, v01 );
      bu00 = _mm_mullo_epi16( facbu, u00 );
      bu01 = _mm_mullo_epi16( facbu, u01 );

      // row 0
      r00 = _mm_srai_epi16( _mm_add_epi16( y00r0, rv00 ), 6 );
      r01 = _mm_srai_epi16( _mm_add_epi16( y01r0, rv01 ), 6 );
      g00 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y00r0, gu00 ), gv00 ), 6 );
      g01 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y01r0, gu01 ), gv01 ), 6 );
      b00 = _mm_srai_epi16( _mm_add_epi16( y00r0, bu00 ), 6 );
      b01 = _mm_srai_epi16( _mm_add_epi16( y01r0, bu01 ), 6 );

      r00 = _mm_packus_epi16( r00, r01 );// rrrr.. saturated
      g00 = _mm_packus_epi16( g00, g01 );// gggg.. saturated
      b00 = _mm_packus_epi16( b00, b01 );// bbbb.. saturated

      r01 = _mm_unpacklo_epi8( r00, zero );// 0r0r..
      gbgb = _mm_unpacklo_epi8( b00, g00 );// gbgb..
      rgb0123 = _mm_unpacklo_epi16( gbgb, r01 );// 0rgb0rgb..
      rgb4567 = _mm_unpackhi_epi16( gbgb, r01 );// 0rgb0rgb..

      r01 = _mm_unpackhi_epi8( r00, zero );
      gbgb = _mm_unpackhi_epi8( b00, g00 );
      rgb89ab = _mm_unpacklo_epi16( gbgb, r01 );
      rgbcdef = _mm_unpackhi_epi16( gbgb, r01 );

      _mm_store_si128( dstrgb128r0++, rgb0123 );
      _mm_store_si128( dstrgb128r0++, rgb4567 );
      _mm_store_si128( dstrgb128r0++, rgb89ab );
      _mm_store_si128( dstrgb128r0++, rgbcdef );

      // row 1
      r00 = _mm_srai_epi16( _mm_add_epi16( y00r1, rv00 ), 6 );
      r01 = _mm_srai_epi16( _mm_add_epi16( y01r1, rv01 ), 6 );
      g00 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y00r1, gu00 ), gv00 ), 6 );
      g01 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y01r1, gu01 ), gv01 ), 6 );
      b00 = _mm_srai_epi16( _mm_add_epi16( y00r1, bu00 ), 6 );
      b01 = _mm_srai_epi16( _mm_add_epi16( y01r1, bu01 ), 6 );

      r00 = _mm_packus_epi16( r00, r01 );// rrrr.. saturated
      g00 = _mm_packus_epi16( g00, g01 );// gggg.. saturated
      b00 = _mm_packus_epi16( b00, b01 );// bbbb.. saturated

      r01 = _mm_unpacklo_epi8( r00, zero );// 0r0r..
      gbgb = _mm_unpacklo_epi8( b00, g00 );// gbgb..
      rgb0123 = _mm_unpacklo_epi16( gbgb, r01 );// 0rgb0rgb..
      rgb4567 = _mm_unpackhi_epi16( gbgb, r01 );// 0rgb0rgb..

      r01 = _mm_unpackhi_epi8( r00, zero );
      gbgb = _mm_unpackhi_epi8( b00, g00 );
      rgb89ab = _mm_unpacklo_epi16( gbgb, r01 );
      rgbcdef = _mm_unpackhi_epi16( gbgb, r01 );

      _mm_store_si128( dstrgb128r1++, rgb0123 );
      _mm_store_si128( dstrgb128r1++, rgb4567 );
      _mm_store_si128( dstrgb128r1++, rgb89ab );
      _mm_store_si128( dstrgb128r1++, rgbcdef );

    }
  }
#endif
}
PlaYUVerFramePelFormat yuv420p =
{
  "YUV420p",
  PlaYUVerFrame::COLOR_YUV,
  3,
  2,
  2,
  frameFromBufferYUV420p,
  bufferFromFrameYUV420p,
  fillRGBbufferYUV420p,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUV420P ), };

/**
 * YUV444p 24 bits per pixel
 */

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
  frameFromBufferYUV444p,
  bufferFromFrameYUV444p,
  fillRGBbufferYUV444p,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUV444P ), };

/**
 * YUV422p 16 bits per pixel
 */

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
  frameFromBufferYUYV422,
  bufferFromFrameYUYV422,
  fillRGBbufferYUV422,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUYV422 ), };

/**
 * Gray Format
 */

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
  Pel iY;
  UInt* buff = ( UInt* )out;
  for( UInt i = 0; i < height * width; i++ )
  {
    iY = *inPel;
    *buff++ = PEL_RGB( iY, iY, iY );
    inPel++;
  }
}
PlaYUVerFramePelFormat gray =
{
  "GRAY",
  PlaYUVerFrame::COLOR_GRAY,
  1,
  0,
  0,
  frameFromBufferGray,
  bufferFromFrameGray,
  fillRGBbufferGray,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_GRAY8 ), };

/**
 * RGB formats
 * Interlaced
 */

Void frameFromBufferRGB( Pel *in, Pel*** out, UInt64 size, Pel* pFirst, Pel* pSecond, Pel* pthird )
{
  for( UInt i = 0; i < size; i++ )
  {
    *pFirst++ = *in++;
    *pSecond++ = *in++;
    *pthird++ = *in++;
  }
}
Void bufferFromFrameRGB( Pel ***in, Pel* out, UInt64 size, Pel* pFirst, Pel* pSecond, Pel* pthird )
{
  for( UInt i = 0; i < size; i++ )
  {
    *out++ = *pFirst++;
    *out++ = *pSecond++;
    *out++ = *pthird++;
  }
}
Void fillRGBbufferRGB( Pel*** in, UChar* out, UInt width, UInt height )
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
    *buff++ = PEL_RGB( iR, iG, iB );
  }
}

/** RGB 8 bits per pixel / 3 channels */
Void frameFromBufferRGB24( Pel *in, Pel*** out, UInt width, UInt height )
{
  frameFromBufferRGB( in, out, width * height, out[COLOR_R][0], out[COLOR_G][0], out[COLOR_B][0] );
}
Void bufferFromFrameRGB24( Pel ***in, Pel* out, UInt width, UInt height )
{
  bufferFromFrameRGB( in, out, width * height, in[COLOR_R][0], in[COLOR_G][0], in[COLOR_B][0] );
}

PlaYUVerFramePelFormat RGB24 =
{
  "RGB24",
  PlaYUVerFrame::COLOR_RGB,
  3,
  1,
  1,
  frameFromBufferRGB24,
  bufferFromFrameRGB24,
  fillRGBbufferRGB,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_RGB24 ), };

/** BGR 8 bits per pixel / 3 channels */
Void frameFromBufferBGR24( Pel *in, Pel*** out, UInt width, UInt height )
{
  frameFromBufferRGB( in, out, width * height, out[COLOR_B][0], out[COLOR_G][0], out[COLOR_R][0] );
}
Void bufferFromFrameBGR24( Pel ***in, Pel* out, UInt width, UInt height )
{
  bufferFromFrameRGB( in, out, width * height, in[COLOR_B][0], in[COLOR_G][0], in[COLOR_R][0] );
}

PlaYUVerFramePelFormat BGR24 =
{
  "BRG24",
  PlaYUVerFrame::COLOR_RGB,
  3,
  1,
  1,
  frameFromBufferBGR24,
  bufferFromFrameBGR24,
  fillRGBbufferRGB,
  ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_BGR24 ), };

PlaYUVerFramePelFormat g_PlaYUVerFramePelFormatsList[PLAYUVER_NUMBER_FORMATS] =
{
  yuv420p,
  yuv444p,
  yuv422p,
  yuyv422,
  gray,
  RGB24,
  BGR24, };

}  // NAMESPACE
