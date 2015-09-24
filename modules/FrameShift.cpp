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
  ( "ShiftHorizontal", m_iShiftHor, "Amount of pixels to shift in horizontal direction" );

  m_pcProcessedFrame = NULL;
  m_iShiftHor = 10;
}

Bool FrameShift::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_
  m_pcProcessedFrame = NULL;
  m_pcProcessedFrame = new PlaYUVerFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), apcFrameList[0]->getPelFormat(),
      apcFrameList[0]->getBitsPel() );
  return true;
}

PlaYUVerFrame* FrameShift::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  Pel* pPelInput;
  Pel* pPelOut;

  UInt xStartOut = m_iShiftHor >= 0 ? m_iShiftHor : 0;
  UInt xStartIn = m_iShiftHor >= 0 ? 0 : -m_iShiftHor;
  UInt xEndOut = m_iShiftHor >= 0 ? m_pcProcessedFrame->getWidth() : m_pcProcessedFrame->getWidth() + m_iShiftHor;

  m_pcProcessedFrame->clear();

  for( UInt y = 0; y < m_pcProcessedFrame->getHeight(); y++ )
  {
    pPelInput = &( apcFrameList[0]->getPelBufferYUV()[LUMA][y][xStartIn] );
    pPelOut = &( m_pcProcessedFrame->getPelBufferYUV()[LUMA][y][xStartOut] );
    for( UInt x = xStartOut; x < xEndOut; x++ )
    {
      *pPelOut = *pPelInput;
      pPelInput++;
      pPelOut++;
    }
  }

  Int iShiftHorChroma = CHROMASHIFT( m_iShiftHor, m_pcProcessedFrame->getChromaWidthRatio() );
  xStartOut = iShiftHorChroma >= 0 ? iShiftHorChroma : 0;
  xStartIn = iShiftHorChroma >= 0 ? 0 : -iShiftHorChroma;
  xEndOut = iShiftHorChroma >= 0 ? m_pcProcessedFrame->getChromaWidth() : m_pcProcessedFrame->getChromaWidth() + iShiftHorChroma;

  for( UInt c = 1; c < m_pcProcessedFrame->getNumberChannels(); c++ )
  {
    for( UInt y = 0; y < m_pcProcessedFrame->getChromaHeight(); y++ )
    {
      pPelInput = &( apcFrameList[0]->getPelBufferYUV()[c][y][xStartIn] );
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
    m_iShiftHor -= 2;
    return true;
  }
  if( value == MODULE_KEY_RIGHT )
  {
    m_iShiftHor += 2;
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

