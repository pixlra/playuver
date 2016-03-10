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
 * \file     FrameShift.cpp
 * \brief    Shift image
 */

#include "FrameShift.h"

namespace plaYUVer
{

FrameShift::FrameShift()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Utilities";
  m_pchModuleName = "FrameShift";
  m_pchModuleTooltip = "Shift frame";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_OPTIONS | MODULE_USES_KEYS;

  m_cModuleOptions.addOptions()/**/
  ( "ShiftHorizontal", m_iShiftHor, "Amount of pixels to shift in horizontal direction" )/**/
  ( "ShiftVertical", m_iShiftVer, "Amount of pixels to shift in vertical direction" );

  m_pcProcessedFrame = NULL;
  m_iShiftHor = 0;
  m_iShiftVer = 0;
}

Bool FrameShift::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_
  m_pcProcessedFrame = new PlaYUVerFrame( apcFrameList[0], false );

//  m_iShiftHor = ( ( m_iShiftHor + 1 ) >> 1 ) << 1;
//  m_iShiftVer = ( ( m_iShiftVer + 1 ) >> 1 ) << 1;
  return true;
}

PlaYUVerFrame* FrameShift::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  Pel* pPelInput;
  Pel* pPelOut;

  UInt xStartOut = m_iShiftHor >= 0 ? m_iShiftHor : 0;
  UInt xStartIn = m_iShiftHor >= 0 ? 0 : -m_iShiftHor;
  UInt xEndOut = m_iShiftHor >= 0 ? m_pcProcessedFrame->getWidth() : m_pcProcessedFrame->getWidth() + m_iShiftHor;

  UInt yStartOut = m_iShiftVer >= 0 ? m_iShiftVer : 0;
  UInt yStartIn = m_iShiftVer >= 0 ? 0 : -m_iShiftVer;
  UInt yEndOut = m_iShiftVer >= 0 ? m_pcProcessedFrame->getHeight() : m_pcProcessedFrame->getHeight() + m_iShiftVer;

  m_pcProcessedFrame->clear();

  for( UInt y = yStartOut, yIn = yStartIn; y < yEndOut; y++, yIn++ )
  {
    pPelInput = &( apcFrameList[0]->getPelBufferYUV()[LUMA][yIn][xStartIn] );
    pPelOut = &( m_pcProcessedFrame->getPelBufferYUV()[LUMA][y][xStartOut] );
    for( UInt x = xStartOut; x < xEndOut; x++ )
    {
      *pPelOut = *pPelInput;
      pPelInput++;
      pPelOut++;
    }
  }

  Int iShiftHorChroma = CHROMASHIFT( m_iShiftHor, m_pcProcessedFrame->getChromaWidthRatio() );
  Int iShiftVerChroma = CHROMASHIFT( m_iShiftVer, m_pcProcessedFrame->getChromaHeightRatio() );

  xStartOut = iShiftHorChroma >= 0 ? iShiftHorChroma : 0;
  xStartIn = iShiftHorChroma >= 0 ? 0 : -iShiftHorChroma;
  xEndOut = iShiftHorChroma >= 0 ? m_pcProcessedFrame->getChromaWidth() : m_pcProcessedFrame->getChromaWidth() + iShiftHorChroma;

  yStartOut = iShiftVerChroma >= 0 ? iShiftVerChroma : 0;
  yStartIn = iShiftVerChroma >= 0 ? 0 : -iShiftVerChroma;
  yEndOut = iShiftVerChroma >= 0 ? m_pcProcessedFrame->getChromaHeight() : m_pcProcessedFrame->getChromaHeight() + iShiftVerChroma;


  for( UInt c = 1; c < m_pcProcessedFrame->getNumberChannels(); c++ )
  {
    for( UInt y = yStartOut, yIn = yStartIn; y < yEndOut; y++, yIn++ )
    {
      pPelInput = &( apcFrameList[0]->getPelBufferYUV()[c][yIn][xStartIn] );
      pPelOut = &( m_pcProcessedFrame->getPelBufferYUV()[c][y][xStartOut] );
      for( UInt x = xStartOut; x < xEndOut; x++ )
      {
        *pPelOut = *pPelInput;
        pPelInput++;
        pPelOut++;
      }
    }
  }
  return m_pcProcessedFrame;
}

Bool FrameShift::keyPressed( enum Module_Key_Supported value )
{
  if( value == MODULE_KEY_LEFT )
  {
    m_iShiftHor -= 1;
    return true;
  }
  if( value == MODULE_KEY_RIGHT )
  {
    m_iShiftHor += 1;
    return true;
  }
  if( value == MODULE_KEY_UP )
  {
    m_iShiftVer -= 1;
    return true;
  }
  if( value == MODULE_KEY_DOWN )
  {
    m_iShiftVer += 1;
    return true;
  }
  return false;
}

Void FrameShift::destroy()
{
  if( m_pcProcessedFrame )
    delete m_pcProcessedFrame;
  m_pcProcessedFrame = NULL;
}

}  // NAMESPACE

