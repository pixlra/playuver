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
 * \file     FramePropertiesDock.h
 * \brief    Main side bar definition
 */

#ifndef __FRAMEPROPERTIESDOCK_H__
#define __FRAMEPROPERTIESDOCK_H__

#include "HistogramWidget.h"
#include "PlaYUVerAppDefs.h"
#include "config.h"
#include "lib/PlaYUVerFrame.h"
#include "lib/PlaYUVerStream.h"

#include <QtWidgets>

class FramePropertiesDock : public QWidget
{
  Q_OBJECT
public:
  FramePropertiesDock( QWidget* parent, Bool* pbMainPlaySwitch );
  ~FramePropertiesDock();

  QSize sizeHint() const;

  Void setFrame( PlaYUVerFrame* m_pcFrame );
  Void setSelection( const QRect& selectionArea );
  Void reset();
  Void stopHistogram();

private:
  PlaYUVerFrame* m_pcFrame;
  PlaYUVerFrame* m_pcSelectedFrame;
  Int m_iLastFrameType;

  Bool* m_pbIsPlaying;

  QComboBox* channelCB;
  QComboBox* colorsCB;
  QComboBox* renderingCB;

  QPushButton* linHistoButton;
  QPushButton* logHistoButton;
  QPushButton* fullImageButton;
  QPushButton* selectionImageButton;

  QButtonGroup* scaleButtonGroup;
  QButtonGroup* renderingButtonGroup;

  QSpinBox* minInterv;
  QSpinBox* maxInterv;

  QLabel* labelPixelsValue;
  QLabel* labelCountValue;
  QLabel* labelRangeValue;
  QLabel* labelMeanValue;
  QLabel* labelStdDevValue;
  QLabel* labelMedianValue;
  QLabel* labelPercentileValue;

  QLabel* colorsLabel;

  QRect m_cSelectionArea;

  HistogramWidget* histogramWidget;

  Void updateStatistiques();
  Void updateDataHistogram();

private Q_SLOTS:
  void slotRefreshOptions( int range );
  void slotHistogramComputationFailed( void );
  void slotChannelChanged( int index );
  void slotScaleChanged( int scale );
  void slotColorsChanged( int index );
  void slotRenderingChanged( int rendering );
  void slotMinValueChanged( int );
  void slotMaxValueChanged( int );

  void slotUpdateInterval( int min, int max );
  void slotUpdateIntervRange( int range );
};

#endif  // __FRAMEPROPERTIESDOCK_H__
