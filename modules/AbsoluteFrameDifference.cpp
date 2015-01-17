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
 * \file     AbsoluteFrameDifference.cpp
 * \brief    Absolute Frame Difference module
 */

#include "AbsoluteFrameDifference.h"

namespace plaYUVer
{

AbsoluteFrameDifference::AbsoluteFrameDifference()
{
  /* Module Definition */
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Measurements";
  m_pchModuleName = "AbsoluteFrameDifference";
  m_pchModuleTooltip = "Measure the absolute difference between two images (Y plane), e. g., abs( Y1 - Y2 )";
  m_uiModuleRequirements = MODULE_REQUIRES_NEW_WINDOW;
  m_uiNumberOfFrames = MODULE_REQUIRES_TWO_FRAMES;
  m_bApplyWhilePlaying = APPLY_WHILE_PLAYING;

  m_pcFrameDifference = NULL;
}

Void AbsoluteFrameDifference::create( PlaYUVerFrame* Input )
{
  m_pcFrameDifference = new PlaYUVerFrame( Input->getWidth(), Input->getHeight(), PlaYUVerFrame::GRAY );
}

PlaYUVerFrame* AbsoluteFrameDifference::process( PlaYUVerFrame* Input1, PlaYUVerFrame* Input2 )
{
  Pel* pInput1PelYUV = Input1->getPelBufferYUV()[0][0];
  Pel* pInput2PelYUV = Input2->getPelBufferYUV()[0][0];
  Pel* pOutputPelYUV = m_pcFrameDifference->getPelBufferYUV()[0][0];
  Int aux_pel_1, aux_pel_2;

  for( UInt y = 0; y < m_pcFrameDifference->getHeight(); y++ )
    for( UInt x = 0; x < m_pcFrameDifference->getWidth(); x++ )
    {
      aux_pel_1 = *pInput1PelYUV++;
      aux_pel_2 = *pInput2PelYUV++;
      *pOutputPelYUV++ = abs( aux_pel_1 - aux_pel_2 );
    }
  return m_pcFrameDifference;
}

Void AbsoluteFrameDifference::destroy()
{
  if( m_pcFrameDifference )
    delete m_pcFrameDifference;
  m_pcFrameDifference = NULL;
}

}  // NAMESPACE

