/*    This file is a part of Calyp project
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
 * \file     CalypFrame.cpp
 * \brief    Video Frame handling
 */

#include "CalypFrame.h"

#include "LibMemory.h"
#include "PixelFormats.h"
#include "config.h"

#include <cassert>
#include <cmath>

#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#endif

std::vector<ClpString> CalypFrame::supportedColorSpacesListNames()
{
  return std::vector<ClpString>{
      "YUV",
      "RGB",
      "GRAY",
      "ARGB",
  };
}

std::vector<ClpString> CalypFrame::supportedPixelFormatListNames()
{
  std::vector<ClpString> formatsList;
  for( int i = 0; i < numberOfFormats(); i++ )
  {
    formatsList.push_back( g_CalypPixFmtDescriptorsMap.at( i ).name );
  }
  return formatsList;
}

std::vector<ClpString> CalypFrame::supportedPixelFormatListNames( int colorSpace )
{
  std::vector<ClpString> formatsList;
  for( int i = 0; i < numberOfFormats(); i++ )
  {
    if( g_CalypPixFmtDescriptorsMap.at( i ).colorSpace == colorSpace )
      formatsList.push_back( g_CalypPixFmtDescriptorsMap.at( i ).name );
  }
  return formatsList;
}

int CalypFrame::numberOfFormats()
{
  return g_CalypPixFmtDescriptorsMap.size();
}

int CalypFrame::findPixelFormat( const ClpString& name )
{
  for( int i = 0; i < numberOfFormats(); i++ )
  {
    if( g_CalypPixFmtDescriptorsMap.at( i ).name == name )
      return i;
  }
  return -1;
}

struct CalypFramePrivate
{
public:
  bool m_bInit;

  //! Struct with the pixel format description.
  const CalypPixelFormatDescriptor* m_pcPelFormat;
  ClpString m_cPelFmtName;

  unsigned int m_uiWidth;         //!< Width of the frame
  unsigned int m_uiHeight;        //!< Height of the frame
  int m_iPixelFormat;             //!< Pixel format number (it follows the list of supported pixel formats)
  unsigned int m_uiBitsPel;       //!< Bits per pixel/channel
  unsigned int m_uiHalfPelValue;  //!< Bits per pixel/channel

  ClpPel*** m_pppcInputPel;

  bool m_bHasRGBPel;          //!< Flag indicating that the ARGB buffer was computed
  unsigned char* m_pcARGB32;  //!< Buffer with the ARGB pixels used in Qt libs

  /** Histogram control variables **/
  bool m_bHasHistogram;
  bool m_bHistogramRunning;
  /** The histogram data.*/
  unsigned int* m_puiHistogram;
  /** If the image is RGB and calcLuma is true, we have 1 more channel */
  unsigned int m_uiHistoChannels;
  /** Numbers of histogram segments depending of image bytes depth*/
  unsigned int m_uiHistoSegments;

  /**
	 * Common constructor function of a frame
	 *
	 * @param width width of the frame
	 * @param height height of the frame
	 * @param pel_format pixel format index (always use PixelFormats enum)
	 *
	 */
  void init( unsigned int width, unsigned int height, int pel_format, int bitsPixel )
  {
    m_bInit = false;
    m_bHasRGBPel = false;
    m_pppcInputPel = NULL;
    m_pcARGB32 = NULL;
    m_uiWidth = width;
    m_uiHeight = height;
    m_iPixelFormat = pel_format;
    m_uiBitsPel = bitsPixel < 8 ? 8 : bitsPixel;
    m_uiHalfPelValue = 1 << ( m_uiBitsPel - 1 );

    if( m_uiWidth == 0 || m_uiHeight == 0 || m_iPixelFormat == -1 || bitsPixel > 16 )
    {
      throw CalypFailure( "CalypFrame", "Cannot create a PlYUVerFrame of this type" );
    }

    m_pcPelFormat = &( g_CalypPixFmtDescriptorsMap.at( pel_format ) );
    int iNumberChannels = m_pcPelFormat->numberChannels;

    std::size_t mem_size = 0;
    std::size_t num_of_ptrs = 0;
    for( int ch = 0; ch < iNumberChannels; ch++ )
    {
      int ratioW = ch > 0 ? m_pcPelFormat->log2ChromaWidth : 0;
      int ratioH = ch > 0 ? m_pcPelFormat->log2ChromaHeight : 0;
      // Add pointers mem
      num_of_ptrs += CHROMASHIFT( m_uiHeight, ratioH );
      // Add pixel mem
      mem_size += CHROMASHIFT( m_uiHeight, ratioH ) * CHROMASHIFT( m_uiWidth, ratioW ) * sizeof( ClpPel );
    }
    mem_size += num_of_ptrs * sizeof( ClpPel* ) + sizeof( ClpPel** ) * iNumberChannels;

    m_pppcInputPel = (ClpPel***)xMallocMem( mem_size );

    ClpPel** pelPtrMem = (ClpPel**)( m_pppcInputPel + iNumberChannels );
    ClpPel* pelMem = (ClpPel*)( pelPtrMem + num_of_ptrs );
    for( int ch = 0; ch < iNumberChannels; ch++ )
    {
      int ratioW = ch > 0 ? m_pcPelFormat->log2ChromaWidth : 0;
      int ratioH = ch > 0 ? m_pcPelFormat->log2ChromaHeight : 0;
      m_pppcInputPel[ch] = pelPtrMem;
      for( unsigned int h = 0; h < CHROMASHIFT( m_uiHeight, ratioH ); h++ )
      {
        *pelPtrMem = pelMem;
        pelPtrMem++;
        pelMem += CHROMASHIFT( m_uiWidth, ratioW );
      }
    }

    /* Alloc ARGB memory */
    getMem1D( &m_pcARGB32, m_uiHeight * m_uiWidth * 4 );

    m_puiHistogram = NULL;
    m_bHasHistogram = false;
    m_bHistogramRunning = false;

    m_uiHistoSegments = 1 << m_uiBitsPel;

    if( m_pcPelFormat->colorSpace == CLP_COLOR_RGB ||
        m_pcPelFormat->colorSpace == CLP_COLOR_RGBA )
      m_uiHistoChannels = m_pcPelFormat->numberChannels + 1;
    else
      m_uiHistoChannels = m_pcPelFormat->numberChannels;

    getMem1D<unsigned int>( &( m_puiHistogram ), m_uiHistoSegments * m_uiHistoChannels );

    m_cPelFmtName = CalypFrame::supportedPixelFormatListNames()[m_iPixelFormat].c_str();

    m_bInit = true;
  }

