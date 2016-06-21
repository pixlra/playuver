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
 * \file     FramePropertiesDock.cpp
 * \brief    Main side bar definition
 */

#include <cmath>
#include <QtGui>
#include <QtDebug>
#include "FramePropertiesDock.h"

FramePropertiesDock::FramePropertiesDock( QWidget* parent, Bool* pbMainPlaySwitch ) :
        QWidget( parent ),
        m_pbIsPlaying( pbMainPlaySwitch )
{
  // -------------- Variables definition --------------
  m_pcFrame = NULL;
  m_pcSelectedFrame = NULL;
  m_iLastFrameType = -1;

  // Histogram area -----------------------------------------------------

  QLabel *channelLabel = new QLabel( tr( "Channel:" ) );
  channelLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  channelCB = new QComboBox;
  channelCB->addItem( tr( "Luminance" ) );
  channelCB->setItemIcon( 0, QIcon( ":/images/channel-luma.png" ) );

  channelCB->setWhatsThis( tr( "<p>Select here the histogram channel to display:<p>"
      "<b>Luminance</b>: Display luminance (perceived brightness).<p>"
      "<b>Red</b>: Display the red image channel.<p>"
      "<b>Green</b>: Display the green image channel.<p>"
      "<b>Blue</b>: Display the blue image channel.<p>"
      "<b>Alpha</b>: Display the alpha image channel. "
      "This channel corresponds to the transparency value and "
      "is supported by some image formats such as PNG or TIFF.<p>"
      "<b>Colors</b>: Display all color channel values at "
      "the same time." ) );

  QString scaleWhatsThis( tr( "<p>Select here the histogram scale.<p>"
      "If the image's maximal values are small, you can use the "
      "linear scale.<p>Logarithmic scale can be used when the maximal"
      " values are big; if it is used, all values (small and large) "
      "will be visible on the graph." ) );

  linHistoButton = new QPushButton;
  linHistoButton->setToolTip( tr( "Linear" ) );
  linHistoButton->setIcon( QIcon( ":/images/histogram-lin.png" ) );
  linHistoButton->setWhatsThis( scaleWhatsThis );
  linHistoButton->setCheckable( true );
  linHistoButton->setAutoExclusive( true );
  linHistoButton->setChecked( true );

  logHistoButton = new QPushButton;
  logHistoButton->setToolTip( tr( "Logarithmic" ) );
  logHistoButton->setIcon( QIcon( ":/images/histogram-log.png" ) );
  logHistoButton->setWhatsThis( scaleWhatsThis );
  logHistoButton->setCheckable( true );
  logHistoButton->setAutoExclusive( true );

  scaleButtonGroup = new QButtonGroup( this );
  scaleButtonGroup->addButton( linHistoButton, HistogramWidget::LinScaleHistogram );
  scaleButtonGroup->addButton( logHistoButton, HistogramWidget::LogScaleHistogram );

  QHBoxLayout *scaleLayout = new QHBoxLayout;
  scaleLayout->addWidget( linHistoButton );
  scaleLayout->addWidget( logHistoButton );

  colorsLabel = new QLabel( tr( "Colors:" ) );
  colorsLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  colorsCB = new QComboBox;
  colorsCB->hide();
  colorsLabel->hide();

  QString regionWhatsThis( tr( "<p>Select here from which region the histogram will be computed:"
      "<p><b>Full Image</b>: Compute histogram using the full image."
      "<p><b>Selection</b>: Compute histogram using the current image "
      "selection." ) );

  fullImageButton = new QPushButton;
  fullImageButton->setToolTip( tr( "Full Image" ) );
  fullImageButton->setIcon( QIcon( ":/images/image-full.png" ) );
  fullImageButton->setWhatsThis( regionWhatsThis );
  fullImageButton->setCheckable( true );
  fullImageButton->setAutoExclusive( true );
  fullImageButton->setChecked( true );

  selectionImageButton = new QPushButton;
  selectionImageButton->setToolTip( tr( "Selection" ) );
  selectionImageButton->setIcon( QIcon( ":/images/image-selection.png" ) );
  selectionImageButton->setWhatsThis( regionWhatsThis );
  selectionImageButton->setCheckable( true );
  selectionImageButton->setAutoExclusive( true );

  renderingButtonGroup = new QButtonGroup( this );
  renderingButtonGroup->addButton( fullImageButton, HistogramWidget::FullImageHistogram );
  renderingButtonGroup->addButton( selectionImageButton, HistogramWidget::ImageSelectionHistogram );

  QHBoxLayout *regionLayout = new QHBoxLayout;
  regionLayout->addWidget( fullImageButton );
  regionLayout->addWidget( selectionImageButton );

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget( channelLabel, 1, 0 );
  mainLayout->addWidget( channelCB, 1, 1 );
  mainLayout->addLayout( scaleLayout, 1, 3 );
  mainLayout->addWidget( colorsLabel, 2, 0 );
  mainLayout->addWidget( colorsCB, 2, 1 );
  mainLayout->addLayout( regionLayout, 2, 3 );
  mainLayout->setColumnStretch( 2, 10 );

  // -------------------------------------------------------------

  histogramWidget = new HistogramWidget( 100, 100 );
  histogramWidget->setWhatsThis( tr( "<p>This is the histogram drawing of "
      "the selected image channel" ) );

  QVBoxLayout *histogramLayout = new QVBoxLayout;
  histogramLayout->setSpacing( 1 );
  histogramLayout->addWidget( histogramWidget );

  mainLayout->addLayout( histogramLayout, 3, 0, 1, 4 );

  // -------------------------------------------------------------

  QLabel *rangeSelectionLabel = new QLabel( tr( "Range:" ) );
  rangeSelectionLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  minInterv = new QSpinBox;
  minInterv->setRange( 0, 255 );
  minInterv->setSingleStep( 1 );
  minInterv->setValue( 0 );
  minInterv->setWhatsThis( tr( "<p>Select here the minimal intensity "
      "value of the histogram selection." ) );

  maxInterv = new QSpinBox;
  maxInterv->setRange( 0, 255 );
  maxInterv->setSingleStep( 1 );
  maxInterv->setValue( 0 );
  minInterv->setWhatsThis( tr( "<p>Select here the maximal intensity value"
      " of the histogram selection." ) );

  QHBoxLayout *rangeLayout = new QHBoxLayout;
  rangeLayout->addWidget( rangeSelectionLabel );
  rangeLayout->addWidget( minInterv );
  rangeLayout->addWidget( maxInterv );

  mainLayout->addLayout( rangeLayout, 4, 0, 1, 4 );

  // ----------------- GUI definition -----------------

  QLabel *pixelsLabel = new QLabel( tr( "Pixels:" ) );
  pixelsLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelPixelsValue = new QLabel;
  labelPixelsValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *countLabel = new QLabel( tr( "Selected:" ) );
  countLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelCountValue = new QLabel;
  labelCountValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *rangeLabel = new QLabel( tr( "Range:" ) );
  rangeLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelRangeValue = new QLabel;
  labelRangeValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *meanLabel = new QLabel( tr( "Mean:" ) );
  meanLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelMeanValue = new QLabel;
  labelMeanValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *stdDevLabel = new QLabel( tr( "Std. deviation:" ) );
  stdDevLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelStdDevValue = new QLabel;
  labelStdDevValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *medianLabel = new QLabel( tr( "Median:" ) );
  medianLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelMedianValue = new QLabel;
  labelMedianValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *percentileLabel = new QLabel( tr( "Percentile:" ) );
  percentileLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelPercentileValue = new QLabel;
  labelPercentileValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  Int gridRow = 0;
  QGridLayout *statisticsLayout = new QGridLayout;
  statisticsLayout->addWidget( pixelsLabel, gridRow, 0 );
  statisticsLayout->addWidget( labelPixelsValue, gridRow++, 1 );
  statisticsLayout->addWidget( countLabel, gridRow, 0 );
  statisticsLayout->addWidget( labelCountValue, gridRow++, 1 );
  statisticsLayout->addWidget( rangeLabel, gridRow, 0 );
  statisticsLayout->addWidget( labelRangeValue, gridRow++, 1 );
  statisticsLayout->addWidget( meanLabel, gridRow, 0 );
  statisticsLayout->addWidget( labelMeanValue, gridRow++, 1 );
  statisticsLayout->addWidget( stdDevLabel, gridRow, 0 );
  statisticsLayout->addWidget( labelStdDevValue, gridRow++, 1 );
  statisticsLayout->addWidget( medianLabel, gridRow, 0 );
  statisticsLayout->addWidget( labelMedianValue, gridRow++, 1 );
//  statisticsLayout->addWidget( percentileLabel, 5, 0 );
//  statisticsLayout->addWidget( labelPercentileValue, 5, 1 );

  QGroupBox *statisticsGroup = new QGroupBox( tr( "Statistics" ) );
  statisticsGroup->setLayout( statisticsLayout );
  statisticsGroup->setWhatsThis( tr( "<p>Here you can see the statistical results calculated from the "
      "selected histogram part. These values are available for all "
      "channels." ) );

  mainLayout->addWidget( statisticsGroup, 5, 0, 1, 4 );

  mainLayout->setRowStretch( 8, 10 );
  setLayout( mainLayout );
  setEnabled( false );

  connect( channelCB, SIGNAL( activated(int) ), this, SLOT( slotChannelChanged(int) ) );
  connect( scaleButtonGroup, SIGNAL( buttonClicked(int) ), this, SLOT( slotScaleChanged(int) ) );
  connect( colorsCB, SIGNAL( activated(int) ), this, SLOT( slotColorsChanged(int) ) );
  connect( renderingButtonGroup, SIGNAL( buttonClicked(int) ), this, SLOT( slotRenderingChanged(int) ) );
  connect( histogramWidget, SIGNAL( signalIntervalChanged( int, int ) ), this, SLOT( slotUpdateInterval(int, int) ) );
  connect( histogramWidget, SIGNAL( signalMaximumValueChanged(int) ), this, SLOT( slotUpdateIntervRange(int) ) );
  connect( histogramWidget, SIGNAL( signalHistogramComputationDone(int) ), this, SLOT( slotRefreshOptions(int) ) );
  connect( histogramWidget, SIGNAL( signalHistogramComputationFailed(void) ), this, SLOT( slotHistogramComputationFailed(void) ) );
  connect( minInterv, SIGNAL( valueChanged (int) ), this, SLOT( slotMinValueChanged(int) ) );
  connect( maxInterv, SIGNAL( valueChanged (int) ), this, SLOT( slotMaxValueChanged(int) ) );

}

