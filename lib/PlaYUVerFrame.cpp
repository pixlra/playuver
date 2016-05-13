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
 * \file     PlaYUVerFrame.cpp
 * \brief    Video Frame handling
 */

#include "config.h"
#include "PlaYUVerFrame.h"

#include <cmath>
#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#endif

#include "LibMemory.h"
#include "PlaYUVerFramePixelFormats.h"

/*! \brief Supported formats
 *
 *  List of supported pixels formats
 */
std::vector<String> PlaYUVerFrame::supportedColorSpacesListNames()
{
  return std::vector<String>{
    "YUV",
    "RGB",
    "GRAY",
  };
}

std::vector<String> PlaYUVerFrame::supportedPixelFormatListNames()
{
  std::vector<String> formatsList;
  for( Int i = 0; i < NUMBER_PEL_FORMATS; i++ )
  {
    formatsList.push_back( g_PlaYUVerPixFmtDescriptorsList[i].name );
  }
  return formatsList;
}

std::vector<String> PlaYUVerFrame::supportedPixelFormatListNames( Int colorSpace )
{
  std::vector<String> formatsList;
  for( Int i = 0; i < NUMBER_PEL_FORMATS; i++ )
  {
    if( g_PlaYUVerPixFmtDescriptorsList[i].colorSpace == colorSpace )
      formatsList.push_back( g_PlaYUVerPixFmtDescriptorsList[i].name );
  }
  return formatsList;
}

Int PlaYUVerFrame::findPixelFormat( String name )
{
  for( Int i = 0; i < NUMBER_PEL_FORMATS; i++ )
  {
    if( g_PlaYUVerPixFmtDescriptorsList[i].name == name )
      return i;
  }
  return -1;
}

struct PlaYUVerFramePrivate
{
  /**
   * Common constructor function of a frame
   *
   * @param width width of the frame
   * @param height height of the frame
   * @param pel_format pixel format index (always use PixelFormats enum)
   *
   */
  Void init( UInt width, UInt height, Int pel_format, Int bitsPixel, Int endianness );

  Bool m_bInit;

  //! Struct with the pixel format description.
  const PlaYUVerPixFmtDescriptor* m_pcPelFormat;
  String m_cPelFmtName;

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

  Bool m_bHasHistogram;
};

/*! \brief Constructors
 *
 *  Constructors and init functions
 */
PlaYUVerFrame::PlaYUVerFrame( UInt width, UInt height, Int pelFormat, Int bitsPixel, Int endianness ) :
        d( new PlaYUVerFramePrivate )
{
  d->init( width, height, pelFormat, bitsPixel, endianness );
}

PlaYUVerFrame::PlaYUVerFrame( PlaYUVerFrame *other, Bool bCopy ) :
        d( new PlaYUVerFramePrivate )
{
  d->init( other->getWidth(), other->getHeight(), other->getPelFormat(), other->getBitsPel(), other->getEndianness() );
  copyFrom( other );
}

PlaYUVerFrame::PlaYUVerFrame( PlaYUVerFrame *other, UInt posX, UInt posY, UInt areaWidth, UInt areaHeight ) :
        d( new PlaYUVerFramePrivate )
{
  const PlaYUVerPixFmtDescriptor* pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsList[other->getPelFormat()] );
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

  d->init( areaWidth, areaHeight, other->getPelFormat(), other->getBitsPel(), other->getEndianness() );
  copyFrom( other, posX, posY );
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

Void PlaYUVerFramePrivate::init( UInt width, UInt height, Int pel_format, Int bitsPixel, Int endianness )
{
  m_bInit = false;
  m_pppcInputPel = NULL;
  m_pcARGB32 = NULL;
  m_uiWidth = width;
  m_uiHeight = height;
  m_iPixelFormat = pel_format;
  m_iNumberChannels = 3;
  m_uiBitsPel = bitsPixel > 8 ? bitsPixel : 8;
  m_iEndianness = endianness;
  if( m_uiBitsPel > 8 && m_iEndianness == -1 )
  {
    throw PlaYUVerFailure( "PlaYUVerFrame", "Invalid endianness type for Bits per Pixel greater than 8" );
  }
  m_uiHalfPelValue = 1 << ( m_uiBitsPel - 1 );

  if( m_uiWidth == 0 || m_uiHeight == 0 || m_iPixelFormat == -1 || bitsPixel > 16 )
  {
    throw PlaYUVerFailure( "PlaYUVerFrame", "Cannot create a PlYUVerFrame of this type" );
  }

  m_pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsList[pel_format] );

  m_bHasRGBPel = false;
  if( m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_GRAY )
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

  m_uiHistoSegments = 1 << m_uiBitsPel;

  if( m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_RGB || m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_ARGB )
    m_uiHistoChannels = m_pcPelFormat->numberChannels + 1;
  else
    m_uiHistoChannels = m_pcPelFormat->numberChannels;

  m_cPelFmtName = PlaYUVerFrame::supportedPixelFormatListNames()[m_iPixelFormat].c_str();

  m_bInit = true;
}