  int getRealHistogramChannel( int channel )
  {
    int realChannel = -1;
    if( channel == CalypFrame::HIST_ALL_CHANNELS )
    {
      realChannel = channel;
    }
    else if( channel >= 0 && channel < 4 )
    {
      realChannel = channel;
    }
    else if( m_pcPelFormat->colorSpace == CLP_COLOR_GRAY )
    {
      if( channel >= 10 && channel < 11 )
      {
        realChannel = channel - 10;
      }
    }
    else if( m_pcPelFormat->colorSpace == CLP_COLOR_YUV )
    {
      if( channel >= 10 && channel < 13 )
      {
        realChannel = channel - 10;
      }
    }
    else if( m_pcPelFormat->colorSpace == CLP_COLOR_RGB || m_pcPelFormat->colorSpace == CLP_COLOR_RGBA )
    {
      if( channel == CalypFrame::HIST_LUMA )
      {
        realChannel = m_uiHistoChannels - 1;
      }
      if( channel >= 20 && channel < 24 )
      {
        realChannel = channel - 20;
      }
    }
    if( realChannel > (int)m_uiHistoChannels )
      realChannel = -1;
    return realChannel;
  }

  ~CalypFramePrivate()
  {
    while( m_bHistogramRunning )
      ;
    if( m_puiHistogram )
      freeMem1D( m_puiHistogram );
    m_bHasHistogram = false;

    if( m_pppcInputPel )
      xFreeMem( m_pppcInputPel );

    if( m_pcARGB32 )
      freeMem1D( m_pcARGB32 );
  }
};

/*!
 * \brief Constructors
 */

CalypFrame::CalypFrame( unsigned int width, unsigned int height, int pelFormat, int bitsPixel )
    : d( new CalypFramePrivate )
{
  d->init( width, height, pelFormat, bitsPixel );
}

CalypFrame::CalypFrame( const CalypFrame& other )
    : d( new CalypFramePrivate )
{
  d->init( other.getWidth(), other.getHeight(), other.getPelFormat(), other.getBitsPel() );
  copyFrom( &other );
}

CalypFrame::CalypFrame( const CalypFrame* other )
    : d( new CalypFramePrivate )
{
  if( other )
  {
    d->init( other->getWidth(), other->getHeight(), other->getPelFormat(), other->getBitsPel() );
    copyFrom( other );
  }
}

CalypFrame::CalypFrame( const CalypFrame& other, unsigned int x, unsigned int y, unsigned int width, unsigned int height )
    : d( new CalypFramePrivate )
{
  const CalypPixelFormatDescriptor* pcPelFormat = &( g_CalypPixFmtDescriptorsMap.at( other.getPelFormat() ) );
  if( pcPelFormat->log2ChromaWidth )
  {
    if( x % ( 1 << pcPelFormat->log2ChromaWidth ) )
      x--;
    if( ( x + width ) % ( 1 << pcPelFormat->log2ChromaWidth ) )
      width++;
  }

  if( pcPelFormat->log2ChromaHeight )
  {
    if( y % ( 1 << pcPelFormat->log2ChromaHeight ) )
      y--;

    if( ( y + height ) % ( 1 << pcPelFormat->log2ChromaHeight ) )
      height++;
  }

  d->init( width, height, other.getPelFormat(), other.getBitsPel() );
  copyFrom( other, x, y );
}

