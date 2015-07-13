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
 * \file     DisparityStereoBM.h
 * \brief    Measure the disparity between two images using the Stereo Var method (OpenCV)
 */

#ifndef __DISPARITYSTEREOBM_H__
#define __DISPARITYSTEREOBM_H__

#include <iostream>
#include "PlaYUVerModuleIf.h"
#include <opencv2/opencv.hpp>

#include <cassert>

namespace plaYUVer
{

class DisparityStereoBM: public PlaYUVerModuleIf
{
REGISTER_CLASS_FACTORY( DisparityStereoBM )

private:
  PlaYUVerFrame* m_pcDisparityFrame;
  UInt m_uiBlockSize;
#if( CV_MAJOR_VERSION == 2)
  cv::StereoBM m_cStereoBM;
#else
  cv::Ptr<cv::StereoBM> m_cStereoBM;
#endif

public:
  DisparityStereoBM();
  virtual ~DisparityStereoBM()
  {
  }

  Bool create( std::vector<PlaYUVerFrame*> apcFrameList );
  PlaYUVerFrame* process( std::vector<PlaYUVerFrame*> apcFrameList );
  Void destroy();
};

}  // NAMESPACE

#endif // __DISPARITYSTEREOBM_H__

