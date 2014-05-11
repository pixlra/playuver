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
 * \file     FrameDifference.cpp
 * \brief    Frame Difference module
 */

#include <cstdio>

#include "FrameDifference.h"
#include "PlaYUVerFrame.h"

namespace plaYUVer
{

static PlaYUVerModuleDefinition FrameDifferenceDef =
{
    FRAME_LEVEL_MODULE,
    "Measurements",
    "FrameDifference",
    "Measure the difference between two images",
    true, };

FrameDifference::FrameDifference()
{
  m_pcFrameDifference = NULL;
  m_cModuleDef = FrameDifferenceDef;
}

Void FrameDifference::create( PlaYUVerFrame* Input )
{
  m_pcFrameDifference = NULL;
  m_pcFrameDifference = new PlaYUVerFrame( Input->getWidth(), Input->getHeight(), PlaYUVerFrame::GRAY );
}

PlaYUVerFrame* FrameDifference::process( PlaYUVerFrame* Input1 )
{
  return m_pcFrameDifference;
}

Void FrameDifference::destroy()
{
  if( m_pcFrameDifference )
    delete m_pcFrameDifference;
  m_pcFrameDifference = NULL;
}

}  // NAMESPACE

