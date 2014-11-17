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
 * \file     PropertiesSidebar.h
 * \brief    Main side bar definition
 */

#ifndef __PROPERTIESSIDEBAR_H__
#define __PROPERTIESSIDEBAR_H__

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#include <QWidget>
#endif
#include "lib/PlaYUVerStream.h"
#include "lib/PlaYUVerFrame.h"
#include "HistogramWidget.h"

namespace plaYUVer
{

class StreamPropertiesSideBar: public QWidget
{
Q_OBJECT
public:
  StreamPropertiesSideBar( QWidget* parent );
  ~StreamPropertiesSideBar();

  QSize sizeHint() const;

  Void setData( PlaYUVerStream* m_pcStream );

private:
  PlaYUVerStream* m_pcStream;

  Bool m_bIsVisible;
  QLabel* labelNameValue;
  QLabel* labelFormatValue;
  QLabel* labelCodecValue;
  QLabel* labelDurationValueTime;
  QLabel* labelDurationValueFrames;
  QLabel* labelSizeValue;
  QLabel* labelColorSpaceValue;
};

class FramePropertiesSideBar: public QWidget
{
Q_OBJECT
public:
  FramePropertiesSideBar( QWidget* parent, Bool *pbIsPlaying );
  ~FramePropertiesSideBar();

  QSize sizeHint() const;

  Void setData( PlaYUVerFrame* m_pcStream );
  Void setSelection( const QRect &selectionArea );

  Void stopHistogram();

private:
  enum ColorChannel
  {
    LuminosityChannel = 0,
    FirstChannel = 1,
    SecondChannel = 2,
    ThirdChannel = 3,
    ColorChannels = 4,
    AlphaChannel = 5,
  };

  enum AllColorsColorType
  {
    AllColorsRed = 0,
    AllColorsGreen,
    AllColorsBlue
  };

  PlaYUVerFrame *m_pcFrame;
  Bool* m_pbIsPlaying;
  Int m_iLastFrameType;

  QComboBox *channelCB;
  QComboBox *colorsCB;
  QComboBox *renderingCB;

  QPushButton *linHistoButton;
  QPushButton *logHistoButton;
  QPushButton *fullImageButton;
  QPushButton *selectionImageButton;

  QButtonGroup *scaleButtonGroup;
  QButtonGroup *renderingButtonGroup;

  QSpinBox *minInterv;
  QSpinBox *maxInterv;

  QLabel *colorsLabel;
  QLabel *labelMeanValue;
  QLabel *labelPixelsValue;
  QLabel *labelStdDevValue;
  QLabel *labelCountValue;
  QLabel *labelMedianValue;
  QLabel *labelPercentileValue;
  QLabel *formatValueLabel;
  QLabel *sizeValueLabel;
  QLabel *colorSpaceValueLabel;
  QLabel *colorModeValueLabel;
  QLabel *depthValueLabel;
  QLabel *alphaValueLabel;

  QRect m_cSelectionArea;

  HistogramWidget *histogramWidget;

  Void updateStatistiques();
  Void updateDataHistogram();

private Q_SLOTS:
  void slotRefreshOptions( bool sixteenBit );
  void slotHistogramComputationFailed( void );
  void slotChannelChanged( int channel );
  void slotScaleChanged( int scale );
  void slotColorsChanged( int color );
  void slotRenderingChanged( int rendering );
  void slotMinValueChanged( int );
  void slotMaxValueChanged( int );

  void slotUpdateInterval( int min, int max );
  void slotUpdateIntervRange( int range );

};

}   // NAMESPACE

#endif // __PROPERTIESSIDEBAR_H__