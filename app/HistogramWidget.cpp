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
 * \file     HistogramWidget.cpp
 * \brief    Function to show image histogram
 *           Based on the work of Ricardo N. Rocha Sardo in SCode project
 */

//TODO:
//      - Optimizar paintEvent. Parece haver codigo redundante  !!
#include "config.h"
#include <cmath>
#include <QtGui>
#include <QtDebug>
#include "HistogramWidget.h"


class HistogramWorker: public QThread
{
private:
  QObject* m_parent;
  PlaYUVerFrame* m_pcFrame;

public:

  HistogramWorker( QObject* parent )
  {
    m_pcFrame = NULL;
    m_parent = parent;
  }

  Void setup( PlaYUVerFrame* frame )
  {
    m_pcFrame = frame;
    run();
  }
  Void run()
  {
    if( m_pcFrame && m_parent )
    {
      EventData *eventData = new EventData();
//      eventDm_imageHistogramata->starting = true;
//      eventData->success = false;
//      eventData->histogram = m_histogram;
//      QCoreApplication::postEvent( m_parent, eventData );

      eventData->starting = true;

      m_pcFrame->calcHistogram();

      eventData->starting = false;
//       eventData->success = m_pcFrame->getHasHistogram();
      eventData->success = true;
      eventData->frame = m_pcFrame;
      QCoreApplication::postEvent( m_parent, eventData );
    }
  }

  /**
   * Custom Event
   */
  class EventData: public QEvent
  {

  public:

    EventData() :
            QEvent( QEvent::User )
    {
      starting = false;
      success = false;
      frame = NULL;
    }

    bool starting;
    bool success;
    PlaYUVerFrame *frame;
  };

};

const QBrush g_eColorPallet[][3] =
{
  {
    Qt::darkGray,  // Luma
    Qt::blue,  // Cb
    Qt::red,  // Cr
  },
  {
    Qt::red,  // Channel Red
    Qt::green,  // Channel Green
    Qt::blue,  // Channel Blue
  }, };

class HistogramWidgetPrivate
{
public:

  enum RepaintType
  {
    HistogramNone = 0,        // No current histogram values calculation.
    HistogramDataLoading,     // The image is being loaded
    HistogramStarted,         // Histogram values calculation started.
    HistogramCompleted,       // Histogram values calculation completed.
    HistogramSkipped,         // Skip histogram while playing
    HistogramFailed           // Histogram values calculation failed.
  };

  HistogramWidgetPrivate()
  {
    blinkTimer = 0;
    imageBits = 8;
    inSelected = false;
    blinkFlag = false;
    clearFlag = HistogramNone;
    xmin = 0.0;
    xmax = 0.0;
    range = 255;
    guideVisible = false;
    inInitialRepaintWait = false;
  }

  // Current selection information.
  double xmin;
  double xminOrg;
  double xmax;
  Int range;
  Int clearFlag;          // Clear drawing zone with message.

  // Image informations
  Int imageChannels;
  Int imageColorSpace;
  Int imageBits;

  bool guideVisible;       // Display color guide.
  bool statisticsVisible;  // Display tooltip histogram statistics.
  bool inSelected;
  bool selectMode;         // If true, a part of the histogram can be
                           // selected !
  bool blinkFlag;
  bool blinkComputation;   // If true, a message will be displayed during
                           // histogram computation, else nothing (limit
                           // flicker effect in widget especially for small
                           // image/computation time).
  bool lumaChannel;        // Compute luminance channel if true.
  bool inInitialRepaintWait;

  QTimer *blinkTimer;

  QColor colorGuide;
};

////////////////////////////////////////////////////////////////////////////////
//                              Constructors
////////////////////////////////////////////////////////////////////////////////

//Constructor without image data
HistogramWidget::HistogramWidget( Int width, Int height, QWidget *parent ) :
        QWidget( parent )
{
  setAttribute( Qt::WA_DeleteOnClose );

  d = new HistogramWidgetPrivate;

//   m_channelType = LumaHistogram;
  m_channelType = FirstChannelHistogram;
  m_scaleType = LogScaleHistogram;
  m_colorType = FirstChannelColor;
  m_renderingType = FullImageHistogram;

  setOptions( HistogramWidget::BlinkComputation | HistogramWidget::SelectMode | HistogramWidget::ShowLumaChannel );

  setMouseTracking( true );
  setMinimumSize( width, height );

  d->blinkTimer = new QTimer( this );

  connect( d->blinkTimer, SIGNAL( timeout() ), this, SLOT( slotBlinkTimerDone() ) );

  m_fullImage = NULL;
  m_selectionImage = NULL;

  m_imageWorker = new HistogramWorker( this );
  m_selectionWorker = new HistogramWorker( this );

  update();
}