FramePropertiesDock::~FramePropertiesDock()
{
  // If there is a currently histogram computation when dialog is closed,
  // stop it before the image data are deleted automatically!
  histogramWidget->stopHistogramComputation();
  if( histogramWidget )
    delete histogramWidget;
}

QSize FramePropertiesDock::sizeHint() const
{
  QSize currSize = size();
  QSize bestSize( 180, currSize.height() );
  if( currSize.width() < bestSize.width() )
    return bestSize;
  return currSize;
}

Void FramePropertiesDock::reset()
{
  m_pcFrame = NULL;
  m_cSelectionArea = QRect();
  if( m_pcSelectedFrame )
  {
    delete m_pcSelectedFrame;
    m_pcSelectedFrame = NULL;
  }

  m_iLastFrameType = -1;

  labelMeanValue->clear();
  labelPixelsValue->clear();
  labelStdDevValue->clear();
  labelCountValue->clear();
  labelMedianValue->clear();
  labelPercentileValue->clear();

  // Remove the histogram data from memory
  histogramWidget->reset();

  setEnabled( false );
}

Void FramePropertiesDock::setFrame( PlaYUVerFrame* pcFrame )
{
  Int colorSpace = pcFrame->getColorSpace();
  if( m_iLastFrameType != colorSpace )
  {
    m_iLastFrameType = colorSpace;
    if( colorSpace == PlaYUVerPixel::COLOR_RGB || colorSpace == PlaYUVerPixel::COLOR_ARGB )
    {
      channelCB->clear();
      channelCB->clear();
      channelCB->insertItem( LuminosityChannel, QIcon( ":/images/channel-luma.png" ), "Luminance" );
      channelCB->insertItem( FirstChannel, QIcon( ":/images/channel-red.png" ), "Red" );
      channelCB->insertItem( SecondChannel, QIcon( ":/images/channel-green.png" ), "Green" );
      channelCB->insertItem( ThirdChannel, QIcon( ":/images/channel-blue.png" ), "Blue" );
      if( colorSpace == PlaYUVerPixel::COLOR_ARGB )
      {
        channelCB->insertItem( AlphaChannel, QIcon( ":/images/channel-alpha.png" ), "Alpha" );
      }
      channelCB->insertItem( ColorChannels, QIcon( ":/images/channel-all.png" ), "Colors" );

      colorsCB->clear();
      colorsCB->addItem( "Red" );
      colorsCB->addItem( "Green" );
      colorsCB->addItem( "Blue" );
      colorsCB->setEnabled( false );
      colorsCB->setWhatsThis( tr( "<p>Select here the main color displayed with Colors Channel mode:"
          "<p><b>Red</b>: Draw the Red image channel in the foreground.<p>"
          "<b>Green</b>: Draw the Green image channel in the foreground.<p>"
          "<b>Blue</b>: Draw the Blue image channel in the foreground.<p>" ) );
      colorsCB->show();
      colorsLabel->show();
    }
    else if( colorSpace == PlaYUVerPixel::COLOR_YUV )
    {
      channelCB->clear();
      channelCB->insertItem( FirstChannel, QIcon( ":/images/channel-luma.png" ), "Luminance" );
      channelCB->insertItem( SecondChannel, QIcon( ":/images/channel-red.png" ), "Chroma U" );
      channelCB->insertItem( ThirdChannel, QIcon( ":/images/channel-green.png" ), "Chroma V" );
      channelCB->insertItem( ColorChannels, QIcon( ":/images/channel-all.png" ), "All Channels" );

      colorsCB->clear();
      colorsCB->addItem( "Luminance" );
      colorsCB->addItem( "Chroma U" );
      colorsCB->addItem( "Chroma V" );
      colorsCB->setEnabled( false );
      colorsCB->setWhatsThis( tr( "<p>Select here the main color displayed with Colors Channel mode:"
          "<p><b>Luminance</b>: DraupdateDataw the Luminance channel in the foreground.<p>"
          "<b>Chroma U</b>: Draw the Chroma U channel in the foreground.<p>"
          "<b>Chroma V</b>: Draw the Chroma V channel in the foreground.<p>" ) );
      colorsCB->show();
      colorsLabel->show();
    }
    else
    {
      channelCB->clear();
      channelCB->addItem( tr( "Luminance" ) );
      channelCB->setItemIcon( 0, QIcon( ":/images/channel-luma.png" ) );
      colorsCB->hide();
      colorsLabel->hide();
    }
  }
  setEnabled( true );
  if( m_pcFrame || !( *m_pbIsPlaying ) )
  {
    m_pcFrame = pcFrame;
    updateDataHistogram();
  }
}

