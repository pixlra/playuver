/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2017  by Luis Lucas      (luisfrlucas@gmail.com)
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
#include "PlaYUVerFramePixelFormats.h"

#ifdef USE_SSE
#include <emmintrin.h>
#include <smmintrin.h>
#include <limits.h>
#endif

#include "LibMemory.h"
#include "PlaYUVerFrame.h"
#ifdef USE_FFMPEG
#include "StreamHandlerLibav.h"
#endif

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

#if 0
Void fillARGB32bufferYUV420p( Pel*** in, UChar* out, UInt width, UInt height, UInt depth )
{
#ifdef USE_SSE
  if( ( width % 16 ) == 0 )
  {
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
  }
  else
#endif
  {
    fillARGB32bufferYUV( in, out, width, height, depth, 1, 1 );
  }
}
#endif

const PlaYUVerPixFmtDescriptor g_PlaYUVerPixFmtDescriptorsList[PlaYUVerFrame::NUMBER_PEL_FORMATS] = {
    {
        "YUV420p",
        PlaYUVerPixel::COLOR_YUV,
        3,
        3,
        1,
        1,
        ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUV420P ),
        {
            {0, 0, 1}, /* Y */
            {1, 0, 1}, /* U */
            {2, 0, 1}, /* V */
        },
    },
    {
        "YUV422p",
        PlaYUVerPixel::COLOR_YUV,
        3,
        3,
        1,
        0,
        ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUV422P ),
        {
            {0, 0, 1}, /* Y */
            {1, 0, 1}, /* U */
            {2, 0, 1}, /* V */
        },
    },
    {
        "YUV444p",
        PlaYUVerPixel::COLOR_YUV,
        3,
        3,
        0,
        0,
        ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUV444P ),
        {
            {0, 0, 1}, /* Y */
            {1, 0, 1}, /* U */
            {2, 0, 1}, /* V */
        },
    },
    {
        "YUYV422",
        PlaYUVerPixel::COLOR_YUV,
        3,
        1,
        1,
        0,
        ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_YUYV422 ),
        {
            {0, 1, 1}, /* Y */
            {0, 3, 2}, /* U */
            {0, 3, 4}, /* V */
        },
    },
    {
        "GRAY",
        PlaYUVerPixel::COLOR_GRAY,
        1,
        1,
        0,
        0,
        ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_GRAY8 ),
        {{0, 0, 1}}, /* Y */
    },
    {
        "RGBp",
        PlaYUVerPixel::COLOR_RGB,
        3,
        3,
        0,
        0,
        ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_RGB24 ),
        {
            {0, 0, 1}, /* R */
            {1, 0, 1}, /* G */
            {2, 0, 1}, /* B */
        },
    },
    {
        "RGB",
        PlaYUVerPixel::COLOR_RGB,
        3,
        1,
        0,
        0,
        ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_RGB24 ),
        {
            {0, 2, 1}, /* R */
            {0, 2, 2}, /* G */
            {0, 2, 3}, /* B */
        },
    },
    {
        "BGR",
        PlaYUVerPixel::COLOR_RGB,
        3,
        1,
        0,
        0,
        ADD_FFMPEG_PEL_FMT( AV_PIX_FMT_BGR24 ),
        {
            {0, 2, 3}, /* B */
            {0, 2, 2}, /* R */
            {0, 2, 1}, /* G */
        },
    }};