////////////////////////////////////////////////////////////////////////////////
//                               Destructor
////////////////////////////////////////////////////////////////////////////////
HistogramWidget::~HistogramWidget()
{
  d->blinkTimer->stop();

//  if( m_imageHistogram )
//    delete m_imageHistogram;
//
//  if( m_selectionHistogram )
//    delete m_selectionHistogram;

  delete m_imageWorker;
  delete m_selectionWorker;

  delete d;
}
////////////////////////////////////////////////////////////////////////////////
//                              Setup Functions
////////////////////////////////////////////////////////////////////////////////

Void HistogramWidget::setOptions( HistogramOptions options )
{
  d->statisticsVisible = ( bool )( options & ShowStatistics );
  d->selectMode = ( bool )( options & SelectMode );
  d->blinkComputation = ( bool )( options & BlinkComputation );
  d->lumaChannel = ( bool )( options & ShowLumaChannel );
}

Void HistogramWidget::setHistogramGuideByColor( QColor color )
{
  d->guideVisible = true;
  d->colorGuide = color;
  update();
}

Void HistogramWidget::reset()
{
  d->guideVisible = false;
  d->clearFlag = HistogramWidgetPrivate::HistogramNone;
  // Remove histogram data from memory.
  m_fullImage = NULL;
  m_selectionImage = NULL;
  update();
}

////////////////////////////////////////////////////////////////////////////////
//                          Custom Event Handler
////////////////////////////////////////////////////////////////////////////////

Void HistogramWidget::customEvent( QEvent *event )
{
  if( !event )
  {
    return;
  }

  HistogramWorker::EventData *ed = ( HistogramWorker::EventData* )event;

  if( !ed )
  {
    return;
  }

  if( ed->frame != m_fullImage && ed->frame != m_selectionImage )
  {
    return;
  }

  if( ed->starting )
  {
    setCursor( Qt::WaitCursor );
    d->clearFlag = HistogramWidgetPrivate::HistogramStarted;
    if( !d->inInitialRepaintWait )
    {
      if( d->clearFlag != HistogramWidgetPrivate::HistogramDataLoading )
      {
        // enter initial repaint wait, repaint only after waiting
        // a short time so that very fast computation does not create
        // flicker.
        d->inInitialRepaintWait = true;
        d->blinkTimer->start( 100 );
      }
      else
      {
        // after the initial repaint, we can repaint immediately
        update();
        d->blinkTimer->start( 200 );
      }
    }
  }
  else
  {
    if( ed->success )
    {
      // Repaint histogram
      d->clearFlag = HistogramWidgetPrivate::HistogramCompleted;
      d->blinkTimer->stop();
      d->inInitialRepaintWait = false;
      unsetCursor();

      // Send signals to refresh information if necessary.
      // The signals may trigger multiple repaints, aVoid this,
      // we repaint once afterwards.
      setUpdatesEnabled( false );

      notifyValuesChanged();
      emit signalHistogramComputationDone( d->range );

      setUpdatesEnabled( true );
      update();
    }
    else
    {
      d->clearFlag = HistogramWidgetPrivate::HistogramFailed;
      d->blinkTimer->stop();
      d->inInitialRepaintWait = false;
      update();
      unsetCursor();
      // Remove old histogram data from memory.
      m_fullImage = NULL;
      m_selectionImage = NULL;
//      if( m_imageHistogram )
//      {
//        delete m_imageHistogram;
//        m_imageHistogram = 0;
//      }
//      if( m_selectionHistogram )
//      {
//        delete m_selectionHistogram;
//        m_selectionHistogram = 0;
//      }
      emit signalHistogramComputationFailed();
    }
  }

  //delete ed;
}
////////////////////////////////////////////////////////////////////////////////
//                          Data Loading Functions
////////////////////////////////////////////////////////////////////////////////

