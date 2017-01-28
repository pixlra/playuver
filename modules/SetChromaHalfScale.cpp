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
 * \file     SetChromaHalfScale.cpp
 * \brief    Set chrome value to half scale (gray)
 */

#include "SetChromaHalfScale.h"

SetChromaHalfScale::SetChromaHalfScale()
{
  /* Module Definition */
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Conversions";
  m_pchModuleName = "SetChromaHalfScale";
  m_pchModuleTooltip = "Copy frame only keeping luma component";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_NOTHING;

  m_pcProcessedFrame = NULL;
}

Void SetChromaHalfScale::create( PlaYUVerFrame* frame )
{
  m_pcProcessedFrame = NULL;
  m_pcProcessedFrame =
      new PlaYUVerFrame( frame->getWidth(), frame->getHeight(), frame->getPelFormat(), frame->getBitsPel() );
}

PlaYUVerFrame* SetChromaHalfScale::process( PlaYUVerFrame* frame )
{
  Pel* pPelInput = frame->getPelBufferYUV()[LUMA][0];
  Pel* pPelOut = m_pcProcessedFrame->getPelBufferYUV()[0][0];
  Pel halfScaleValue = 1 << ( frame->getBitsPel() - 1 );
  for( UInt i = 0; i < frame->getHeight() * frame->getWidth(); i++ )
  {
    *pPelOut++ = *pPelInput++;
  }
  pPelInput = frame->getPelBufferYUV()[CHROMA_U][0];
  for( UInt i = 0; i < m_pcProcessedFrame->getChromaLength() * 2; i++ )
  {
    *pPelOut++ = halfScaleValue;
  }
  return m_pcProcessedFrame;
}

Void SetChromaHalfScale::destroy()
{
  if( m_pcProcessedFrame )
    delete m_pcProcessedFrame;
  m_pcProcessedFrame = NULL;
}
