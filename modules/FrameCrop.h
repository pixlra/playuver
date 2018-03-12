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
 * \file     FrameCrop.h
 * \brief    Crop a region of an image
 */

#ifndef __FRAMECROP_H__
#define __FRAMECROP_H__

// CalypLib
#include "lib/CalypModuleIf.h"

class FrameCrop : public CalypModuleIf
{
  REGISTER_CLASS_FACTORY( FrameCrop )

private:
  CalypFrame* m_pcCropedFrame;
  unsigned int m_uiXPosition;
  unsigned int m_uiYPosition;
  int m_iXSize;
  int m_iYSize;

public:
  FrameCrop();
  virtual ~FrameCrop() {}
  void create( CalypFrame* );
  CalypFrame* process( CalypFrame* );
  void destroy();
};

#endif  // __FRAMECROP_H__
