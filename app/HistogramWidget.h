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
 * \file     HistogramWidget.h
 * \brief    Function to show image histogram
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

#ifndef __HISTOGRAMWIDGET_H__
#define __HISTOGRAMWIDGET_H__

#include "config.h"
#include "PlaYUVerAppDefs.h"
#include <QWidget>
#include <QColor>
#include "lib/PlaYUVerFrame.h"

class QEvent;

namespace plaYUVer
{

class HistogramWorker;
class HistogramWidgetPrivate;

class HistogramWidget: public QWidget
{
Q_OBJECT

public:

  enum HistogramChannel
  {
    /** Luminance */
    LumaHistogram = 0,
    /** Channels */
    FirstChannelHistogram,
    SecondChannelHistogram,
    ThirdChannelHistogram,
    /** Alpha channel */
    AlphaChannelHistogram,
    /** All color channels */
    ColorChannelsHistogram,
  };

  enum HistogramScale
  {
    /** Linear scale */
    LinScaleHistogram = 0,
    /** Logarithmic scale */
    LogScaleHistogram
  };

  enum HistogramAllColorMode
  {
    // Select wich component is shown in foreground
    FirstChannelColor,
    SecondChannelColor,
    ThirdChannelColor,
  };

  enum HistogramRendering
  {
    /** Full image histogram rendering */
    FullImageHistogram = 0,
    /** Image selection histogram rendering */
    ImageSelectionHistogram
  };

  enum HistogramOption
  {
    /** Blink the histogram area during histogram computation */
    BlinkComputation = 0x1,
    /** Show image statistics in tooltip */
    ShowStatistics = 0x2,
    /** It allows the user to select an area of the histograma */
    SelectMode = 0x4,
    /** Show the luminosity channel when applied */
    ShowLumaChannel = 0x8,
    /** Set all the options */
    AllOptions = BlinkComputation | ShowStatistics | SelectMode | ShowLumaChannel
  };Q_DECLARE_FLAGS( HistogramOptions, HistogramOption )

public  :

  /**
   * Constructor without image data. Needed to use @p updateData() method
   * after to create instance.
   * @param width of the widget
   * @param height of the widget
   * @param parent the histogram widget parent
   */
  HistogramWidget( Int width, Int height, QWidget *parent = 0 );

  ~HistogramWidget();

  /** Stop current histogram computations.*/
  Void stopHistogramComputation();

  /** Update full image histogram data. */
  Void updateData( PlaYUVerFrame *pcFrame, PlaYUVerFrame *pcFrameSelection );

  /** @see @p HistogramOption */
  Void setOptions( HistogramOptions options = AllOptions );
  Void setDataLoading();
  Void setLoadingSkipped();
  Void setLoadingFailed();
  Void setHistogramGuideByColor( QColor color );

  Void reset();

public:
  /** Channel type to draw */
  Int m_channelType;
  /** Scale to use for drawing */
  Int m_scaleType;
  /** Color to use for drawing in All Colors Channel mode */
  Int m_colorType;
  /** Using full image or image selection for histogram rendering */
  Int m_renderingType;

  /** Full image */
  HistogramWorker* m_imageWorker;
  //PlaYUVerFrameStats* m_imageHistogram;
  PlaYUVerFrame* m_fullImage;
  /** Histogram area selection */
  HistogramWorker* m_selectionWorker;
  //PlaYUVerFrameStats* m_selectionHistogram;
  PlaYUVerFrame* m_selectionImage;

  Q_SIGNALS:
  void signalIntervalChanged( int min, int max );
  void signalMaximumValueChanged( int );
  void signalHistogramComputationDone( int );
  void signalHistogramComputationFailed( void );

public Q_SLOTS:
  void slotMinValueChanged( int min );
  void slotMaxValueChanged( int max );

protected slots:
  void slotBlinkTimerDone( void );

protected:
  Void paintEvent( QPaintEvent * );
  Void mousePressEvent( QMouseEvent * e );
  Void mouseReleaseEvent( QMouseEvent * e );
  Void mouseMoveEvent( QMouseEvent * e );

private:
  Void customEvent( QEvent *event );
  Void notifyValuesChanged();

private:
  HistogramWidgetPrivate* d;
};

}
  // NAMESPACE

Q_DECLARE_OPERATORS_FOR_FLAGS( plaYUVer::HistogramWidget::HistogramOptions )

#endif // __HISTOGRAMWIDGET_H__
