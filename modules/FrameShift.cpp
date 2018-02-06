/*    This file is a part of PlaYUVer project
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
 * \file     FrameShift.cpp
 * \brief    Shift image
 */

#include "FrameShift.h"

FrameShift::FrameShift()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Utilities";
  m_pchModuleLongName = "Pixel Shift";
  m_pchModuleName = "FrameShift";
  m_pchModuleTooltip = "Shift frame horizontal and vertical";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_OPTIONS | MODULE_USES_KEYS;

  m_cModuleOptions.addOptions()                                                               /**/
      ( "ShiftHorizontal", m_iShiftHor, "Amount of pixels to shift in horizontal direction" ) /**/
      ( "ShiftVertical", m_iShiftVer, "Amount of pixels to shift in vertical direction" );

  m_pcProcessedFrame = NULL;
  m_iShiftHor = 0;
  m_iShiftVer = 0;
}

Bool FrameShift::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_
  m_pcProcessedFrame = new PlaYUVerFrame( apcFrameList[0] );
  return true;
}

PlaYUVerFrame* FrameShift::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  Pel* pPelInput;
  Pel* pPelOut;

  m_pcProcessedFrame->clear();

  for( UInt ch = 0; ch < m_pcProcessedFrame->getNumberChannels(); ch++ )
  {
    UInt uiWidth = m_pcProcessedFrame->getWidth( ch );
    UInt uiHeight = m_pcProcessedFrame->getHeight( ch );

    Int iShiftHor = m_iShiftHor >> ( ch > 0 ? m_pcProcessedFrame->getChromaWidthRatio() : 0 );
    Int iShiftVer = m_iShiftVer >> ( ch > 0 ? m_pcProcessedFrame->getChromaHeightRatio() : 0 );

    UInt xStartIn = iShiftHor >= 0 ? 0 : -iShiftHor;
    UInt yStartIn = iShiftVer >= 0 ? 0 : -iShiftVer;

    UInt xStartOut = std::max( iShiftHor, 0 );
    UInt xEndOut = std::min( uiWidth, uiWidth + iShiftHor );

    UInt yStartOut = std::max( iShiftVer, 0 );
    UInt yEndOut = std::min( uiHeight, uiHeight + iShiftVer );

    for( UInt y = yStartOut, yIn = yStartIn; y < yEndOut; y++, yIn++ )
    {
      pPelInput = &( apcFrameList[0]->getPelBufferYUV()[ch][yIn][xStartIn] );
      pPelOut = &( m_pcProcessedFrame->getPelBufferYUV()[ch][y][xStartOut] );
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