CalypFrame::CalypFrame( const CalypFrame* other, unsigned int posX, unsigned int posY, unsigned int areaWidth, unsigned int areaHeight )
    : d( new CalypFramePrivate )
{
  if( !other )
    return;

  const CalypPixelFormatDescriptor* pcPelFormat = &( g_CalypPixFmtDescriptorsMap.at( other->getPelFormat() ) );
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

  d->init( areaWidth, areaHeight, other->getPelFormat(), other->getBitsPel() );
  copyFrom( other, posX, posY );
}

CalypFrame::~CalypFrame()
{
  delete d;
}

bool CalypFrame::haveSameFmt( const CalypFrame& other, unsigned int match ) const
{
  bool bRet = true;
  if( match & MATCH_COLOR_SPACE )
    bRet &= ( getColorSpace() == other.getColorSpace() );
  if( match & MATCH_RESOLUTION )
    bRet &= ( getWidth() == other.getWidth() ) && ( getHeight() == other.getHeight() );
  if( match & MATCH_PEL_FMT )
    bRet &= ( getPelFormat() == other.getPelFormat() );
  if( match & MATCH_BITS )
    bRet &= ( getBitsPel() == other.getBitsPel() );
  if( match & MATCH_COLOR_SPACE_IGNORE_GRAY )
    bRet &= ( getColorSpace() == CLP_COLOR_GRAY || getColorSpace() == other.getColorSpace() );
  return bRet;
}

bool CalypFrame::haveSameFmt( const CalypFrame* other, unsigned int match ) const
{
  bool bRet = false;
  if( other )
  {
    bRet = haveSameFmt( *other, match );
  }
  return bRet;
}

int CalypFrame::getPelFormat() const
{
  return d->m_iPixelFormat;
}

ClpString CalypFrame::getPelFmtName()
{
  return d->m_pcPelFormat->name;
}

int CalypFrame::getColorSpace() const
{
  return d->m_pcPelFormat->colorSpace;
}

unsigned int CalypFrame::getNumberChannels() const
{
  return d->m_pcPelFormat->numberChannels;
}

unsigned int CalypFrame::getWidth( int channel ) const
{
  return CHROMASHIFT( d->m_uiWidth, channel > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0 );
}
unsigned int CalypFrame::getHeight( int channel ) const
{
  return CHROMASHIFT( d->m_uiHeight, channel > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0 );
}

unsigned int CalypFrame::getPixels( int channel ) const
{
  return getWidth( channel ) * getWidth( channel );
}

unsigned char CalypFrame::getChromaWidthRatio() const
{
  return d->m_pcPelFormat->log2ChromaWidth;
}

unsigned char CalypFrame::getChromaHeightRatio() const
{
  return d->m_pcPelFormat->log2ChromaHeight;
}

unsigned int CalypFrame::getChromaLength() const
{
  return getWidth( 1 ) * getHeight( 1 );
}

unsigned int CalypFrame::getBitsPel() const
{
  return d->m_uiBitsPel;
}

unsigned long CalypFrame::getBytesPerFrame()
{
  return getBytesPerFrame( d->m_uiWidth, d->m_uiHeight, d->m_iPixelFormat, d->m_uiBitsPel );
}

unsigned long CalypFrame::getBytesPerFrame( unsigned int uiWidth, unsigned int uiHeight, int iPixelFormat, unsigned int bitsPixel )
{
  const CalypPixelFormatDescriptor* pcPelFormat = &( g_CalypPixFmtDescriptorsMap.at( iPixelFormat ) );
  unsigned int bytesPerPixel = ( bitsPixel - 1 ) / 8 + 1;
  unsigned long long int numberBytes = uiWidth * uiHeight;
  if( pcPelFormat->numberChannels > 1 )
  {
    unsigned long long int numberBytesChroma =
        CHROMASHIFT( uiWidth, pcPelFormat->log2ChromaWidth ) * CHROMASHIFT( uiHeight, pcPelFormat->log2ChromaHeight );
    numberBytes += ( pcPelFormat->numberChannels - 1 ) * numberBytesChroma;
  }
  return numberBytes * bytesPerPixel;
}

void CalypFrame::reset()
{
  ClpPel* pPel;
  ClpPel pelValue = 1 << ( d->m_uiBitsPel - 1 );
  int ratioH, ratioW;
  unsigned int i;
  for( unsigned int ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    pPel = d->m_pppcInputPel[ch][0];
    for( i = 0; i < CHROMASHIFT( d->m_uiHeight, ratioH ) * CHROMASHIFT( d->m_uiWidth, ratioW ); i++ )
    {
      *pPel++ = pelValue;
    }
  }
}