Void HistogramWidget::setDataLoading()
{
  if( d->clearFlag != HistogramWidgetPrivate::HistogramDataLoading )
  {
    setCursor( Qt::WaitCursor );
    d->clearFlag = HistogramWidgetPrivate::HistogramDataLoading;
    // enter initial repaint wait, repaint only after waiting
    // a short time so that very fast computation does not create flicker
    d->inInitialRepaintWait = true;
    d->blinkTimer->start( 100 );
    update();
  }
}

Void HistogramWidget::setLoadingSkipped()
{
  d->clearFlag = HistogramWidgetPrivate::HistogramSkipped;
  d->blinkTimer->stop();
  d->inInitialRepaintWait = false;
  update();
  unsetCursor();
}

Void HistogramWidget::setLoadingFailed()
{
  d->clearFlag = HistogramWidgetPrivate::HistogramFailed;
  d->blinkTimer->stop();
  d->inInitialRepaintWait = false;
  update();
  unsetCursor();
}

////////////////////////////////////////////////////////////////////////////////

Void HistogramWidget::stopHistogramComputation()
{
//  if( m_imageHistogram )
//    m_imageHistogram->setRunningFlag( false );
//
//  if( m_selectionHistogram )
//    m_selectionHistogram->setRunningFlag( false );

  d->blinkTimer->stop();
}

////////////////////////////////////////////////////////////////////////////////
//                          Update Data Methods
////////////////////////////////////////////////////////////////////////////////

Void HistogramWidget::updateData( PlaYUVerFrame *pcFrame, PlaYUVerFrame *pcFrameSelection )
{
  d->imageBits = pcFrame->getBitsPel();
  d->imageColorSpace = pcFrame->getColorSpace();
  d->imageChannels = pcFrame->getNumberChannels();

  // We are deleting the histogram data,
  // so we must not use it to draw any more.
  d->clearFlag = HistogramWidgetPrivate::HistogramNone;

  // Do not using PlaYUVerFrameStatistics::getHistogramSegment()
  // method here because histogram hasn't yet been computed.

  d->range = ( 1 << d->imageBits ) - 1;
  emit signalMaximumValueChanged( d->range );

  m_fullImage = pcFrame;
  m_imageWorker->setup( pcFrame );

  m_selectionImage = pcFrameSelection;
  m_selectionWorker->setup( pcFrameSelection );

}

////////////////////////////////////////////////////////////////////////////////

Void HistogramWidget::slotBlinkTimerDone()
{
  d->blinkFlag = !d->blinkFlag;
  d->inInitialRepaintWait = false;
  update();
  d->blinkTimer->start( 200 );
}

////////////////////////////////////////////////////////////////////////////////
//                              Paint Event
////////////////////////////////////////////////////////////////////////////////