PlaYUVerFrame::~PlaYUVerFrame()
{
  if( d->m_puiHistogram )
  {
    freeMem1D( d->m_puiHistogram );
  }
  d->m_puiHistogram = NULL;
  d->m_bHasHistogram = false;

  if( d->m_pppcInputPel )
    freeMem3ImageComponents<Pel>( d->m_pppcInputPel );
  d->m_pppcInputPel = NULL;

  if( d->m_pcARGB32 )
    freeMem1D( d->m_pcARGB32 );
  d->m_pcARGB32 = NULL;

  delete d;
}

Void PlaYUVerFrame::clear()
{
  Pel* pPel;
  Pel uiValue = 128;
  Int ratioH, ratioW;
  UInt i;
  for( UInt ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    pPel = d->m_pppcInputPel[ch][0];
    for( i = 0; i < CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ); i++ )
    {
      *pPel++ = uiValue;
    }
  }
}

Bool PlaYUVerFrame::haveSameFmt( PlaYUVerFrame* other, UInt match ) const
    {
  Bool bRet = true;
  if( other )
  {
    if( match & MATCH_COLOR_SPACE )
      bRet &= ( getColorSpace() == other->getColorSpace() );
    if( match & MATCH_RESOLUTION )
      bRet &= ( getWidth() == other->getWidth() ) && ( getHeight() == other->getHeight() );
    if( match & MATCH_PEL_FMT )
      bRet &= ( getPelFormat() == other->getPelFormat() );
    if( match & MATCH_BITS )
      bRet &= ( getBitsPel() == other->getBitsPel() );
    if( match & MATCH_COLOR_SPACE_IGNORE_GRAY )
      bRet &= ( getColorSpace() == PlaYUVerPixel::COLOR_GRAY || getColorSpace() == other->getColorSpace() );
  }
  else
  {
    bRet = false;
  }
  return bRet;
}

Int PlaYUVerFrame::getPelFormat() const
{
  return d->m_iPixelFormat;
}

String PlaYUVerFrame::getPelFmtName()
{
  return d->m_pcPelFormat->name;
}

Int PlaYUVerFrame::getColorSpace() const
{
  return d->m_pcPelFormat->colorSpace;
}

UInt PlaYUVerFrame::getNumberChannels() const
{
  return d->m_pcPelFormat->numberChannels;
}

UInt PlaYUVerFrame::getWidth( Int channel ) const
    {
  return CHROMASHIFT( d->m_uiWidth, channel > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0 );
}
UInt PlaYUVerFrame::getHeight( Int channel ) const
    {
  return CHROMASHIFT( d->m_uiHeight, channel > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0 );
}

UInt PlaYUVerFrame::getPixels( Int channel ) const
    {
  return getWidth( channel ) * getWidth( channel );
}

UInt8 PlaYUVerFrame::getChromaWidthRatio() const
{
  return d->m_pcPelFormat->log2ChromaWidth;
}

UInt8 PlaYUVerFrame::getChromaHeightRatio() const
{
  return d->m_pcPelFormat->log2ChromaHeight;
}

UInt PlaYUVerFrame::getChromaWidth() const
{
  if( d->m_pcPelFormat->colorSpace == GRAY )
    return 0;
  return CHROMASHIFT( d->m_uiWidth, d->m_pcPelFormat->log2ChromaWidth );
}

UInt PlaYUVerFrame::getChromaHeight() const
{
  if( d->m_pcPelFormat->colorSpace == GRAY )
    return 0;
  return CHROMASHIFT( d->m_uiHeight, d->m_pcPelFormat->log2ChromaHeight );
}