Void FramePropertiesDock::setSelection( const QRect &selectionArea )
{
  if( m_pcFrame )
  {
    Bool bSelectionChanged = !( selectionArea.size() == m_cSelectionArea.size() );
    m_cSelectionArea = selectionArea;
    if( selectionArea.isValid() )
    {
      histogramWidget->stopHistogramComputation();

      /*
       * Allocate a new image for the selection
       * or just copy information
       */
      if( bSelectionChanged )
      {
        if( m_pcSelectedFrame )
        {
          delete m_pcSelectedFrame;
        }
        m_pcSelectedFrame = new PlaYUVerFrame( m_pcFrame, selectionArea.x(), selectionArea.y(), selectionArea.width(), selectionArea.height() );
      }
      updateDataHistogram();
      selectionImageButton->click();
      slotRenderingChanged( HistogramWidget::ImageSelectionHistogram );
    }
    else
    {
      fullImageButton->hide();
      selectionImageButton->hide();
      slotRenderingChanged( HistogramWidget::FullImageHistogram );
    }
  }
}

Void FramePropertiesDock::updateDataHistogram()
{
  if( m_pcFrame && isVisible() )
  {
    if( !*m_pbIsPlaying )
    {
      if( m_pcSelectedFrame )
      {
        m_pcSelectedFrame->copyFrom( m_pcFrame, m_cSelectionArea.x(), m_cSelectionArea.y() );
        fullImageButton->show();
        selectionImageButton->show();
      }
      else
      {
        fullImageButton->hide();
        selectionImageButton->hide();
      }
      histogramWidget->updateData( m_pcFrame, m_pcSelectedFrame );
    }
    else
    {
      histogramWidget->setLoadingSkipped();
    }
    histogramWidget->update();
  }
  else
  {
    histogramWidget->setLoadingFailed();
    slotHistogramComputationFailed();
  }
}

