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
 * \file     SaliencyDetection.h
 * \brief    Modules to detect saliency in image and videos
 */

#ifndef __SALIENCYDETECTION_H__
#define __SALIENCYDETECTION_H__

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/saliency.hpp>

// PlaYUVerLib
#include "lib/PlaYUVerModuleIf.h"

class SaliencyDetectionModule : public PlaYUVerModuleIf
{
protected:
  PlaYUVerFrame* m_pcSaliencyFrame;

  cv::Ptr<cv::saliency::Saliency> m_ptrSaliencyAlgorithm;
  cv::Mat m_matSaliency;

  Bool commonCreate( std::vector<PlaYUVerFrame*> apcFrameList );
  Bool commonProcess( std::vector<PlaYUVerFrame*> apcFrameList );

public:
  SaliencyDetectionModule();
  virtual ~SaliencyDetectionModule() {}
  Void destroy();
};

class SaliencyDetectionSpectral : public SaliencyDetectionModule
{
  REGISTER_CLASS_FACTORY( SaliencyDetectionSpectral )
private:
  Bool m_bBinaryMap;

public:
  SaliencyDetectionSpectral();
  virtual ~SaliencyDetectionSpectral() {}
  Bool create( std::vector<PlaYUVerFrame*> apcFrameList );
  PlaYUVerFrame* process( std::vector<PlaYUVerFrame*> apcFrameList );
};

class SaliencyDetectionFineGrained : public SaliencyDetectionModule
{
  REGISTER_CLASS_FACTORY( SaliencyDetectionFineGrained )
public:
  SaliencyDetectionFineGrained();
  virtual ~SaliencyDetectionFineGrained() {}
  Bool create( std::vector<PlaYUVerFrame*> apcFrameList );
  PlaYUVerFrame* process( std::vector<PlaYUVerFrame*> apcFrameList );
};

#endif  // __SALIENCYDETECTION_H__
