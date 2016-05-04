/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2016  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     DisparityStereoSGBM.h
 * \brief    Measure the disparity between two images using the SGBM method (OpenCV)
 *  The class implements the modified H. Hirschmuller algorithm
 *  that differs from the original one as follows:
 *  - By default, the algorithm is single-pass, which means that you consider only 5 directions
 *  instead of 8. Set mode=StereoSGBM::MODE_HH in createStereoSGBM to run the full variant of the
 *  algorithm but beware that it may consume a lot of memory.
 *  - The algorithm matches blocks, not individual pixels. Though, setting blockSize=1 reduces the
 *  blocks to single pixels.
 *  - Mutual information cost function is not implemented. Instead, a simpler Birchfield-Tomasi
 *  sub-pixel metric from @cite BT98 is used. Though, the color images are supported as well.
 *  - Some pre- and post- processing steps from K. Konolige algorithm StereoBM are included, for
 *  example: pre-filtering (StereoBM::PREFILTER_XSOBEL type) and post-filtering (uniqueness
 *  check, quadratic interpolation and speckle filtering).
 */

#ifndef __DISPARITYSTEREOSGBM_H__
#define __DISPARITYSTEREOSGBM_H__

#include <iostream>
#include "lib/PlaYUVerModuleIf.h"
#include <opencv2/opencv.hpp>

#include <cassert>

class DisparityStereoSGBM: public PlaYUVerModuleIf
{
REGISTER_CLASS_FACTORY( DisparityStereoSGBM )

private:
  PlaYUVerFrame* m_pcDisparityFrame;
  Bool m_bUseHH;
  Int m_uiNumberOfDisparities;
  UInt m_uiBlockSize;
  #if( CV_MAJOR_VERSION == 3)
  cv::Ptr<cv::StereoSGBM> m_cStereoMatch;
  #else
  cv::StereoSGBM m_cStereoMatch;
#endif

public:
  DisparityStereoSGBM();
  virtual ~DisparityStereoSGBM()
  {
  }

  Bool create( std::vector<PlaYUVerFrame*> apcFrameList );
  PlaYUVerFrame* process( std::vector<PlaYUVerFrame*> apcFrameList );
  Void destroy();
};

#endif // __DISPARITYSTEREOSGBM_H__