Void FramePropertiesDock::updateStatistiques()
{
  QString value;

  Int min = minInterv->value();
  Int max = maxInterv->value();
  Int channel = histogramWidget->m_channelType;

  if( channel == HistogramWidget::ColorChannelsHistogram )
    channel = colorsCB->currentIndex();

  PlaYUVerFrame* frame;

  if( histogramWidget->m_renderingType == HistogramWidget::FullImageHistogram )
    frame = histogramWidget->m_fullImage;
  else
    frame = histogramWidget->m_selectionImage;

  if( frame )
  {

    QString rangeText = "[" + QString::number( frame->getMin( channel ) ) + ":" +  QString::number( frame->getMax( channel ) ) + "]";
    labelRangeValue->setText( rangeText );

    double mean = frame->getMean( channel, min, max );
    labelMeanValue->setText( value.setNum( mean, 'f', 1 ) );

    double pixels = frame->getPixels();
    labelPixelsValue->setText( value.setNum( ( float )pixels, 'f', 0 ) );

    double stddev = frame->getStdDev( channel, min, max );
    labelStdDevValue->setText( value.setNum( stddev, 'f', 1 ) );

    double counts = frame->getCount( channel, min, max );
    labelCountValue->setText( value.setNum( ( float )counts, 'f', 0 ) );

    double median = frame->getMedian( channel, min, max );
    labelMedianValue->setText( value.setNum( median, 'f', 1 ) );

    double percentile = ( pixels > 0 ? ( 100.0 * counts / pixels ) : 0.0 );
    labelPercentileValue->setText( value.setNum( percentile, 'f', 1 ) );
  }
}

