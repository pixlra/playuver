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
 * \file     MeasureOpticalFlowDualTVL1.h
 * \brief    Absolute Frame Difference module
 */

#ifndef __MeasureOpticalFlowDualTVL1_H__
#define __MeasureOpticalFlowDualTVL1_H__

#include <opencv2/core/core.hpp>
#include <opencv2/video/video.hpp>

// PlaYUVerLib
#include "lib/PlaYUVerModuleIf.h"

class MeasureOpticalFlowDualTVL1 : public PlaYUVerModuleIf
{
  REGISTER_CLASS_FACTORY( MeasureOpticalFlowDualTVL1 )

 private:
  Bool m_bShowReconstruction;
  Int m_iStep;
  cv::Ptr<cv::DenseOpticalFlow> m_cTvl1;
  cv::Mat_<cv::Point2f> m_cvFlow;
  PlaYUVerFrame* m_pcFramePrev;
  PlaYUVerFrame* m_pcFrameAfter;
  PlaYUVerFrame* m_pcOutputFrame;

  Void drawFlow();
  Void compensateFlow();

 public:
  MeasureOpticalFlowDualTVL1();
  virtual ~MeasureOpticalFlowDualTVL1() {}
  Bool create( std::vector<PlaYUVerFrame*> apcFrameList );
  PlaYUVerFrame* process( std::vector<PlaYUVerFrame*> apcFrameList );
  Void destroy();
};

#endif  // __MeasureOpticalFlowDualTVL1_H__