ClpPel*** CalypFrame::getPelBufferYUV() const
{
  return d->m_pppcInputPel;
}

ClpPel*** CalypFrame::getPelBufferYUV()
{
  d->m_bHasHistogram = false;
  d->m_bHasRGBPel = false;
  return d->m_pppcInputPel;
}

unsigned char* CalypFrame::getRGBBuffer() const
{
  if( d->m_bHasRGBPel )
  {
    return d->m_pcARGB32;
  }
  return NULL;
}

ClpPel CalypFrame::operator()( unsigned int ch, unsigned int xPos, unsigned int yPos )
{
  if( ch < d->m_pcPelFormat->numberChannels )
    return d->m_pppcInputPel[ch][yPos][xPos];
  return 0;
}

CalypPixel CalypFrame::operator()( unsigned int xPos, unsigned int yPos )
{
  return getPixel( xPos, yPos );
}

CalypPixel CalypFrame::getPixel( unsigned int xPos, unsigned int yPos )
{
  CalypPixel PixelValue( d->m_pcPelFormat->colorSpace );
  for( unsigned int ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    PixelValue[ch] = d->m_pppcInputPel[ch][( yPos >> ratioH )][( xPos >> ratioW )];
  }
  return PixelValue;
}

CalypPixel CalypFrame::getPixel( unsigned int xPos, unsigned int yPos, CalypColorSpace eColorSpace )
{
  return getPixel( xPos, yPos ).convertPixel( eColorSpace );
}

void CalypFrame::setPixel( unsigned int xPos, unsigned int yPos, CalypPixel pixel )
{
  for( unsigned int ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    d->m_pppcInputPel[ch][( yPos >> ratioH )][( xPos >> ratioW )] = pixel[ch];
  }
  d->m_bHasHistogram = false;
  d->m_bHasRGBPel = false;
}

void CalypFrame::copyFrom( const CalypFrame& other )
{
  if( haveSameFmt( other, MATCH_ALL ) )
    return;
  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;
  memcpy( *d->m_pppcInputPel[CLP_LUMA], other.getPelBufferYUV()[CLP_LUMA][0], getBytesPerFrame() * sizeof( ClpPel ) );
}

void CalypFrame::copyFrom( const CalypFrame* other )
{
  if( other )
    copyFrom( *other );
}

void CalypFrame::copyFrom( const CalypFrame& other, unsigned int xPos, unsigned int yPos )
{
  if( haveSameFmt( other, MATCH_ALL ) )
    return;
  ClpPel*** pInput = other.getPelBufferYUV();
  for( unsigned int ch = 0; ch < d->m_pcPelFormat->numberChannels; ch++ )
  {
    int ratioH = ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0;
    int ratioW = ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0;
    for( unsigned int i = 0; i < CHROMASHIFT( d->m_uiHeight, ratioH ); i++ )
    {
      memcpy( d->m_pppcInputPel[ch][i], &( pInput[ch][( yPos >> ratioH ) + i][( xPos >> ratioW )] ),
              ( d->m_uiWidth >> ratioW ) * sizeof( ClpPel ) );
    }
  }
  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;
}

void CalypFrame::copyFrom( const CalypFrame* other, unsigned int x, unsigned int y )
{
  if( other )
    copyFrom( *other, x, y );
}

void CalypFrame::frameFromBuffer( ClpByte* Buff, int iEndianness, unsigned long uiBuffSize )
{
  if( uiBuffSize != getBytesPerFrame() )
    return;

  frameFromBuffer( Buff, iEndianness );
}

void CalypFrame::frameFromBuffer( ClpByte* Buff, int iEndianness )
{
  ClpByte* ppBuff[MAX_NUMBER_PLANES];
  ClpByte* pTmpBuff;
  ClpPel* pPel;
  unsigned int bytesPixel = ( d->m_uiBitsPel - 1 ) / 8 + 1;
  int ratioH, ratioW, step;
  unsigned int i, ch;
  int startByte = 0;
  int endByte = bytesPixel;
  int incByte = 1;
  int b;
  int maxval = pow( 2, d->m_uiBitsPel ) - 1;

  if( iEndianness == 0 )
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
        // Check max value and bound it to "maxval" to prevent segfault when
        // calculating histogram
        if( *pPel > maxval )
          *pPel = 0;
        // -----
      }
      pPel++;
      pTmpBuff += step;
    }
  }
  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;
}

