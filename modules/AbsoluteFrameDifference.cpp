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
 * \file     AbsoluteFrameDifference.cpp
 * \brief    Absolute Frame Difference module
 */

#include "AbsoluteFrameDifference.h"

AbsoluteFrameDifference::AbsoluteFrameDifference()
{
  /* Module Definition */
  m_iModuleAPI = MODULE_API_2;  // Use API version 2 (recommended).
  // See this example for details on the functions prototype
  m_iModuleType = FRAME_PROCESSING_MODULE;      // Apply module to the frames or to
                                                // the whole sequence.
  m_pchModuleCategory = "Measurements";         // Category (sub-menu)
  m_pchModuleName = "AbsoluteFrameDifference";  // Name
  m_pchModuleTooltip =
      "Measure the absolute difference "  // Description
      "between two images (Y plane), e. g., abs( Y1 - Y2 )";
  m_uiNumberOfFrames = MODULE_REQUIRES_TWO_FRAMES;      // Number of Frames required
                                                        // (ONE_FRAME, TWO_FRAMES,
                                                        // THREE_FRAMES)
  m_uiModuleRequirements = MODULE_REQUIRES_NEW_WINDOW;  // Module requirements
                                                        // (check
                                                        // PlaYUVerModulesIf.h).
  // Several requirements should be "or" between each others.
  m_pcFrameDifference = NULL;
}

Bool AbsoluteFrameDifference::create( std::vector<PlaYUVerFrame*> apcFrameList )
{
  _BASIC_MODULE_API_2_CHECK_

  for( UInt i = 1; i < apcFrameList.size(); i++ )
    if( !apcFrameList[i]->haveSameFmt( apcFrameList[0],
                                       PlaYUVerFrame::MATCH_COLOR_SPACE | PlaYUVerFrame::MATCH_RESOLUTION | PlaYUVerFrame::MATCH_BITS ) )
      return false;

  m_pcFrameDifference = new PlaYUVerFrame( apcFrameList[0]->getWidth(), apcFrameList[0]->getHeight(), PlaYUVerFrame::GRAY );
  return true;
}

PlaYUVerFrame* AbsoluteFrameDifference::process( std::vector<PlaYUVerFrame*> apcFrameList )
{
  PlaYUVerFrame* Input1 = apcFrameList[0];
  PlaYUVerFrame* Input2 = apcFrameList[1];
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
