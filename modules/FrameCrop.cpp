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
 * \file     FrameCrop.cpp
 * \brief    Crop a region of an image
 */

#include "FrameCrop.h"

FrameCrop::FrameCrop()
{
  /* Module Definition */
  m_iModuleType = FRAME_PROCESSING_MODULE;
  m_pchModuleCategory = "Utilities";
  m_pchModuleLongName = "Crop";
  m_pchModuleName = "FrameCrop";
  m_pchModuleTooltip = "Crop a region of a frame";
  m_uiNumberOfFrames = MODULE_REQUIRES_ONE_FRAME;
  m_uiModuleRequirements = MODULE_REQUIRES_OPTIONS;

  m_cModuleOptions.addOptions()                                                                   /**/
      ( "xPosition", m_uiXPosition, "X cordinate of the left-top corner of the crop region [0]" ) /**/
      ( "yPosition", m_uiYPosition, "Y cordinate of the left-top corner of the crop region [0]" ) /**/
      ( "width", m_iXSize, "Width of the crop region [-1]" )                                      /**/
      ( "height", m_iYSize, "Height of the crop region [-1]" );

  m_pcCropedFrame = NULL;
  m_uiXPosition = 0;
  m_uiYPosition = 0;
  m_iXSize = -1;
  m_iYSize = -1;
}

Void FrameCrop::create( PlaYUVerFrame* frame )
{
  m_pcCropedFrame = NULL;
  if( m_uiXPosition >= frame->getWidth() )
    m_uiXPosition = 0;
  if( m_uiYPosition >= frame->getHeight() )
    m_uiXPosition = 0;
  if( m_iXSize == -1 || ( m_uiXPosition + m_iXSize ) >= frame->getWidth() )
    m_iXSize = frame->getWidth() - m_uiXPosition;
  if( m_iYSize == -1 || ( m_uiYPosition + m_iYSize ) >= frame->getHeight() )
    m_iYSize = frame->getHeight() - m_uiYPosition;
  m_pcCropedFrame = new PlaYUVerFrame( m_iXSize, m_iYSize, frame->getPelFormat(), frame->getBitsPel() );
}

PlaYUVerFrame* FrameCrop::process( PlaYUVerFrame* frame )
{
  m_pcCropedFrame->copyFrom( frame, m_uiXPosition, m_uiYPosition );
  return m_pcCropedFrame;
}

Void FrameCrop::destroy()
{
  if( m_pcCropedFrame )
    delete m_pcCropedFrame;
  m_pcCropedFrame = NULL;
}