Void FramePropertiesDock::stopHistogram()
{
  histogramWidget->stopHistogramComputation();
}

////////////////////////////////////////////////////////////////////////////////
//                                  SLOTS
////////////////////////////////////////////////////////////////////////////////

Void FramePropertiesDock::slotRefreshOptions( int range )
{
  if( !isVisible() )
    return;

  slotChannelChanged( channelCB->currentIndex() );

  if( linHistoButton->isChecked() )
    slotScaleChanged( HistogramWidget::LinScaleHistogram );
  else if( logHistoButton->isChecked() )
    slotScaleChanged( HistogramWidget::LogScaleHistogram );

  slotColorsChanged( colorsCB->currentIndex() );

  if( m_cSelectionArea.isValid() )
  {
    if( fullImageButton->isChecked() )
      slotRenderingChanged( HistogramWidget::FullImageHistogram );
    else if( selectionImageButton->isChecked() )
      slotRenderingChanged( HistogramWidget::ImageSelectionHistogram );
  }
}

Void FramePropertiesDock::slotHistogramComputationFailed()
{
  m_pcFrame = NULL;
}

Void FramePropertiesDock::slotChannelChanged( Int channel )
{
  if( m_pcFrame->getColorSpace() == PlaYUVerPixel::COLOR_YUV )
    channel += 1;
  if( channel == AlphaChannel && m_pcFrame->getColorSpace() != PlaYUVerPixel::COLOR_ARGB )
    channel = ColorChannels;

  switch( channel )
  {
  case FirstChannel:
    histogramWidget->m_channelType = HistogramWidget::FirstChannelHistogram;
    colorsCB->setEnabled( false );
    break;
  case SecondChannel:
    histogramWidget->m_channelType = HistogramWidget::SecondChannelHistogram;
    colorsCB->setEnabled( false );
    break;
  case ThirdChannel:
    histogramWidget->m_channelType = HistogramWidget::ThirdChannelHistogram;
    colorsCB->setEnabled( false );
    break;
  case AlphaChannel:
    histogramWidget->m_channelType = HistogramWidget::AlphaChannelHistogram;
    colorsCB->setEnabled( false );
    break;
  case ColorChannels:
    histogramWidget->m_channelType = HistogramWidget::ColorChannelsHistogram;
    colorsCB->setEnabled( true );
    break;
  default:          // Luminance.
    //histogramWidget->m_channelType = HistogramWidget::LumaHistogram;
    histogramWidget->m_channelType = HistogramWidget::FirstChannelHistogram;
    colorsCB->setEnabled( false );
    break;
  }
  histogramWidget->update();
  updateStatistiques();
}