UInt PlaYUVerFrame::getChromaLength() const
{
  return getChromaWidth() * getChromaHeight();
}

UInt PlaYUVerFrame::getBitsPel() const
{
  return d->m_uiBitsPel;
}
Int PlaYUVerFrame::getEndianness() const
{
  return d->m_iEndianness;
}

UInt64 PlaYUVerFrame::getBytesPerFrame()
{
  return getBytesPerFrame( d->m_uiWidth, d->m_uiHeight, d->m_iPixelFormat, d->m_uiBitsPel );
}

UInt64 PlaYUVerFrame::getBytesPerFrame( UInt uiWidth, UInt uiHeight, Int iPixelFormat, UInt bitsPixel )
{
  const PlaYUVerPixFmtDescriptor* pcPelFormat = &( g_PlaYUVerPixFmtDescriptorsList[iPixelFormat] );
  UInt bytesPerPixel = ( bitsPixel - 1 ) / 8 + 1;
  UInt64 numberBytes = uiWidth * uiHeight;
  if( pcPelFormat->numberChannels > 1 )
  {
    UInt64 numberBytesChroma = CHROMASHIFT( uiWidth, pcPelFormat->log2ChromaWidth ) * CHROMASHIFT( uiHeight, pcPelFormat->log2ChromaHeight );
    numberBytes += ( pcPelFormat->numberChannels - 1 ) * numberBytesChroma;
  }
  return numberBytes * bytesPerPixel;
}

Pel*** PlaYUVerFrame::getPelBufferYUV() const
{
  return d->m_pppcInputPel;
}
Pel*** PlaYUVerFrame::getPelBufferYUV()
{
  d->m_bHasHistogram = false;
  d->m_bHasRGBPel = false;
  return d->m_pppcInputPel;
}

UChar* PlaYUVerFrame::getRGBBuffer() const
{
  if( d->m_bHasRGBPel )
  {
    return d->m_pcARGB32;
  }
  return NULL;
}
UChar* PlaYUVerFrame::getRGBBuffer()
{
  if( !d->m_bHasRGBPel )
  {
    fillRGBBuffer();
  }
  return d->m_pcARGB32;
}

PlaYUVerPixel PlaYUVerFrame::getPixelValue( Int xPos, Int yPos )
{
  PlaYUVerPixel PixelValue( d->m_pcPelFormat->colorSpace, d->m_uiHalfPelValue, d->m_uiHalfPelValue, d->m_uiHalfPelValue );
  for( UInt ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    Int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    PixelValue[ch] = d->m_pppcInputPel[ch][( yPos >> ratioH )][( xPos >> ratioW )];
  }
  return PixelValue;
}

PlaYUVerPixel PlaYUVerFrame::getPixelValue( Int xPos, Int yPos, PlaYUVerPixel::ColorSpace eColorSpace )
{
  PlaYUVerPixel PixelValue( d->m_pcPelFormat->colorSpace );
  for( UInt ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    Int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    PixelValue[ch] = d->m_pppcInputPel[ch][( yPos >> ratioH )][( xPos >> ratioW )];
  }
  PixelValue = PixelValue.ConvertPixel( eColorSpace );
  return PixelValue;
}

Void PlaYUVerFrame::setPixelValue( Int xPos, Int yPos, PlaYUVerPixel pixel )
{
  for( UInt ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    Int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    d->m_pppcInputPel[ch][( yPos >> ratioH )][( xPos >> ratioW )] = pixel.Components()[ch];
  }
  d->m_bHasHistogram = false;
  d->m_bHasRGBPel = false;
}

Void PlaYUVerFrame::copyFrom( PlaYUVerFrame* input_frame )
{
  if( d->m_iPixelFormat != input_frame->getPelFormat() && d->m_uiWidth == input_frame->getWidth() && d->m_uiHeight == input_frame->getHeight() )
    return;
  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;
  memcpy( *d->m_pppcInputPel[LUMA], input_frame->getPelBufferYUV()[LUMA][0], getBytesPerFrame() * sizeof(Pel) );
}

