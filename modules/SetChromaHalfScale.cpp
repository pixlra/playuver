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
 * \file     SetChromaHalfScale.cpp
 * \brief    Set chrome value to half scale (gray)
 */

#include "SetChromaHalfScale.h"

SetChromaHalfScale::SetChromaHalfScale()
{
  /* Module Definition */
  m_iModuleType = CLP_FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Conversions";
  m_pchModuleName = "SetChromaHalfScale";
  m_pchModuleLongName = "Half scale chroma";
  m_pchModuleTooltip = "Copy frame only keeping luma component";
  m_uiNumberOfFrames = 1;
  m_uiModuleRequirements = CLP_MODULE_REQUIRES_NOTHING;

  m_pcProcessedFrame = NULL;
}

void SetChromaHalfScale::create( CalypFrame* frame )
{
  m_pcProcessedFrame = NULL;
  m_pcProcessedFrame =
      new CalypFrame( frame->getWidth(), frame->getHeight(), frame->getPelFormat(), frame->getBitsPel() );
}

CalypFrame* SetChromaHalfScale::process( CalypFrame* frame )
{
  ClpPel* pPelInput = frame->getPelBufferYUV()[CLP_LUMA][0];
  ClpPel* pPelOut = m_pcProcessedFrame->getPelBufferYUV()[0][0];
  ClpPel halfScaleValue = 1 << ( frame->getBitsPel() - 1 );
  for( unsigned int i = 0; i < frame->getHeight() * frame->getWidth(); i++ )
  {
    *pPelOut++ = *pPelInput++;
  }
  pPelInput = frame->getPelBufferYUV()[CLP_CHROMA_U][0];
  for( unsigned int i = 0; i < m_pcProcessedFrame->getChromaLength() * 2; i++ )
  {
    *pPelOut++ = halfScaleValue;
  }
  return m_pcProcessedFrame;
}

void SetChromaHalfScale::destroy()
{
  if( m_pcProcessedFrame )
    delete m_pcProcessedFrame;
  m_pcProcessedFrame = NULL;
}
