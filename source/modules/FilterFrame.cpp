/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by Luis Lucas      (luisfrlucas@gmail.com)
 *                           Joao Carreira   (jfmcarreira@gmail.com)
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
 * \file     FilterFrame.cpp
 * \brief    Filter frame module
 */

#include <cstdio>

#include "FilterFrame.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

PlaYUVerModuleDefinition FilterFrameDef = {
    FRAME_LEVEL_MODULE,             // Apply module to the frames or to the whole sequence.
                                    // Currently only support for frame
    "Filters",                      // Category (sub-menu)
    "Y_Filter",                     // Name
    "Filter Y matrix of YUV frame", // Description
    MODULE_REQUIRES_ONE_FRAME,      // Number of Frames required (ONE_FRAME, TWO_FRAMES, THREE_FRAMES)
    MODULE_REQUIRES_NOTHING,        // Module requirements (check PlaYUVerModulesIf.h).
                                    // Several requirements should be "or" between each others.
    APPLY_WHILE_PLAYING,            // Apply module while playing
};

FilterFrame::FilterFrame()
{
  setModuleDefinition( FilterFrameDef );
  m_pcFilteredFrame = NULL;
}

Void FilterFrame::create( PlaYUVerFrame* InputFrame )
{
  m_pcFilteredFrame = NULL;
  m_pcFilteredFrame = new PlaYUVerFrame( InputFrame->getWidth(), InputFrame->getHeight(), PlaYUVerFrame::GRAY );
}

PlaYUVerFrame* FilterFrame::process( PlaYUVerFrame* InputFrame )
{
  Pel*** pppOutputPelYUV = m_pcFilteredFrame->getPelBufferYUV();
  Pel*** pppInputPelYUV = InputFrame->getPelBufferYUV();
  memcpy( pppOutputPelYUV[LUMA][0], pppInputPelYUV[LUMA][0], m_pcFilteredFrame->getWidth() * m_pcFilteredFrame->getHeight() * sizeof(Pel) );
  return m_pcFilteredFrame;
}

Void FilterFrame::destroy()
{
  if( m_pcFilteredFrame )
    delete m_pcFilteredFrame;
  m_pcFilteredFrame = NULL;
}

}  // NAMESPACE