Void PlaYUVerFrame::copyFrom( PlaYUVerFrame* input_frame, UInt xPos, UInt yPos )
{
  if( d->m_iPixelFormat != input_frame->getPelFormat() )
    return;
  Pel ***pInput = input_frame->getPelBufferYUV();
  for( UInt ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    Int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    Int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    for( UInt i = 0; i < CHROMASHIFT( d->m_uiHeight, ratioH ); i++ )
    {
      memcpy( d->m_pppcInputPel[ch][i], &( pInput[ch][( yPos >> ratioH ) + i][( xPos >> ratioW )] ), ( d->m_uiWidth >> ratioW ) * sizeof(Pel) );
    }
  }
  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;
}

Void PlaYUVerFrame::frameFromBuffer( Byte *Buff, UInt64 uiBuffSize )
{
  if( uiBuffSize != getBytesPerFrame() )
    return;

  frameFromBuffer( Buff );
}

Void PlaYUVerFrame::frameFromBuffer( Byte *Buff )
{
  Byte* ppBuff[MAX_NUMBER_PLANES];
  Byte* pTmpBuff;
  Pel* pPel;
  UInt bytesPixel = ( d->m_uiBitsPel - 1 ) / 8 + 1;
  Int ratioH, ratioW, step;
  UInt i, ch;
  Int startByte = 0;
  Int endByte = bytesPixel;
  Int incByte = 1;
  Int b;

  if( d->m_iEndianness == 0 )
  {
    startByte = bytesPixel - 1;
    endByte = -1;
    incByte = -1;
  }

  ppBuff[0] = Buff;
  for( i = 1; i < MAX_NUMBER_PLANES; i++ )
  {
    ratioW = i > 1 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = i > 1 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    ppBuff[i] = ppBuff[i - 1] + CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ) * bytesPixel;
  }

  for( ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    step = d->m_pcPelFormat->comp[ch].step_minus1;

    pPel = d->m_pppcInputPel[ch][0];
    pTmpBuff = ppBuff[d->m_pcPelFormat->comp[ch].plane] + ( d->m_pcPelFormat->comp[ch].offset_plus1 - 1 );

    for( i = 0; i < CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ); i++ )
    {
      *pPel = 0;
      for( b = startByte; b != endByte; b += incByte )
      {
        *pPel += *pTmpBuff << ( b * 8 );
        pTmpBuff++;
      }
      pPel++;
      pTmpBuff += step;
    }
  }
  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;
}

Void PlaYUVerFrame::frameToBuffer( Byte *output_buffer )
{
  UInt bytesPixel = ( d->m_uiBitsPel - 1 ) / 8 + 1;
  Byte* ppBuff[MAX_NUMBER_PLANES];
  Byte* pTmpBuff;
  Pel* pTmpPel;
  Int ratioH, ratioW, step;
  UInt i, ch;
  Int startByte = 0;
  Int endByte = bytesPixel;
  Int incByte = 1;
  Int b;

  if( d->m_iEndianness == 0 )
  {
    startByte = bytesPixel - 1;
    endByte = -1;
    incByte = -1;
  }

  ppBuff[0] = output_buffer;
  for( Int i = 1; i < MAX_NUMBER_PLANES; i++ )
  {
    ratioW = i > 1 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = i > 1 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    ppBuff[i] = ppBuff[i - 1] + CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ) * bytesPixel;
  }

  for( ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    step = d->m_pcPelFormat->comp[ch].step_minus1;

    pTmpPel = d->m_pppcInputPel[ch][0];
    pTmpBuff = ppBuff[d->m_pcPelFormat->comp[ch].plane];

    for( i = 0; i < CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ); i++ )
    {
      for( b = startByte; b != endByte; b += incByte )
      {
        *pTmpBuff = *pTmpPel >> ( 8 * b );
        pTmpBuff++;
      }
      pTmpPel++;
      pTmpBuff += step;
    }
  }
}

