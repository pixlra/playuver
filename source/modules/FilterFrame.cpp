/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by plaYUVer developers
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
    FRAME_LEVEL_MODULE,
    "Filters",
    "Y_Filter",
    "Filter Y matrix of YUV frame",
};

FilterFrame::FilterFrame()
{
  m_cModuleDef = FilterFrameDef;

  m_pcFilteredFrame = NULL;
}

Void FilterFrame::create( PlaYUVerFrame* InputFrame )
{
  if( m_pcFilteredFrame )
    delete m_pcFilteredFrame;
  m_pcFilteredFrame = new PlaYUVerFrame( InputFrame->getWidth(), InputFrame->getHeight(), InputFrame->getPelFormat() );
}

PlaYUVerFrame* FilterFrame::process( PlaYUVerFrame* InputFrame )
{
  Pel*** pppPelYUV = m_pcFilteredFrame->getPelBufferYUV();

  m_pcFilteredFrame->CopyFrom( InputFrame );

  for( UInt y = 0; y < m_pcFilteredFrame->getHeight() / 2; y++ )
  {
    for( UInt x = 0; x < m_pcFilteredFrame->getWidth() / 2; x++ )
    {
      pppPelYUV[1][y][x] = 128;
      pppPelYUV[2][y][x] = 128;
    }
  }
  //m_pcFilteredFrame->YUV420toRGB();
  return m_pcFilteredFrame;
}

Void FilterFrame::destroy()
{
  if( m_pcFilteredFrame )
    delete m_pcFilteredFrame;
  m_pcFilteredFrame = NULL;
}

}  // NAMESPACE

