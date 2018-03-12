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
 * \file     FramePropertiesDock.h
 * \brief    Main side bar definition
 */

#ifndef __FRAMEPROPERTIESDOCK_H__
#define __FRAMEPROPERTIESDOCK_H__

#include "CommonDefs.h"
#include "HistogramWidget.h"
#include "config.h"
#include "lib/CalypFrame.h"
#include "lib/CalypStream.h"

#include <QtWidgets>

class FramePropertiesDock : public QWidget
{
  Q_OBJECT
public:
  FramePropertiesDock( QWidget* parent, bool* pbMainPlaySwitch );
  ~FramePropertiesDock();

  QSize sizeHint() const;

  void setFrame( CalypFrame* m_pcFrame );
  void setSelection( const QRect& selectionArea );
  void reset();
  void stopHistogram();

private:
  CalypFrame* m_pcFrame;
  CalypFrame* m_pcSelectedFrame;
  int m_iLastFrameType;

  bool* m_pbIsPlaying;

  QComboBox* channelCB;
  QComboBox* colorsCB;
  QComboBox* renderingCB;

  QPushButton* linHistoButton;
  QPushButton* logHistoButton;
  QPushButton* fullImageButton;
  QPushButton* selectionImageButton;

  QButtonGroup* scaleButtonGroup;
  QButtonGroup* renderingButtonGroup;

  QSpinBox* mininterv;
  QSpinBox* maxinterv;

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

  void updateStatistiques();
  void updateDataHistogram();

private Q_SLOTS:
  void slotRefreshOptions( int range );
  void slotHistogramComputationFailed( void );
  void slotChannelChanged( int index );
  void slotScaleChanged( int scale );
  void slotColorsChanged( int index );
  void slotRenderingChanged( int rendering );
  void slotMinValueChanged( int );
  void slotMaxValueChanged( int );

  void slotUpdateinterval( int min, int max );
  void slotUpdateintervRange( int range );
};

#endif  // __FRAMEPROPERTIESDOCK_H__