Void PlaYUVerFrame::fillRGBBuffer()
{
#define CLAMP_YUV2RGB(X) X = X < 0 ? 0 : X > 255 ? 255 : X;
#define YUV2RGB(  iY, iU, iV, iR, iG, iB ) \
iR = iY + ( ( 1436 * ( iV - 128) ) >> 10 ); \
iG = iY - ( ( 352 * ( iU - 128 ) + 731 * ( iV - 128 ) ) >> 10 ); \
iB = iY + ( ( 1812 * ( iU - 128 ) ) >> 10 ); \
CLAMP_YUV2RGB(iR) CLAMP_YUV2RGB(iG) CLAMP_YUV2RGB(iB)

  if( d->m_bHasRGBPel )
    return;
  Int iR, iG, iB;
  Int shiftBits = d->m_uiBitsPel - 8;
  UInt* pARGB = ( UInt* )d->m_pcARGB32;
  if( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_GRAY )
  {
    Pel* pY = d->m_pppcInputPel[LUMA][0];
    Int iY;
    for( UInt i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
    {
      iY = *pY++;
      iY >>= shiftBits;
      iR = iY;
      iG = iY;
      iB = iY;
      *pARGB++ = PEL_RGB( iR, iG, iB );
    }
  }
  else if( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_RGB )
  {
    Pel* pR = d->m_pppcInputPel[COLOR_R][0];
    Pel* pG = d->m_pppcInputPel[COLOR_G][0];
    Pel* pB = d->m_pppcInputPel[COLOR_B][0];

    for( UInt i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
    {
      iR = *pR++;
      iG = *pG++;
      iB = *pB++;
      *pARGB++ = PEL_RGB( ( iR >> shiftBits ), ( iG >> shiftBits ), ( iB >> shiftBits ) );
    }
  }
  else if( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_YUV )
  {
    Pel* pLineY = d->m_pppcInputPel[LUMA][0];
    Pel* pLineU = d->m_pppcInputPel[CHROMA_U][0];
    Pel* pLineV = d->m_pppcInputPel[CHROMA_V][0];
    UInt uiChromaStride = CHROMASHIFT( d->m_uiWidth, d->m_pcPelFormat->log2ChromaWidth );
    Pel* pY;
    Pel* pU;
    Pel* pV;
    Int iY, iU, iV, iR, iG, iB;
    UInt* pARGBLine = pARGB;
    UInt* pARGB;

    UInt y, x;
    Int i, j;
    for( y = 0; y < d->m_uiHeight >> d->m_pcPelFormat->log2ChromaHeight; y++ )
    {
      for( i = 0; i < 1 << d->m_pcPelFormat->log2ChromaHeight; i++ )
      {
        pY = pLineY;
        pU = pLineU;
        pV = pLineV;
        pARGB = pARGBLine;
        for( x = 0; x < d->m_uiWidth >> d->m_pcPelFormat->log2ChromaWidth; x++ )
        {
          iU = *pU++;
          iU >>= shiftBits;
          iV = *pV++;
          iV >>= shiftBits;
          for( j = 0; j < ( 1 << d->m_pcPelFormat->log2ChromaWidth ); j++ )
          {
            iY = *pY++;
            iY >>= shiftBits;
            YUV2RGB( iY, iU, iV, iR, iG, iB );
            *pARGB++ = PEL_RGB( iR, iG, iB );
          }
        }
        pLineY += d->m_uiWidth;
        pARGBLine += d->m_uiWidth;
      }
      pLineU += uiChromaStride;
      pLineV += uiChromaStride;
    }
  }
  d->m_bHasRGBPel = true;
}

/**
 * Histogram
 */

Void PlaYUVerFrame::calcHistogram()
{
  if( d->m_bHasHistogram )
    return;

  if( !d->m_puiHistogram )
  {
    getMem1D<UInt>( &( d->m_puiHistogram ), d->m_uiHistoSegments * d->m_uiHistoChannels );
    if( !d->m_puiHistogram )
    {
      return;
    }
  }
  d->m_bRunningFlag = true;

  xMemSet( UInt, d->m_uiHistoSegments * d->m_uiHistoChannels, d->m_puiHistogram );

  UInt i, j;
  UInt numberChannels = d->m_pcPelFormat->numberChannels;
  Int colorSpace = d->m_pcPelFormat->colorSpace;
  if( colorSpace == PlaYUVerPixel::COLOR_YUV )
  {
    const Pel *data[3];
    data[LUMA] = d->m_pppcInputPel[LUMA][0];
    data[CHROMA_U] = d->m_pppcInputPel[CHROMA_U][0];
    data[CHROMA_V] = d->m_pppcInputPel[CHROMA_V][0];
    for( i = 0; i < d->m_uiHeight * d->m_uiWidth && d->m_bRunningFlag; i++ )
    {
      d->m_puiHistogram[*( data[LUMA] ) + LUMA * d->m_uiHistoSegments]++;
      data[LUMA] += 1;
    }
    for( i = 0; ( i < getChromaLength() ) && d->m_bRunningFlag; i++ )
    {
      for( j = 1; j < numberChannels; j++ )
      {
        d->m_puiHistogram[*( data[j] ) + j * d->m_uiHistoSegments]++;
        data[j] += 1;
      }
    }
  }
  else
  {
    const Pel* data[3];
    Pel luma;
    data[COLOR_R] = d->m_pppcInputPel[COLOR_R][0];
    data[COLOR_G] = d->m_pppcInputPel[COLOR_G][0];
    data[COLOR_B] = d->m_pppcInputPel[COLOR_B][0];

    for( i = 0; ( i < d->m_uiHeight * d->m_uiWidth ) && d->m_bRunningFlag; i++ )
    {
      for( j = 0; j < numberChannels; j++ )
      {
        d->m_puiHistogram[*( data[j] ) + j * d->m_uiHistoSegments]++;
      }
      if( ( colorSpace == PlaYUVerPixel::COLOR_RGB || colorSpace == PlaYUVerPixel::COLOR_ARGB ) )
      {
        PlaYUVerPixel currPixel( colorSpace, *( data[COLOR_R] ), *( data[COLOR_G] ), *( data[COLOR_B] ) );
        luma = currPixel.ConvertPixel( PlaYUVerPixel::COLOR_YUV ).Y();
        d->m_puiHistogram[luma + j * d->m_uiHistoSegments]++;
      }
      for( j = 0; j < numberChannels; j++ )
        data[j] += 1;
    }
  }
  d->m_bHasHistogram = true;
  d->m_bRunningFlag = false;
}

Int PlaYUVerFrame::getHistogramSegment()
{
  return d->m_uiHistoSegments;
}

Int getRealHistoChannel( Int colorSpace, Int channel )
{
  Int histoChannel;

  switch( colorSpace )
  {
  case PlaYUVerPixel::COLOR_GRAY:
    if( channel != LUMA )
    {
      return -1;
    }
    histoChannel = 0;
    break;
  case PlaYUVerPixel::COLOR_RGB:
    if( channel != COLOR_R && channel != COLOR_G && channel != COLOR_B && channel != LUMA )
    {
      return -1;
    }
    else
    {
//       if( channel == LUMA )
//       {
//         histoChannel = 3;
//       }
//       else
      {
        histoChannel = channel;
      }
    }
    break;
  case PlaYUVerPixel::COLOR_ARGB:
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
  case PlaYUVerPixel::COLOR_YUV:
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

UInt PlaYUVerFrame::getCount( Int channel, UInt start, UInt end )
{
  UInt i;
  Int histoChannel;
  Int indexStart;
  UInt count = 0;

  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  for( i = start; i <= end; i++ )
  {
    count += d->m_puiHistogram[indexStart + i];
  }
  return count;
}

Double PlaYUVerFrame::getMean( Int channel, UInt start, UInt end )
{
  Int indexStart;
  Int histoChannel;
  Double mean = 0.0;
  Double count;

  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  for( UInt i = start; i <= end; i++ )
  {
    mean += i * d->m_puiHistogram[indexStart + i];
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

  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0;
  }

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  count = getCount( channel, start, end );

  for( UInt i = start; i <= end; i++ )
  {
    sum += d->m_puiHistogram[indexStart + i];
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

  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;
  mean = getMean( channel, start, end );
  count = getCount( channel, start, end );

  if( count == 0.0 )
    count = 1.0;

  /*------------ original

   for ( i = start ; i <= end ; i++ )
   {
   dev += ( i - mean ) * ( i - mean ) * d->m_puiHistogram[indexStart +i];
   }

   return sqrt( dev / count );

   -----------------------*/

  for( UInt i = start; i <= end; i++ )
  {
    dev += ( i * i ) * d->m_puiHistogram[indexStart + i];
  }

  return sqrt( ( dev - count * mean * mean ) / ( count - 1 ) );

}

Double PlaYUVerFrame::getHistogramValue( Int channel, UInt bin )
{
  Double value;
  Int histoChannel;
  Int indexStart;

  if( !d->m_bHasHistogram || bin < 0 || bin > d->m_uiHistoSegments - 1 )
    return 0.0;

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  value = d->m_puiHistogram[indexStart + bin];

  return value;
}

Double PlaYUVerFrame::getMaximum( Int channel )
{
  Double max = 0.0;

  Int histoChannel;
  Int indexStart;

  if( !d->m_bHasHistogram )
    return 0.0;

  histoChannel = getRealHistoChannel( getColorSpace(), channel );

  if( histoChannel == -1 )
  {
    return 0.0;
  }

  indexStart = histoChannel * d->m_uiHistoSegments;

  for( UInt x = 0; x < d->m_uiHistoSegments; x++ )
  {
    if( d->m_puiHistogram[indexStart + x] > max )
    {
      max = d->m_puiHistogram[indexStart + x];
    }
  }

  return max;
}

/*
 **************************************************************
 * Interface to other libs
 **************************************************************
 */

Bool PlaYUVerFrame::toMat( cv::Mat& cvMat, Bool convertToGray )
{
  Bool bRet = false;
#ifdef USE_OPENCV
  if( convertToGray && !( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_YUV || d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_GRAY ) )
  {
    return bRet;
  }
  Int cvType = CV_MAKETYPE( CV_8U, convertToGray ? 1 : d->m_pcPelFormat->numberChannels );
  cvMat.create( d->m_uiHeight, d->m_uiWidth, cvType );
  UChar* pCvPel = cvMat.data;
  if( !convertToGray && ( d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_YUV || d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_RGB ) )
  {
    fillRGBBuffer();
    UChar* pARGB = d->m_pcARGB32;
    for( UInt i = 0; i < d->m_uiWidth * d->m_uiHeight; i++ )
    {
      *pCvPel++ = *pARGB++;
      *pCvPel++ = *pARGB++;
      *pCvPel++ = *pARGB++;
      pARGB++;
    }
    bRet = true;
  }
  else if( convertToGray || d->m_pcPelFormat->colorSpace == PlaYUVerPixel::COLOR_GRAY )
  {
    Pel* pPel = d->m_pppcInputPel[LUMA][0];
    for( UInt i = 0; i < d->m_uiWidth * d->m_uiHeight; i++ )
    {
      *pCvPel++ = *pPel++;
    }
    bRet = true;
  }
  else
  {
    return false;
  }
#endif
  return bRet;
}

Bool PlaYUVerFrame::fromMat( cv::Mat& cvMat )
{
  Bool bRet = false;
#ifdef USE_OPENCV
  if( !d->m_bInit )
  {
    if( d->m_iPixelFormat == NO_FMT )
    {
      switch( cvMat.channels() )
      {
        case 1:
        d->m_iPixelFormat = findPixelFormat( "GRAY" );
        break;
        case 3:
        d->m_iPixelFormat = findPixelFormat( "BGR24" );
        break;
        default:
        return false;
      }
    }
    d->init( cvMat.cols, cvMat.rows, d->m_iPixelFormat, 8, -1 );
  }

  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;

  UChar* pCvPel = cvMat.data;
  if( d->m_iPixelFormat != GRAY )
  {
    Pel* pInputPelR = d->m_pppcInputPel[COLOR_R][0];
    Pel* pInputPelG = d->m_pppcInputPel[COLOR_G][0];
    Pel* pInputPelB = d->m_pppcInputPel[COLOR_B][0];
    for( UInt i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
    {
      *pInputPelB++ = *pCvPel++;
      *pInputPelG++ = *pCvPel++;
      *pInputPelR++ = *pCvPel++;
    }
    bRet = true;
  }
  else
  {
    Pel* pPel = d->m_pppcInputPel[LUMA][0];
    for( UInt i = 0; i < d->m_uiWidth * d->m_uiHeight; i++ )
    {
      *pPel++ = *pCvPel++;
    }
    bRet = true;
  }
#endif
  return bRet;
}

/*
 **************************************************************
 * Quality Related Function API
 **************************************************************
 */

std::vector<String> PlaYUVerFrame::supportedQualityMetricsList()
{
  return std::vector<String> {
    "PSNR",
    "MSE",
    "SSIM",
  };
}

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
    dSSIM = compute_ssim( d->m_pppcInputPel[component], Org->getPelBufferYUV()[component], d->m_uiWidth, d->m_uiHeight, 8, 8, 255, 8 );
  }
  else
  {
    dSSIM = compute_ssim( d->m_pppcInputPel[component], Org->getPelBufferYUV()[component], getChromaWidth(), getChromaHeight(), 4, 4, 255, 8 );
  }
  return dSSIM;
}