Void FramePropertiesDock::slotScaleChanged( Int scale )
{
  histogramWidget->m_scaleType = scale;
  histogramWidget->update();
}

Void FramePropertiesDock::slotColorsChanged( Int color )
{
  switch( color )
  {
  case AllColorsGreen:
    histogramWidget->m_colorType = HistogramWidget::SecondChannelColor;
    break;
  case AllColorsBlue:
    histogramWidget->m_colorType = HistogramWidget::ThirdChannelColor;
    break;
  default:          // Red.
    histogramWidget->m_colorType = HistogramWidget::FirstChannelColor;
    break;
  }

  histogramWidget->update();
  updateStatistiques();
}

Void FramePropertiesDock::slotRenderingChanged( Int rendering )
{
  histogramWidget->m_renderingType = rendering;
  histogramWidget->update();
  updateStatistiques();
}

Void FramePropertiesDock::slotMinValueChanged( Int min )
{
  // Called when user changes values of spin box.
  // Communicate the change to histogram widget.

  // make the one control "push" the other
  if( min == maxInterv->value() + 1 )
    maxInterv->setValue( min );

  maxInterv->setMinimum( min - 1 );
  histogramWidget->slotMinValueChanged( min );
  updateStatistiques();
}

Void FramePropertiesDock::slotMaxValueChanged( Int max )
{
  if( max == minInterv->value() - 1 )
    minInterv->setValue( max );

  minInterv->setMaximum( max + 1 );
  histogramWidget->slotMaxValueChanged( max );
  updateStatistiques();
}

Void FramePropertiesDock::slotUpdateInterval( Int min, Int max )
{
  // Called when value is set from within histogram widget.
  // Block signals to prevent slotMinValueChanged and
  // slotMaxValueChanged being called.
  minInterv->blockSignals( true );
  minInterv->setMaximum( max + 1 );
  minInterv->setValue( min );
  minInterv->blockSignals( false );

  maxInterv->blockSignals( true );
  maxInterv->setMinimum( min - 1 );
  maxInterv->setValue( max );
  maxInterv->blockSignals( false );

  updateStatistiques();
}

Void FramePropertiesDock::slotUpdateIntervRange( Int range )
{
  maxInterv->blockSignals( true );
  maxInterv->setMaximum( range );
  maxInterv->blockSignals( false );
}