Void HistogramWidget::paintEvent( QPaintEvent * )
{
  int maxX = size().width() - 1;
  int maxY = size().height() - 1;

  // Widget is disabled, not initialized,
  // or loading, but no message shall be drawn:
  // Drawing grayed frame.
  if( !isEnabled() || d->clearFlag == HistogramWidgetPrivate::HistogramNone
      || ( !d->blinkComputation && ( d->clearFlag == HistogramWidgetPrivate::HistogramStarted || d->clearFlag == HistogramWidgetPrivate::HistogramDataLoading ) ) )
  {
    QPainter p1( this );

    p1.fillRect( 0, 0, maxX, maxY, palette().color( QPalette::Disabled, QPalette::Window ) );
    p1.setPen( QPen( palette().color( QPalette::Disabled, QPalette::WindowText ), 1, Qt::SolidLine ) );
    p1.drawRect( 0, 0, maxX, maxY );

    return;
  }
  // Image data is loading or histogram is being computed:
  // Draw message.
  else if( d->blinkComputation && ( d->clearFlag == HistogramWidgetPrivate::HistogramStarted || d->clearFlag == HistogramWidgetPrivate::HistogramDataLoading ) )
  {
    QPainter p1( this );

    p1.fillRect( 0, 0, maxX, maxY, Qt::white );

    if( d->blinkFlag )
      p1.setPen( Qt::green );
    else
      p1.setPen( Qt::darkGreen );

    if( d->clearFlag == HistogramWidgetPrivate::HistogramDataLoading )
      p1.drawText( 0, 0, maxX, maxY, Qt::AlignCenter, tr( "Loading image..." ) );
    else
      p1.drawText( 0, 0, maxX, maxY, Qt::AlignCenter, tr( "Histogram\ncalculation\nin progress..." ) );

    return;
  }
  // Histogram computation skipped:
  // Draw message.
  else if( d->clearFlag == HistogramWidgetPrivate::HistogramSkipped )
  {
    QPainter p1( this );
    p1.fillRect( 0, 0, maxX, maxY, Qt::white );
    p1.setPen( Qt::darkGreen );
    p1.drawText( 0, 0, maxX, maxY, Qt::AlignCenter, tr( "Histogram\ncalculation\nskipped." ) );
    //bitBlt(this, 0, 0, &pm);
    return;
  }
  // Histogram computation failed:
  // Draw message.
  else if( d->clearFlag == HistogramWidgetPrivate::HistogramFailed )
  {
    QPainter p1( this );
    p1.fillRect( 0, 0, maxX, maxY, Qt::white );
    p1.setPen( Qt::red );
    p1.drawText( 0, 0, maxX, maxY, Qt::AlignCenter, tr( "Histogram\ncalculation\nfailed." ) );
    //bitBlt(this, 0, 0, &pm);
    return;
  }

  int x, y;
  int yr, yg, yb;             // For all color channels.
  int wWidth = width() - 1;
  int wHeight = height() - 1;
  double max;
  PlaYUVerFrame *frame;

  if( m_renderingType == ImageSelectionHistogram && m_selectionImage )
    frame = m_selectionImage;
  else
    frame = m_fullImage;

  if( !frame )
    return;

  x = 0;
  y = 0;
  yr = 0;
  yg = 0;
  yb = 0;
  max = 0.0;

  switch( m_channelType )
  {
  case HistogramWidget::FirstChannelHistogram:      // Y channel.
    max = frame->getMaximum( LUMA );
    break;
  case HistogramWidget::SecondChannelHistogram:    // Cb channel.
    max = frame->getMaximum( CHROMA_U );
    break;
  case HistogramWidget::ThirdChannelHistogram:     // Cr channel.
    max = frame->getMaximum( CHROMA_V );
    break;
  case HistogramWidget::AlphaChannelHistogram:    // Alpha channel.
    max = frame->getMaximum( COLOR_A );
    break;

  case HistogramWidget::ColorChannelsHistogram:   // All color channels.
    max = qMax( qMax( frame->getMaximum( LUMA ), frame->getMaximum( CHROMA_U ) ), frame->getMaximum( CHROMA_V ) );
    break;
//   case HistogramWidget::LumaHistogram:            // Luminance.
//     max = frame->getMaximum( LUMA );
//     break;
  }

  switch( m_scaleType )
  {
  case HistogramWidget::LinScaleHistogram:
    break;

  case HistogramWidget::LogScaleHistogram:
    if( max > 0.0 )
      max = log( max );
    else
      max = 1.0;
    break;
  }

  // From Qt 4.0, QWidget automatically double-buffers its painting,
  // so there's no need to write double-buffering code in paintEvent()
  // to aVoid flicker.
  QPainter p1( this );
  Int pallet = d->imageColorSpace;

  // Drawing selection or all histogram values.

  for( x = 0; x < wWidth; x++ )
  {
    double value = 0.0;
    double value_r = 0.0, value_g = 0.0, value_b = 0.0;  // For all channels.
    int i, j;

    i = ( x * frame->getHistogramSegment() ) / wWidth;
    j = ( ( x + 1 ) * frame->getHistogramSegment() ) / wWidth;

    do
    {
      double v;
      double vr, vg, vb;     // For all color channels.

      v = 0.0;
      vr = 0.0;
      vg = 0.0;
      vb = 0.0;

      switch( m_channelType )
      {
      case HistogramWidget::FirstChannelHistogram:    // Y channel.
        v = frame->getHistogramValue( LUMA, i++ );
        break;
      case HistogramWidget::SecondChannelHistogram:  // Cb channel.
        v = frame->getHistogramValue( CHROMA_U, i++ );
        break;
      case HistogramWidget::ThirdChannelHistogram:   // Cr channel.
        v = frame->getHistogramValue( CHROMA_V, i++ );
        break;
      case HistogramWidget::AlphaChannelHistogram:  // Alpha channel.
        v = frame->getHistogramValue( COLOR_A, i++ );
        break;
      case HistogramWidget::ColorChannelsHistogram:  // All color.
        vr = frame->getHistogramValue( LUMA, i );
        vg = frame->getHistogramValue( CHROMA_U, i );
        vb = frame->getHistogramValue( CHROMA_V, i++ );
        break;
//       case HistogramWidget::LumaHistogram:           // Luminance.
//         v = frame->getHistogramValue( LUMA, i++ );
//         break;

      }

      if( m_channelType != HistogramWidget::ColorChannelsHistogram )
      {
        if( v > value )
          value = v;
      }
      else
      {
        if( vr > value_r )
          value_r = vr;
        if( vg > value_g )
          value_g = vg;
        if( vb > value_b )
          value_b = vb;
      }
    }
    while( i < j );

    if( m_channelType != HistogramWidget::ColorChannelsHistogram )
    {
      switch( m_scaleType )
      {
      case HistogramWidget::LinScaleHistogram:
        y = ( int )( ( wHeight * value ) / max );
        break;

      case HistogramWidget::LogScaleHistogram:
        if( value <= 0.0 )
          value = 1.0;
        y = ( int )( ( wHeight * log( value ) ) / max );
        break;

      default:
        y = 0;
        break;
      }
    }
    else
    {
      switch( m_scaleType )
      {
      case HistogramWidget::LinScaleHistogram:
        yr = ( int )( ( wHeight * value_r ) / max );
        yg = ( int )( ( wHeight * value_g ) / max );
        yb = ( int )( ( wHeight * value_b ) / max );
        break;

      case HistogramWidget::LogScaleHistogram:
        if( value_r <= 0.0 )
          value_r = 1.0;
        if( value_g <= 0.0 )
          value_g = 1.0;
        if( value_b <= 0.0 )
          value_b = 1.0;
        yr = ( int )( ( wHeight * log( value_r ) ) / max );
        yg = ( int )( ( wHeight * log( value_g ) ) / max );
        yb = ( int )( ( wHeight * log( value_b ) ) / max );
        break;

      default:
        yr = 0;
        yg = 0;
        yb = 0;
        break;
      }
    }

    // Drawing the histogram + selection or only the histogram.

    if( m_channelType != HistogramWidget::ColorChannelsHistogram )
    {
      if( d->selectMode == true )   // Selection mode enable ?
      {
        if( x >= ( int )( d->xmin * wWidth ) && x <= ( int )( d->xmax * wWidth ) )
        {
          p1.setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, 0 );
          p1.setPen( QPen( Qt::lightGray, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - y );
        }
        else
        {
          p1.setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - y );
          p1.setPen( QPen( Qt::white, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight - y, x, 0 );

          if( x == wWidth / 4 || x == wWidth / 2 || x == 3 * wWidth / 4 )
          {
            p1.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, 0 );
          }
        }
      }
      else
      {
        p1.setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
        p1.drawLine( x, wHeight, x, wHeight - y );
        p1.setPen( QPen( Qt::white, 1, Qt::SolidLine ) );
        p1.drawLine( x, wHeight - y, x, 0 );

        if( x == wWidth / 4 || x == wWidth / 2 || x == 3 * wWidth / 4 )
        {
          p1.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, 0 );
        }
      }
    }
    else    // All colors channel
    {
      if( d->selectMode == true )   // Histogram selection mode enable ?
      {
        if( x >= ( int )( d->xmin * wWidth ) && x <= ( int )( d->xmax * wWidth ) )
        {
          p1.setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, 0 );
          p1.setPen( QPen( Qt::lightGray, 1, Qt::SolidLine ) );

          // Witch color must be used on the foreground with all
          // colors channel mode?
          switch( m_colorType )
          {
          case HistogramWidget::FirstChannelColor:
            p1.drawLine( x, wHeight, x, wHeight - yr );
            break;

          case HistogramWidget::SecondChannelColor:
            p1.drawLine( x, wHeight, x, wHeight - yg );
            break;

          default:
            p1.drawLine( x, wHeight, x, wHeight - yb );
            break;
          }
        }
        else
        {
          // Which color must be used on the foreground with all
          // colors channel mode?
          switch( m_colorType )
          {
          case HistogramWidget::FirstChannelColor:
            p1.setPen( QPen( g_eColorPallet[pallet][SecondChannelColor], 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, wHeight - yg );
            p1.setPen( QPen( g_eColorPallet[pallet][ThirdChannelColor], 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, wHeight - yb );
            p1.setPen( QPen( g_eColorPallet[pallet][FirstChannelColor], 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, wHeight - yr );

            p1.setPen( QPen( Qt::white, 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight - qMax( qMax( yr, yg ), yb ), x, 0 );
            p1.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight - yg - 1, x, wHeight - yg );
            p1.drawLine( x, wHeight - yb - 1, x, wHeight - yb );
            p1.drawLine( x, wHeight - yr - 1, x, wHeight - yr );

            break;

          case HistogramWidget::SecondChannelColor:
            p1.setPen( QPen( g_eColorPallet[pallet][ThirdChannelColor], 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, wHeight - yb );
            p1.setPen( QPen( g_eColorPallet[pallet][FirstChannelColor], 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, wHeight - yr );
            p1.setPen( QPen( g_eColorPallet[pallet][SecondChannelColor], 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, wHeight - yg );

            p1.setPen( QPen( Qt::white, 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight - qMax( qMax( yr, yg ), yb ), x, 0 );
            p1.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight - yb - 1, x, wHeight - yb );
            p1.drawLine( x, wHeight - yr - 1, x, wHeight - yr );
            p1.drawLine( x, wHeight - yg - 1, x, wHeight - yg );

            break;

          default:
            p1.setPen( QPen( g_eColorPallet[pallet][FirstChannelColor], 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, wHeight - yr );
            p1.setPen( QPen( g_eColorPallet[pallet][SecondChannelColor], 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, wHeight - yg );
            p1.setPen( QPen( g_eColorPallet[pallet][ThirdChannelColor], 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, wHeight - yb );

            p1.setPen( QPen( Qt::white, 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight - qMax( qMax( yr, yg ), yb ), x, 0 );
            p1.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight - yr - 1, x, wHeight - yr );
            p1.drawLine( x, wHeight - yg - 1, x, wHeight - yg );
            p1.drawLine( x, wHeight - yb - 1, x, wHeight - yb );

            break;
          }

          if( x == wWidth / 4 || x == wWidth / 2 || x == 3 * wWidth / 4 )
          {
            p1.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
            p1.drawLine( x, wHeight, x, 0 );
          }
        }
      }
      else
      {
        // Which color must be used on the foreground with all
        // colors channel mode?
        switch( m_colorType )
        {
        case HistogramWidget::FirstChannelColor:
          p1.setPen( QPen( Qt::green, 1, Qt::SolidLine ) );
          p1.setPen( QPen( g_eColorPallet[pallet][SecondChannelColor], 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - yg );
          p1.setPen( QPen( Qt::blue, 1, Qt::SolidLine ) );
          p1.setPen( QPen( g_eColorPallet[pallet][ThirdChannelColor], 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - yb );
          p1.setPen( QPen( Qt::red, 1, Qt::SolidLine ) );
          p1.setPen( QPen( g_eColorPallet[pallet][FirstChannelColor], 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - yr );

          p1.setPen( QPen( Qt::white, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight - qMax( qMax( yr, yg ), yb ), x, 0 );
          p1.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight - yg - 1, x, wHeight - yg );
          p1.drawLine( x, wHeight - yb - 1, x, wHeight - yb );
          p1.drawLine( x, wHeight - yr - 1, x, wHeight - yr );

          break;

        case HistogramWidget::SecondChannelColor:
          p1.setPen( QPen( Qt::blue, 1, Qt::SolidLine ) );
          p1.setPen( QPen( g_eColorPallet[pallet][ThirdChannelColor], 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - yb );
          p1.setPen( QPen( Qt::red, 1, Qt::SolidLine ) );
          p1.setPen( QPen( g_eColorPallet[pallet][FirstChannelColor], 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - yr );
          p1.setPen( QPen( Qt::green, 1, Qt::SolidLine ) );
          p1.setPen( QPen( g_eColorPallet[pallet][SecondChannelColor], 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - yg );

          p1.setPen( QPen( Qt::white, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight - qMax( qMax( yr, yg ), yb ), x, 0 );
          p1.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight - yb - 1, x, wHeight - yb );
          p1.drawLine( x, wHeight - yr - 1, x, wHeight - yr );
          p1.drawLine( x, wHeight - yg - 1, x, wHeight - yg );

          break;

        default:
          p1.setPen( QPen( Qt::red, 1, Qt::SolidLine ) );
          p1.setPen( QPen( g_eColorPallet[pallet][FirstChannelColor], 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - yr );
          p1.setPen( QPen( Qt::green, 1, Qt::SolidLine ) );
          p1.setPen( QPen( g_eColorPallet[pallet][SecondChannelColor], 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - yg );
          p1.setPen( QPen( Qt::blue, 1, Qt::SolidLine ) );
          p1.setPen( QPen( g_eColorPallet[pallet][ThirdChannelColor], 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, wHeight - yb );

          p1.setPen( QPen( Qt::white, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight - qMax( qMax( yr, yg ), yb ), x, 0 );
          p1.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight - yr - 1, x, wHeight - yr );
          p1.drawLine( x, wHeight - yg - 1, x, wHeight - yg );
          p1.drawLine( x, wHeight - yb - 1, x, wHeight - yb );

          break;
        }

        if( x == wWidth / 4 || x == wWidth / 2 || x == 3 * wWidth / 4 )
        {
          p1.setPen( QPen( Qt::gray, 1, Qt::SolidLine ) );
          p1.drawLine( x, wHeight, x, 0 );
        }
      }
    }
  }

  // Drawing color guide.

  p1.setPen( QPen( Qt::red, 1, Qt::DotLine ) );
  int guidePos;

  if( d->guideVisible )
  {
    switch( m_channelType )
    {
    case HistogramWidget::FirstChannelHistogram:
      guidePos = d->colorGuide.red();
      break;

    case HistogramWidget::SecondChannelHistogram:
      guidePos = d->colorGuide.green();
      break;

    case HistogramWidget::ThirdChannelHistogram:
      guidePos = d->colorGuide.blue();
      break;

//     case HistogramWidget::LumaHistogram:
//       guidePos = qMax( qMax( d->colorGuide.red(), d->colorGuide.green() ), d->colorGuide.blue() );
//       break;

    default:                                     // Alpha.
      guidePos = -1;
      break;
    }

    if( guidePos != -1 )
    {
      int xGuide = ( guidePos * wWidth ) / frame->getHistogramSegment();
      p1.drawLine( xGuide, 0, xGuide, wHeight );

      QString string = tr( "x:%1" ).arg( guidePos );
      QFontMetrics fontMt( string );
      QRect rect = fontMt.boundingRect( 0, 0, wWidth, wHeight, 0, string );
      p1.setPen( QPen( Qt::red, 1, Qt::SolidLine ) );
      rect.moveTop( 1 );

      if( xGuide < wWidth / 2 )
      {
        rect.moveLeft( xGuide );
        p1.fillRect( rect, QBrush( QColor( 250, 250, 255 ) ) );
        p1.drawRect( rect );
        rect.moveLeft( xGuide + 3 );
        p1.drawText( rect, Qt::AlignLeft, string );
      }
      else
      {
        rect.moveRight( xGuide );
        p1.fillRect( rect, QBrush( QColor( 250, 250, 255 ) ) );
        p1.drawRect( rect );
        rect.moveRight( xGuide - 3 );
        p1.drawText( rect, Qt::AlignRight, string );
      }
    }
  }

  if( d->statisticsVisible )
  {
    QString tipText, value;
    QString cellBeg( "<tr><td><nobr><font size=-1>" );
    QString cellMid( "</font></nobr></td><td><nobr><font size=-1>" );
    QString cellEnd( "</font></nobr></td></tr>" );
    tipText = "<table cellspacing=0 cellpadding=0>";

    tipText += cellBeg + tr( "Mean:" ) + cellMid;
    double mean = frame->getMean( m_channelType, 0, frame->getHistogramSegment() - 1 );
    tipText += value.setNum( mean, 'f', 1 ) + cellEnd;

    tipText += cellBeg + tr( "Pixels:" ) + cellMid;
    double pixels = frame->getPixels();
    tipText += value.setNum( ( float )pixels, 'f', 0 ) + cellEnd;

    tipText += cellBeg + tr( "Std dev.:" ) + cellMid;
    double stddev = frame->getStdDev( m_channelType, 0, frame->getHistogramSegment() - 1 );
    tipText += value.setNum( stddev, 'f', 1 ) + cellEnd;

    tipText += cellBeg + tr( "Selected:" ) + cellMid;
    double counts = frame->getCount( m_channelType, 0, frame->getHistogramSegment() - 1 );
    tipText += value.setNum( ( float )counts, 'f', 0 ) + cellEnd;

    tipText += cellBeg + tr( "Median:" ) + cellMid;
    double median = frame->getMedian( m_channelType, 0, frame->getHistogramSegment() - 1 );
    tipText += value.setNum( median, 'f', 1 ) + cellEnd;

    tipText += cellBeg + tr( "Percent:" ) + cellMid;
    double percentile = ( pixels > 0 ? ( 100.0 * counts / pixels ) : 0.0 );
    tipText += value.setNum( percentile, 'f', 1 ) + cellEnd;

    tipText += "</table>";

    setToolTip( tipText );
  }

  // Drawing frame.

  p1.setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
  p1.drawRect( 0, 0, maxX, maxY );

  //bitBlt(this, 0, 0, &pm);
}

////////////////////////////////////////////////////////////////////////////////
//                              Mouse Events
////////////////////////////////////////////////////////////////////////////////

Void HistogramWidget::mousePressEvent( QMouseEvent * e )
{
  if( d->selectMode == true && d->clearFlag == HistogramWidgetPrivate::HistogramCompleted )
  {
    if( !d->inSelected )
    {
      d->inSelected = true;
      update();
    }

    d->xmin = ( ( double )e->pos().x() ) / ( ( double )width() );
    d->xminOrg = d->xmin;
    notifyValuesChanged();
    //emit signalValuesChanged( (int)(d->xmin * d->range),  );
    d->xmax = 0.0;
  }
}

Void HistogramWidget::mouseReleaseEvent( QMouseEvent * )
{
  if( d->selectMode == true && d->clearFlag == HistogramWidgetPrivate::HistogramCompleted )

  {
    d->inSelected = false;
    // Only single click without mouse move? Remove selection.
    if( d->xmax == 0.0 )
    {
      d->xmin = 0.0;
      //emit signalMinValueChanged( 0 );
      //emit signalMaxValueChanged( d->range );
      notifyValuesChanged();
      update();
    }
  }
}

Void HistogramWidget::mouseMoveEvent( QMouseEvent * e )
{
  if( d->selectMode == true && d->clearFlag == HistogramWidgetPrivate::HistogramCompleted )
  {
    setCursor( Qt::SizeHorCursor );

    if( d->inSelected )
    {
      double max = ( ( double )e->pos().x() ) / ( ( double )width() );
      //int max = (int)(e->pos().x()*((float)m_imageHistogram->getHistogramSegment()/(float)width()));

      if( max < d->xminOrg )
      {
        d->xmax = d->xminOrg;
        d->xmin = max;
        //emit signalMinValueChanged( (int)(d->xmin * d->range) );
      }
      else
      {
        d->xmin = d->xminOrg;
        d->xmax = max;
      }

      notifyValuesChanged();
      //emit signalMaxValueChanged( d->xmax == 0.0 ? d->range : (int)(d->xmax * d->range) );

      update();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

Void HistogramWidget::notifyValuesChanged()
{
  emit signalIntervalChanged( ( int )( d->xmin * d->range ), d->xmax == 0.0 ? d->range : ( int )( d->xmax * d->range ) );
}

Void HistogramWidget::slotMinValueChanged( int min )
{
  if( d->selectMode == true && d->clearFlag == HistogramWidgetPrivate::HistogramCompleted )
  {
    if( min == 0 && d->xmax == 1.0 )
    {
      // everything is selected means no selection
      d->xmin = 0.0;
      d->xmax = 0.0;
    }
    if( min >= 0 && min < d->range )
    {
      d->xmin = ( ( double )min ) / d->range;
    }
    update();
  }
}

Void HistogramWidget::slotMaxValueChanged( int max )
{
  if( d->selectMode == true && d->clearFlag == HistogramWidgetPrivate::HistogramCompleted )
  {
    if( d->xmin == 0.0 && max == d->range )
    {
      // everything is selected means no selection
      d->xmin = 0.0;
      d->xmax = 0.0;
    }
    else if( max > 0 && max <= d->range )
    {
      d->xmax = ( ( double )max ) / d->range;
    }
    update();
  }
}