void CalypFrame::frameToBuffer( ClpByte* output_buffer, int iEndianness )
{
  unsigned int bytesPixel = ( d->m_uiBitsPel - 1 ) / 8 + 1;
  ClpByte* ppBuff[MAX_NUMBER_PLANES];
  ClpByte* pTmpBuff;
  ClpPel* pTmpPel;
  int ratioH, ratioW, step;
  unsigned int i, ch;
  int startByte = 0;
  int endByte = bytesPixel;
  int incByte = 1;
  int b;

  if( iEndianness == 0 )
  {
    startByte = bytesPixel - 1;
    endByte = -1;
    incByte = -1;
  }

  ppBuff[0] = output_buffer;
  for( int i = 1; i < MAX_NUMBER_PLANES; i++ )
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

void CalypFrame::fillRGBBuffer()
{
#define PEL_ARGB( a, r, g, b ) ( ( a & 0xff ) << 24 ) | ( ( r & 0xff ) << 16 ) | ( ( g & 0xff ) << 8 ) | ( b & 0xff )
#define PEL_RGB( r, g, b ) PEL_ARGB( 0xffu, r, g, b )
#define CLAMP_YUV2RGB( X ) X = X < 0 ? 0 : X > 255 ? 255 : X;
#define YUV2RGB( iY, iU, iV, iR, iG, iB )                          \
  iR = iY + ( ( 1436 * ( iV - 128 ) ) >> 10 );                     \
  iG = iY - ( ( 352 * ( iU - 128 ) + 731 * ( iV - 128 ) ) >> 10 ); \
  iB = iY + ( ( 1812 * ( iU - 128 ) ) >> 10 );                     \
  CLAMP_YUV2RGB( iR )                                              \
  CLAMP_YUV2RGB( iG )                                              \
  CLAMP_YUV2RGB( iB )

  if( d->m_bHasRGBPel )
    return;
  int shiftBits = d->m_uiBitsPel - 8;
  unsigned int* pARGB = (unsigned int*)d->m_pcARGB32;
  if( d->m_pcPelFormat->colorSpace == CLP_COLOR_GRAY )
  {
    ClpPel* pY = d->m_pppcInputPel[CLP_LUMA][0];
    for( unsigned int i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
    {
      *pARGB++ = PEL_RGB( *pY, *pY, *pY );
      pY++;
    }
  }
  else if( d->m_pcPelFormat->colorSpace == CLP_COLOR_RGB )
  {
    ClpPel* pR = d->m_pppcInputPel[CLP_COLOR_R][0];
    ClpPel* pG = d->m_pppcInputPel[CLP_COLOR_G][0];
    ClpPel* pB = d->m_pppcInputPel[CLP_COLOR_B][0];
    for( unsigned int i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
      *pARGB++ = PEL_RGB( ( *pR++ ) >> shiftBits, ( *pG++ ) >> shiftBits, ( *pB++ ) >> shiftBits );
  }
  else if( d->m_pcPelFormat->colorSpace == CLP_COLOR_RGBA )
  {
    ClpPel* pR = d->m_pppcInputPel[CLP_COLOR_R][0];
    ClpPel* pG = d->m_pppcInputPel[CLP_COLOR_G][0];
    ClpPel* pB = d->m_pppcInputPel[CLP_COLOR_B][0];
    ClpPel* pA = d->m_pppcInputPel[CLP_COLOR_A][0];
    for( unsigned int i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
      *pARGB++ = PEL_ARGB( ( *pA++ ) >> shiftBits, ( *pR++ ) >> shiftBits, ( *pG++ ) >> shiftBits, ( *pB++ ) >> shiftBits );
  }
  else if( d->m_pcPelFormat->colorSpace == CLP_COLOR_YUV )
  {
    ClpPel* pLineY = d->m_pppcInputPel[CLP_LUMA][0];
    ClpPel* pLineU = d->m_pppcInputPel[CLP_CHROMA_U][0];
    ClpPel* pLineV = d->m_pppcInputPel[CLP_CHROMA_V][0];
    unsigned int uiChromaStride = CHROMASHIFT( d->m_uiWidth, d->m_pcPelFormat->log2ChromaWidth );
    ClpPel* pY;
    ClpPel* pU;
    ClpPel* pV;
    int iY, iU, iV, iR, iG, iB;
    unsigned int* pARGBLine = pARGB;
    unsigned int* pARGB;

    unsigned int y, x;
    int i, j;
    for( y = 0; y < CHROMASHIFT( d->m_uiHeight, d->m_pcPelFormat->log2ChromaHeight ); y++ )
    {
      for( i = 0; i < 1 << d->m_pcPelFormat->log2ChromaHeight; i++ )
      {
        pY = pLineY;
        pU = pLineU;
        pV = pLineV;
        pARGB = pARGBLine;
        for( x = 0; x < CHROMASHIFT( d->m_uiWidth, d->m_pcPelFormat->log2ChromaWidth ); x++ )
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

void CalypFrame::calcHistogram()
{
  if( d->m_bHasHistogram || !d->m_puiHistogram )
    return;

  d->m_bHistogramRunning = true;

  xMemSet( unsigned int, d->m_uiHistoSegments * d->m_uiHistoChannels, d->m_puiHistogram );

  unsigned int numberChannels = d->m_pcPelFormat->numberChannels;
  for( unsigned int ch = 0; ch < numberChannels; ch++ )
  {
    unsigned int size = CHROMASHIFT( d->m_uiWidth, ch > 0 ? d->m_pcPelFormat->log2ChromaWidth : 0 ) *
                        CHROMASHIFT( d->m_uiHeight, ch > 0 ? d->m_pcPelFormat->log2ChromaHeight : 0 );

    ClpPel* chPel = &( d->m_pppcInputPel[ch][0][0] );
    for( unsigned int i = 0; i < size; i++ )
    {
      d->m_puiHistogram[*chPel + ch * d->m_uiHistoSegments]++;
      chPel++;
    }
  }

  if( d->m_pcPelFormat->colorSpace == CLP_COLOR_RGB ||
      d->m_pcPelFormat->colorSpace == CLP_COLOR_RGBA )
  {
    for( unsigned int y = 0; y < d->m_uiHeight; y++ )
      for( unsigned int x = 0; x < d->m_uiWidth; x++ )
      {
        ClpPel luma = getPixel( x, y ).convertPixel( CLP_COLOR_YUV )[0];
        d->m_puiHistogram[luma + ( d->m_uiHistoChannels - 1 ) * d->m_uiHistoSegments]++;
      }
  }
  d->m_bHasHistogram = true;
  d->m_bHistogramRunning = false;
}

int CalypFrame::getNumHistogramSegment()
{
  return d->m_uiHistoSegments;
}

unsigned int CalypFrame::getMinimumPelValue( int channel )
{
  if( !d->m_bHasHistogram )
    return 0;

  channel = d->getRealHistogramChannel( channel );
  if( channel < 0 )
    return 0;

  int indexStart;
  int indexEnd;
  if( channel == HIST_ALL_CHANNELS )
  {
    indexStart = 0;
    indexEnd = d->m_uiHistoChannels * d->m_uiHistoSegments;
  }
  else
  {
    indexStart = channel * d->m_uiHistoSegments;
    indexEnd = indexStart + d->m_uiHistoSegments;
  }

  for( int i = indexStart; i < indexEnd; i++ )
  {
    if( d->m_puiHistogram[i] > 0 )
    {
      return i - indexStart;
    }
  }
  return 0;
}

unsigned int CalypFrame::getMaximumPelValue( int channel )
{
  if( !d->m_bHasHistogram )
    return 0;

  channel = d->getRealHistogramChannel( channel );
  if( channel < 0 )
    return 0;

  int indexStart;
  int indexEnd;
  if( channel == HIST_ALL_CHANNELS )
  {
    indexStart = d->m_uiHistoChannels * d->m_uiHistoSegments;
    indexEnd = 0;
  }
  else
  {
    indexStart = ( channel + 1 ) * d->m_uiHistoSegments - 1;
    indexEnd = indexStart - d->m_uiHistoSegments;
  }

  for( int i = indexStart; i > indexEnd; i-- )
  {
    if( d->m_puiHistogram[i] > 0 )
    {
      return i - indexEnd + 1;
    }
  }
  return 0;
}

unsigned int CalypFrame::getMaximum( int channel )
{
  if( !d->m_bHasHistogram )
    return 0;

  channel = d->getRealHistogramChannel( channel );
  if( channel < 0 )
    return 0;

  unsigned int maxValue = 0;
  int indexStart;
  int indexEnd;
  if( channel == HIST_ALL_CHANNELS )
  {
    indexStart = 0;
    indexEnd = d->m_uiHistoChannels * d->m_uiHistoSegments;
  }
  else
  {
    indexStart = channel * d->m_uiHistoSegments;
    indexEnd = indexStart + d->m_uiHistoSegments;
  }

  for( int x = indexStart; x < indexEnd; x++ )
  {
    if( d->m_puiHistogram[x] > maxValue )
    {
      maxValue = d->m_puiHistogram[x];
    }
  }
  return maxValue;
}

unsigned int CalypFrame::getNumPixelsRange( int channel, unsigned int start, unsigned int end )
{
  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0;
  }
  channel = d->getRealHistogramChannel( channel );
  if( channel < 0 )
  {
    return 0;
  }

  unsigned int count = 0;
  int indexStart;
  indexStart = channel * d->m_uiHistoSegments;
  for( unsigned int i = start; i <= end; i++ )
  {
    count += d->m_puiHistogram[indexStart + i];
  }
  return count;
}

double CalypFrame::getMean( int channel, unsigned int start, unsigned int end )
{
  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }
  channel = d->getRealHistogramChannel( channel );
  if( channel < 0 )
  {
    return 0.0;
  }

  double mean = 0.0;
  double count;
  int indexStart = channel * d->m_uiHistoSegments;
  for( unsigned int i = start; i <= end; i++ )
  {
    mean += i * d->m_puiHistogram[indexStart + i];
  }

  count = getNumPixelsRange( channel, start, end );

  if( count > 0.0 )
  {
    return mean / count;
  }

  return mean;
}

int CalypFrame::getMedian( int channel, unsigned int start, unsigned int end )
{
  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0;
  }

  channel = d->getRealHistogramChannel( channel );
  if( channel < 0 )
  {
    return 0;
  }

  double sum = 0.0;
  int indexStart = channel * d->m_uiHistoSegments;
  double count = getNumPixelsRange( channel, start, end );
  for( unsigned int i = start; i <= end; i++ )
  {
    sum += d->m_puiHistogram[indexStart + i];
    if( sum * 2 > count )
      return i;
  }

  return 0;
}

double CalypFrame::getStdDev( int channel, unsigned int start, unsigned int end )
{
  if( !d->m_bHasHistogram || start < 0 || end > d->m_uiHistoSegments - 1 || start > end )
  {
    return 0.0;
  }

  channel = d->getRealHistogramChannel( channel );
  if( channel < 0 )
  {
    return 0.0;
  }

  int indexStart = channel * d->m_uiHistoSegments;
  double mean = getMean( channel, start, end );
  double count = getNumPixelsRange( channel, start, end );
  double dev = 0.0;
  if( count == 0.0 )
    count = 1.0;

  /*------------ original

	 for ( i = start ; i <= end ; i++ )
	 {
	 dev += ( i - mean ) * ( i - mean ) * d->m_puiHistogram[indexStart +i];
	 }

	 return sqrt( dev / count );

	 -----------------------*/

  for( unsigned int i = start; i <= end; i++ )
  {
    dev += ( i * i ) * d->m_puiHistogram[indexStart + i];
  }

  return sqrt( ( dev - count * mean * mean ) / ( count - 1 ) );
}

double CalypFrame::getHistogramValue( int channel, unsigned int bin )
{
  if( !d->m_bHasHistogram || bin < 0 || bin > d->m_uiHistoSegments - 1 )
    return 0.0;

  channel = d->getRealHistogramChannel( channel );
  if( channel < 0 )
  {
    return 0;
  }

  int indexStart = channel * d->m_uiHistoSegments;
  return d->m_puiHistogram[indexStart + bin];
}

/*
 **************************************************************
 * interface to other libs
 **************************************************************
 */

bool CalypFrame::toMat( cv::Mat& cvMat, bool convertToGray )
{
  bool bRet = false;
#ifdef USE_OPENCV
  if( convertToGray && !( d->m_pcPelFormat->colorSpace == CLP_COLOR_YUV ||
                          d->m_pcPelFormat->colorSpace == CLP_COLOR_GRAY ) )
  {
    return bRet;
  }
  int cvType = CV_MAKETYPE( CV_8U, convertToGray ? 1 : d->m_pcPelFormat->numberChannels );
  cvMat.create( d->m_uiHeight, d->m_uiWidth, cvType );
  unsigned char* pCvPel = cvMat.data;
  if( !convertToGray && ( d->m_pcPelFormat->colorSpace == CLP_COLOR_YUV ||
                          d->m_pcPelFormat->colorSpace == CLP_COLOR_RGB ) )
  {
    fillRGBBuffer();
    unsigned char* pARGB = d->m_pcARGB32;
    for( unsigned int i = 0; i < d->m_uiWidth * d->m_uiHeight; i++ )
    {
      *pCvPel++ = *pARGB++;
      *pCvPel++ = *pARGB++;
      *pCvPel++ = *pARGB++;
      pARGB++;
    }
    bRet = true;
  }
  else if( convertToGray || d->m_pcPelFormat->colorSpace == CLP_COLOR_GRAY )
  {
    ClpPel* pPel = d->m_pppcInputPel[CLP_LUMA][0];
    for( unsigned int i = 0; i < d->m_uiWidth * d->m_uiHeight; i++ )
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

bool CalypFrame::fromMat( cv::Mat& cvMat )
{
  bool bRet = false;
#ifdef USE_OPENCV
  if( !d->m_bInit )
  {
    if( d->m_iPixelFormat == CLP_INVALID_FMT )
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
    d->init( cvMat.cols, cvMat.rows, d->m_iPixelFormat, 8 );
  }

  d->m_bHasRGBPel = false;
  d->m_bHasHistogram = false;

  unsigned char* pCvPel = cvMat.data;
  if( d->m_iPixelFormat != CLP_GRAY )
  {
    ClpPel* pInputPelR = d->m_pppcInputPel[CLP_COLOR_R][0];
    ClpPel* pInputPelG = d->m_pppcInputPel[CLP_COLOR_G][0];
    ClpPel* pInputPelB = d->m_pppcInputPel[CLP_COLOR_B][0];
    for( unsigned int i = 0; i < d->m_uiHeight * d->m_uiWidth; i++ )
    {
      *pInputPelB++ = *pCvPel++;
      *pInputPelG++ = *pCvPel++;
      *pInputPelR++ = *pCvPel++;
    }
    bRet = true;
  }
  else
  {
    ClpPel* pPel = d->m_pppcInputPel[CLP_LUMA][0];
    for( unsigned int i = 0; i < d->m_uiWidth * d->m_uiHeight; i++ )
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

std::vector<ClpString> CalypFrame::supportedQualityMetricsList()
{
  return std::vector<ClpString>{
      "PSNR",
      "MSE",
      "SSIM",
  };
}

double CalypFrame::getQuality( int Metric, CalypFrame* Org, int component )
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

double CalypFrame::getMSE( CalypFrame* Org, int component )
{
  ClpPel* pPelYUV = getPelBufferYUV()[component][0];
  ClpPel* pOrgPelYUV = Org->getPelBufferYUV()[component][0];

  int aux_pel_1, aux_pel_2;
  int diff = 0;
  double ssd = 0;

  unsigned int numberOfPixels = 0;
  if( component == CLP_LUMA )
  {
    numberOfPixels = Org->getHeight() * Org->getWidth();
  }
  else
  {
    numberOfPixels = getChromaLength();
  }

  for( unsigned int i = 0; i < numberOfPixels; i++ )
  {
    aux_pel_1 = *pPelYUV++;
    aux_pel_2 = *pOrgPelYUV++;
    diff = aux_pel_1 - aux_pel_2;
    ssd += (double)( diff * diff );
  }
  if( ssd == 0.0 )
  {
    return 0.0;
  }
  return ssd / double( numberOfPixels );
}

double CalypFrame::getPSNR( CalypFrame* Org, int component )
{
  unsigned int uiMaxValue = ( 1 << Org->getBitsPel() ) - 1;
  double uiMaxValueSquare = uiMaxValue * uiMaxValue;
  double dPSNR = 100;
  double dMSE = getMSE( Org, component );
  if( dMSE != 0 )
    dPSNR = 10 * log10( uiMaxValueSquare / dMSE );
  return dPSNR;
}

float compute_ssim( ClpPel** refImg, ClpPel** encImg, int width, int height, int win_width, int win_height,
                    int max_pel_value_comp, int overlapSize )
{
  static const float K1 = 0.01f, K2 = 0.03f;
  float max_pix_value_sqd;
  float C1, C2;
  float win_pixels = (float)( win_width * win_height );
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

  max_pix_value_sqd = (float)( max_pel_value_comp * max_pel_value_comp );
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

      meanOrg = (float)imeanOrg / win_pixels;
      meanEnc = (float)imeanEnc / win_pixels;

      varOrg = ( (float)ivarOrg - ( (float)imeanOrg ) * meanOrg ) / win_pixels_bias;
      varEnc = ( (float)ivarEnc - ( (float)imeanEnc ) * meanEnc ) / win_pixels_bias;
      covOrgEnc = ( (float)icovOrgEnc - ( (float)imeanOrg ) * meanEnc ) / win_pixels_bias;

      mb_ssim = (float)( ( 2.0 * meanOrg * meanEnc + C1 ) * ( 2.0 * covOrgEnc + C2 ) );
      mb_ssim /= (float)( meanOrg * meanOrg + meanEnc * meanEnc + C1 ) * ( varOrg + varEnc + C2 );

      cur_distortion += mb_ssim;
      win_cnt++;
    }
  }

  cur_distortion /= (float)win_cnt;

  if( cur_distortion >= 1.0 && cur_distortion < 1.01 )  // avoid float accuracy problem at very low QP(e.g.2)
    cur_distortion = 1.0;

  return cur_distortion;
}

double CalypFrame::getSSIM( CalypFrame* Org, int component )
{
  double dSSIM = 1;
  if( component == CLP_LUMA )
  {
    dSSIM = compute_ssim( d->m_pppcInputPel[component], Org->getPelBufferYUV()[component], d->m_uiWidth, d->m_uiHeight,
                          8, 8, 255, 8 );
  }
  else
  {
    dSSIM = compute_ssim( d->m_pppcInputPel[component], Org->getPelBufferYUV()[component], getWidth( component ),
                          getHeight( component ), 4, 4, 255, 4 );
  }
  return dSSIM;
}
