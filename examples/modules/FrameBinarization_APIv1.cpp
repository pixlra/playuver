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
 * \file     FrameBinarization_APIv1.cpp
 * \brief    Binarize frame module (example of APIv1)
 */

#include "FrameBinarization_APIv1.h"

REGISTER_CLASS_MAKER( FrameBinarization_APIv1 )

FrameBinarization_APIv1::FrameBinarization_APIv1()
{
  /* Module Definition */
  m_iModuleType = CLP_FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Utilities";
  m_pchModuleName = "FrameBinarization_APIv1";
  m_pchModuleLongName = "Frame Binarization API v1";
  m_pchModuleTooltip = "Binarize frame";
  m_uiNumberOfFrames = 1;
  m_uiModuleRequirements = CLP_MODULE_REQUIRES_OPTIONS;

  m_cModuleOptions.addOptions() /**/
      ( "threshold", m_uiThreshold, "Threshold level for binarization (0-255) [128]" );

  m_pcBinFrame = NULL;
  m_uiThreshold = 128;
}

void FrameBinarization_APIv1::create( CalypFrame* frame )
{
  m_pcBinFrame = NULL;
  m_pcBinFrame = new CalypFrame( frame->getWidth(), frame->getHeight(), CalypFrame::GRAY, 8 );
}

CalypFrame* FrameBinarization_APIv1::process( CalypFrame* frame )
{
  const ClpPel* pPelInput = frame->getPelBufferYUV()[0][0];
  ClpPel* pPelBin = m_pcBinFrame->getPelBufferYUV()[0][0];
  for( unsigned int y = 0; y < frame->getHeight(); y++ )
    for( unsigned int x = 0; x < frame->getWidth(); x++ )
    {
      *pPelBin++ = *pPelInput++ >= m_uiThreshold ? 255 : 0;
    }
  return m_pcBinFrame;
}

void FrameBinarization_APIv1::destroy()
{
  if( m_pcBinFrame )
    delete m_pcBinFrame;
  m_pcBinFrame = NULL;
}
