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
 * \file     PropertiesSidebar.cpp
 * \brief    Main side bar definition
 */

#include "config.h"
#include <cmath>
#include <QtGui>
#include <QtDebug>
#include "TypeDef.h"
#include "PropertiesSidebar.h"

namespace plaYUVer
{

static inline QSize bestSize( QSize currSize );

////////////////////////////////////////////////////////////////////////////////
//                       StreamPropertiesSideBar
////////////////////////////////////////////////////////////////////////////////

StreamPropertiesSideBar::StreamPropertiesSideBar( QWidget* parent ) :
        QWidget( parent )
{
  // -------------- Variables definition --------------
  m_pcStream = NULL;

  // ----------------- GUI definition -----------------

  QLabel *formatLabel = new QLabel( tr( "File Format:" ) );
  formatLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelFormatValue = new QLabel;
  labelFormatValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *codecLabel = new QLabel( tr( "Codec:" ) );
  codecLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelCodecValue = new QLabel;
  labelCodecValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *durationLabel = new QLabel( tr( "Duration:" ) );
  durationLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelDurationValueTime = new QLabel;
  labelDurationValueTime->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  labelDurationValueFrames = new QLabel;
  labelDurationValueFrames->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *sizeLabel = new QLabel( tr( "Resolution:" ) );
  sizeLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelSizeValue = new QLabel;
  labelSizeValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *colorSpaceLabel = new QLabel( tr( "Color Space:" ) );
  colorSpaceLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelColorSpaceValue = new QLabel;
  labelColorSpaceValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QGridLayout *propertiesLayout = new QGridLayout;
  Int layout_line = 0;
  propertiesLayout->addWidget( formatLabel, layout_line, 0 );
  propertiesLayout->addWidget( labelFormatValue, layout_line, 1 );
  layout_line++;
  propertiesLayout->addWidget( codecLabel, layout_line, 0 );
  propertiesLayout->addWidget( labelCodecValue, layout_line, 1 );
  layout_line++;
  propertiesLayout->addWidget( durationLabel, layout_line, 0 );
  propertiesLayout->addWidget( labelDurationValueTime, layout_line, 1 );
  layout_line++;
  propertiesLayout->addWidget( labelDurationValueFrames, layout_line, 1 );
  layout_line++;
  propertiesLayout->addWidget( sizeLabel, layout_line, 0 );
  propertiesLayout->addWidget( labelSizeValue, layout_line, 1 );
  layout_line++;
  propertiesLayout->addWidget( colorSpaceLabel, layout_line, 0 );
  propertiesLayout->addWidget( labelColorSpaceValue, layout_line, 1 );

  propertiesLayout->setRowStretch( 8, 10 );
  setLayout( propertiesLayout );

  //QGroupBox *propertiesGroup = new QGroupBox;
  //propertiesGroup->setLayout( propertiesLayout );
  //QGridLayout *mainLayout = new QGridLayout;
  //mainLayout->addWidget( propertiesGroup );
  //mainLayout->setRowStretch( 8, 10 );
  //setLayout( mainLayout );

  setEnabled( false );
}

StreamPropertiesSideBar::~StreamPropertiesSideBar()
{

}

QSize StreamPropertiesSideBar::sizeHint() const
{
  return bestSize( size() );
}

Void StreamPropertiesSideBar::setData( InputStream* pcStream )
{
  // Clear information.
  labelFormatValue->clear();
  labelCodecValue->clear();
  labelDurationValueTime->clear();
  labelDurationValueFrames->clear();
  labelSizeValue->clear();
  labelColorSpaceValue->clear();

  if( !pcStream || !pcStream->getCurrFrame() )
  {
    setEnabled( false );
    return;
  }
  else
  {
    m_pcStream = pcStream;
    setEnabled( true );

    QString value;
    labelFormatValue->setText( m_pcStream->getFormatName() );
    labelCodecValue->setText( m_pcStream->getCodecName() );
    Int duration[3];
    m_pcStream->getDuration( duration );
    value = QString( tr( "%1h:%2m:%3s" ) ).arg( duration[0] ).arg( duration[1] ).arg( duration[2] );
    labelDurationValueTime->setText( value );
    value = QString( tr( "%1 frms" ) ).arg( m_pcStream->getFrameNum() );
    labelDurationValueFrames->setText( value );
    UInt width = m_pcStream->getWidth();
    UInt height = m_pcStream->getHeight();
    UInt fps = m_pcStream->getFrameRate();
    value = QString( tr( "%1 x %2 @ %3 Hz" ) ).arg( width ).arg( height ).arg( fps );
    labelSizeValue->setText( value );
    value = QString( PlaYUVerFrame::supportedPixelFormatList().at( m_pcStream->getCurrFrame()->getPelFormat() ) );
    labelColorSpaceValue->setText( value );
  }
}

////////////////////////////////////////////////////////////////////////////////
//                FramePropertiesSideBar
////////////////////////////////////////////////////////////////////////////////

FramePropertiesSideBar::FramePropertiesSideBar( QWidget* parent ) :
        QWidget( parent )
{
  // -------------- Variables definition --------------
  m_pcFrame = NULL;

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
  colorsCB->addItem( tr( "Red" ) );
  colorsCB->addItem( tr( "Green" ) );
  colorsCB->addItem( tr( "Blue" ) );
  colorsCB->setEnabled( false );
  colorsCB->setWhatsThis( tr( "<p>Select here the main color displayed with Colors Channel mode:"
      "<p><b>Red</b>: Draw the red image channel in the foreground.<p>"
      "<b>Green</b>: Draw the green image channel in the foreground.<p>"
      "<b>Blue</b>: Draw the blue image channel in the foreground.<p>" ) );

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

  histogramWidget = new HistogramWidget( 256, 140 );
  histogramWidget->setOptions( HistogramWidget::BlinkComputation | HistogramWidget::SelectMode | HistogramWidget::ShowLumaChannel );
  histogramWidget->setWhatsThis( tr( "<p>This is the histogram drawing of "
      "the selected image channel" ) );

  QVBoxLayout *histogramLayout = new QVBoxLayout;
  histogramLayout->setSpacing( 1 );
  histogramLayout->addWidget( histogramWidget );

  mainLayout->addLayout( histogramLayout, 3, 0, 1, 4 );

  // -------------------------------------------------------------

  QLabel *rangeLabel = new QLabel( tr( "Range:" ) );
  rangeLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

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
  rangeLayout->addWidget( rangeLabel );
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

  QGridLayout *statisticsLayout = new QGridLayout;
  statisticsLayout->addWidget( pixelsLabel, 0, 0 );
  statisticsLayout->addWidget( labelPixelsValue, 0, 1 );
  statisticsLayout->addWidget( countLabel, 1, 0 );
  statisticsLayout->addWidget( labelCountValue, 1, 1 );
  statisticsLayout->addWidget( meanLabel, 2, 0 );
  statisticsLayout->addWidget( labelMeanValue, 2, 1 );
  statisticsLayout->addWidget( stdDevLabel, 3, 0 );
  statisticsLayout->addWidget( labelStdDevValue, 3, 1 );
  statisticsLayout->addWidget( medianLabel, 4, 0 );
  statisticsLayout->addWidget( labelMedianValue, 4, 1 );
  statisticsLayout->addWidget( percentileLabel, 5, 0 );
  statisticsLayout->addWidget( labelPercentileValue, 5, 1 );

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
  connect( histogramWidget, SIGNAL( signalHistogramComputationDone(bool) ), this, SLOT( slotRefreshOptions(bool) ) );
  connect( histogramWidget, SIGNAL( signalHistogramComputationFailed(void) ), this, SLOT( slotHistogramComputationFailed(void) ) );
  connect( minInterv, SIGNAL( valueChanged (int) ), this, SLOT( slotMinValueChanged(int) ) );
  connect( maxInterv, SIGNAL( valueChanged (int) ), this, SLOT( slotMaxValueChanged(int) ) );

}

FramePropertiesSideBar::~FramePropertiesSideBar()
{
  // If there is a currently histogram computation when dialog is closed,
  // stop it before the image data are deleted automatically!
  histogramWidget->stopHistogramComputation();

  if( histogramWidget )
    delete histogramWidget;

}

QSize FramePropertiesSideBar::sizeHint() const
{
  return bestSize( size() );
}

Void FramePropertiesSideBar::setData( PlaYUVerFrame* pcFrame )
{
  // Clear information.
  labelMeanValue->clear();
  labelPixelsValue->clear();
  labelStdDevValue->clear();
  labelCountValue->clear();
  labelMedianValue->clear();
  labelPercentileValue->clear();

  if( !pcFrame )
  {
    // Remove the histogram data from memory
    histogramWidget->reset();
    setEnabled( false );
    return;
  }
  else
  {
    Int pel_fmt = pcFrame->getPelFormat();
    if( ( PlaYUVerFrame::isRGBorYUVorGray( pel_fmt ) == PlaYUVerFrame::COLOR_RGB || PlaYUVerFrame::isRGBorYUVorGray( pel_fmt ) == PlaYUVerFrame::COLOR_ARGB ) )
    {
      channelCB->clear();
      channelCB->clear();
      channelCB->insertItem( LuminosityChannel, QIcon( ":/images/channel-luma.png" ), "Luminance" );
      channelCB->insertItem( RedChannel, QIcon( ":/images/channel-red.png" ), "Red" );
      channelCB->insertItem( GreenChannel, QIcon( ":/images/channel-green.png" ), "Green" );
      channelCB->insertItem( BlueChannel, QIcon( ":/images/channel-blue.png" ), "Blue" );
      if( PlaYUVerFrame::isRGBorYUVorGray( pel_fmt ) == PlaYUVerFrame::COLOR_ARGB )
      {
        channelCB->insertItem( AlphaChannel, QIcon( ":/images/channel-alpha.png" ), "Alpha" );
      }
      channelCB->insertItem( ColorChannels, QIcon( ":/images/channel-all.png" ), "Colors" );
      colorsCB->show();
      colorsLabel->show();
    }
    else if( PlaYUVerFrame::isRGBorYUVorGray( pel_fmt ) == PlaYUVerFrame::COLOR_YUV )
    {
      channelCB->clear();
      channelCB->insertItem( LuminosityChannel, QIcon( ":/images/channel-luma.png" ), "Luminance" );
      channelCB->insertItem( ChromaUhannel, QIcon( ":/images/channel-red.png" ), "Chroma U" );
      channelCB->insertItem( ChromaVhannel, QIcon( ":/images/channel-green.png" ), "Chroma V" );
      channelCB->insertItem( ColorChannels, QIcon( ":/images/channel-all.png" ), "All Channels" );
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

    setEnabled( true );

    m_pcFrame = pcFrame;

    if( m_pcFrame->isValid() )
    {
      // If a selection area is done in Image Editor and if the current
      // image is the same in Image Viewer, then compute too the histogram
      // for this selection.
      histogramWidget->updateData( *m_pcFrame, PlaYUVerFrame() );
      fullImageButton->hide();
      selectionImageButton->hide();
      updateInformations();
    }
    else
    {
      histogramWidget->setLoadingFailed();
      slotHistogramComputationFailed();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//                              Set Selection
////////////////////////////////////////////////////////////////////////////////

Void FramePropertiesSideBar::setSelection( const QRect &selectionArea )
{
  // This is necessary to stop computation because image.bits() is
  // currently used by threaded histogram algorithm.

  m_cSelectionArea = selectionArea;

  if( selectionArea.isValid() )
  {

    histogramWidget->stopHistogramComputation();

    m_pcFrame->CopyFrom( m_pcFrameSelection );
//         histogramWidget->updateSelectionData( imageSelection.bits(),
//                                                  imageSelection.width(),
//                                                  imageSelection.height(),
//                                                  imageSelection.bitsPerChannel(),
//                                                  imageSelection.colorSpace() );
    histogramWidget->updateSelectionData( *m_pcFrameSelection );
    fullImageButton->show();
    selectionImageButton->show();
    selectionImageButton->click();
  }
  else
  {
    fullImageButton->hide();
    selectionImageButton->hide();
    slotRenderingChanged( HistogramWidget::FullImageHistogram );
  }
}
////////////////////////////////////////////////////////////////////////////////
//                                  SLOTS
////////////////////////////////////////////////////////////////////////////////

void FramePropertiesSideBar::slotRefreshOptions( bool /*depth*/)
{
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

void FramePropertiesSideBar::slotHistogramComputationFailed()
{
  m_pcFrame = NULL;
  m_pcFrameSelection = NULL;
}

void FramePropertiesSideBar::slotChannelChanged( int channel )
{
  if( PlaYUVerFrame::isRGBorYUVorGray( m_pcFrame->getPelFormat() ) == PlaYUVerFrame::COLOR_YUV )
    channel += 1;
  if( channel == AlphaChannel && PlaYUVerFrame::isRGBorYUVorGray( m_pcFrame->getPelFormat() ) != PlaYUVerFrame::COLOR_ARGB )
    channel = ColorChannels;

  switch( channel )
  {
  case RedChannel:
    histogramWidget->m_channelType = HistogramWidget::RedChannelHistogram;
    colorsCB->setEnabled( false );
    break;
  case GreenChannel:
    histogramWidget->m_channelType = HistogramWidget::GreenChannelHistogram;
    colorsCB->setEnabled( false );
    break;
  case BlueChannel:
    histogramWidget->m_channelType = HistogramWidget::BlueChannelHistogram;
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
    histogramWidget->m_channelType = HistogramWidget::LumaHistogram;
    colorsCB->setEnabled( false );
    break;
  }
  histogramWidget->update();
  updateStatistiques();
}

void FramePropertiesSideBar::slotScaleChanged( int scale )
{
  histogramWidget->m_scaleType = scale;
  histogramWidget->update();
}

void FramePropertiesSideBar::slotColorsChanged( int color )
{
  switch( color )
  {
  case AllColorsGreen:
    histogramWidget->m_colorType = HistogramWidget::GreenColor;
    break;
  case AllColorsBlue:
    histogramWidget->m_colorType = HistogramWidget::BlueColor;
    break;
  default:          // Red.
    histogramWidget->m_colorType = HistogramWidget::RedColor;
    break;
  }

  histogramWidget->update();
  updateStatistiques();
}

void FramePropertiesSideBar::slotRenderingChanged( int rendering )
{
  histogramWidget->m_renderingType = rendering;
  histogramWidget->update();
  updateStatistiques();
}

void FramePropertiesSideBar::slotMinValueChanged( int min )
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

void FramePropertiesSideBar::slotMaxValueChanged( int max )
{
  if( max == minInterv->value() - 1 )
    minInterv->setValue( max );

  minInterv->setMaximum( max + 1 );
  histogramWidget->slotMaxValueChanged( max );
  updateStatistiques();
}

void FramePropertiesSideBar::slotUpdateInterval( int min, int max )
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

void FramePropertiesSideBar::slotUpdateIntervRange( int range )
{
  maxInterv->blockSignals( true );
  maxInterv->setMaximum( range );
  maxInterv->blockSignals( false );
}
////////////////////////////////////////////////////////////////////////////////
//                             Informations
////////////////////////////////////////////////////////////////////////////////

void FramePropertiesSideBar::updateInformations()
{
#if 0
  QString value;
  switch( image.colorMode() )
  {
    case SImage::BlackWhite:
    value = tr( "Black and White" );
    break;
    case SImage::Gray:
    value = tr( "Grayscale" );
    break;
    default:
    value = tr( "Color" );
    break;
  }
  colorModeValueLabel->setText( value );
  value = tr( "%n bit(s)", "", image.depth() );
  depthValueLabel->setText( value );
  alphaValueLabel->setText( image.hasAlpha() ? tr( "Yes" ) : tr( "No" ) );
#endif
}
////////////////////////////////////////////////////////////////////////////////
//                                Statistiques
////////////////////////////////////////////////////////////////////////////////

void FramePropertiesSideBar::updateStatistiques()
{
  QString value;

  int min = minInterv->value();
  int max = maxInterv->value();
  int channel = histogramWidget->m_channelType;
  //int channel = channelCB->currentIndex();

  if( channel == HistogramWidget::ColorChannelsHistogram )
    channel = colorsCB->currentIndex() + 1;

  PlaYUVerFrameStatistics *histogram;

  if( histogramWidget->m_renderingType == HistogramWidget::FullImageHistogram )
    histogram = histogramWidget->m_imageHistogram;
  else
    histogram = histogramWidget->m_selectionHistogram;

  double mean = histogram->getMean( channel, min, max );
  labelMeanValue->setText( value.setNum( mean, 'f', 1 ) );

  double pixels = histogram->getPixels();
  labelPixelsValue->setText( value.setNum( ( float )pixels, 'f', 0 ) );

  double stddev = histogram->getStdDev( channel, min, max );
  labelStdDevValue->setText( value.setNum( stddev, 'f', 1 ) );

  double counts = histogram->getCount( channel, min, max );
  labelCountValue->setText( value.setNum( ( float )counts, 'f', 0 ) );

  double median = histogram->getMedian( channel, min, max );
  labelMedianValue->setText( value.setNum( median, 'f', 1 ) );

  double percentile = ( pixels > 0 ? ( 100.0 * counts / pixels ) : 0.0 );
  labelPercentileValue->setText( value.setNum( percentile, 'f', 1 ) );
}

static inline QSize bestSize( QSize currSize )
{
  QSize bestSize( 180, currSize.height() );
  if( currSize.width() < bestSize.width() )
    return bestSize;
  else
    return currSize;
}

}   // NAMESPACE

