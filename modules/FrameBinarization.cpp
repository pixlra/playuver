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
 * \file     FrameBinarization.cpp
 * \brief    Binarize frame module
 */

#include "FrameBinarization.h"

FrameBinarization::FrameBinarization()
{
  /* Module Definition */
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Utilities";
	m_pchModuleLongName = "Binarization";
  m_pchModuleName = "FrameBinarization";
  m_pchModuleTooltip = "Binarize frame";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_OPTIONS;

  m_cModuleOptions.addOptions() /**/
      ( "threshold", m_uiThreshold, "Threshold level for binarization (0-255) [128]" );

  m_pcBinFrame = NULL;
  m_uiThreshold = 128;
}

Void FrameBinarization::create( PlaYUVerFrame* frame )
{
  m_pcBinFrame = NULL;
  m_pcBinFrame = new PlaYUVerFrame( frame->getWidth(), frame->getHeight(), PlaYUVerFrame::GRAY, 8 );
}

PlaYUVerFrame* FrameBinarization::process( PlaYUVerFrame* frame )
{
  Pel* pPelInput = frame->getPelBufferYUV()[0][0];
  Pel* pPelBin = m_pcBinFrame->getPelBufferYUV()[0][0];
  for( UInt y = 0; y < frame->getHeight(); y++ )
    for( UInt x = 0; x < frame->getWidth(); x++ )
    {
      *pPelBin++ = *pPelInput++ >= m_uiThreshold ? 255 : 0;
    }
  return m_pcBinFrame;
}

Void FrameBinarization::destroy()
{
  if( m_pcBinFrame )
    delete m_pcBinFrame;
  m_pcBinFrame = NULL;
}
