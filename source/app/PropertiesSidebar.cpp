/*    This file is a part of plaYUVerApp project
 *    Copyright (C) 2014  by plaYUVerApp developers
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

#include "PlaYUVerFrame.h"

namespace plaYUVer
{

class PropertiesSidebarPrivate
{
public:

  PropertiesSidebarPrivate()
  {
    channelCB = 0;
    colorsCB = 0;
    renderingCB = 0;
    linHistoButton = 0;
    logHistoButton = 0;
    fullImageButton = 0;
    selectionImageButton = 0;
    minInterv = 0;
    maxInterv = 0;
    labelMeanValue = 0;
    labelPixelsValue = 0;
    labelStdDevValue = 0;
    labelCountValue = 0;
    labelMedianValue = 0;
    labelPercentileValue = 0;
    sizeValueLabel = 0;
    colorSpaceValueLabel = 0;
    depthValueLabel = 0;
    alphaValueLabel = 0;
    selectionArea = QRect();

    inLoadingProcess = false;
  }

  bool inLoadingProcess;

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
  QLabel *depthValueLabel;
  QLabel *alphaValueLabel;

  QRect selectionArea;

  InputStream* m_pcStream;
  PlaYUVerFrame *m_pcFrame;
};

////////////////////////////////////////////////////////////////////////////////
//                              Constructor 
////////////////////////////////////////////////////////////////////////////////

PropertiesSidebar::PropertiesSidebar( QWidget* parent ) :
    QWidget( parent )
{
  d = new PropertiesSidebarPrivate;

  // Histogram area -----------------------------------------------------

  QLabel *channelLabel = new QLabel( tr( "Channel:" ) );
  channelLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  d->channelCB = new QComboBox;
  d->channelCB->addItem( tr( "Luminance" ) );
  d->channelCB->setItemIcon( 0, QIcon( ":/images/channel-luma.png" ) );

  d->channelCB->setWhatsThis( tr( "<p>Select here the histogram channel to display:<p>"
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

  d->linHistoButton = new QPushButton;
  d->linHistoButton->setToolTip( tr( "Linear" ) );
  d->linHistoButton->setIcon( QIcon( ":/images/histogram-lin.png" ) );
  d->linHistoButton->setWhatsThis( scaleWhatsThis );
  d->linHistoButton->setCheckable( true );
  d->linHistoButton->setAutoExclusive( true );
  d->linHistoButton->setChecked( true );

  d->logHistoButton = new QPushButton;
  d->logHistoButton->setToolTip( tr( "Logarithmic" ) );
  d->logHistoButton->setIcon( QIcon( ":/images/histogram-log.png" ) );
  d->logHistoButton->setWhatsThis( scaleWhatsThis );
  d->logHistoButton->setCheckable( true );
  d->logHistoButton->setAutoExclusive( true );

  QHBoxLayout *scaleLayout = new QHBoxLayout;
  scaleLayout->addWidget( d->linHistoButton );
  scaleLayout->addWidget( d->logHistoButton );

  d->colorsLabel = new QLabel( tr( "Colors:" ) );
  d->colorsLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  d->colorsCB = new QComboBox;
  d->colorsCB->addItem( tr( "Red" ) );
  d->colorsCB->addItem( tr( "Green" ) );
  d->colorsCB->addItem( tr( "Blue" ) );
  d->colorsCB->setEnabled( false );
  d->colorsCB->setWhatsThis( tr( "<p>Select here the main color displayed with Colors Channel mode:"
      "<p><b>Red</b>: Draw the red image channel in the foreground.<p>"
      "<b>Green</b>: Draw the green image channel in the foreground.<p>"
      "<b>Blue</b>: Draw the blue image channel in the foreground.<p>" ) );

  d->colorsCB->hide();
  d->colorsLabel->hide();

  QString regionWhatsThis( tr( "<p>Select here from which region the histogram will be computed:"
      "<p><b>Full Image</b>: Compute histogram using the full image."
      "<p><b>Selection</b>: Compute histogram using the current image "
      "selection." ) );

  d->fullImageButton = new QPushButton;
  d->fullImageButton->setToolTip( tr( "Full Image" ) );
  d->fullImageButton->setIcon( QIcon( ":/images/image-full.png" ) );
  d->fullImageButton->setWhatsThis( regionWhatsThis );
  d->fullImageButton->setCheckable( true );
  d->fullImageButton->setAutoExclusive( true );
  d->fullImageButton->setChecked( true );

  d->selectionImageButton = new QPushButton;
  d->selectionImageButton->setToolTip( tr( "Selection" ) );
  d->selectionImageButton->setIcon( QIcon( ":/images/image-selection.png" ) );
  d->selectionImageButton->setWhatsThis( regionWhatsThis );
  d->selectionImageButton->setCheckable( true );
  d->selectionImageButton->setAutoExclusive( true );

  QHBoxLayout *regionLayout = new QHBoxLayout;
  regionLayout->addWidget( d->fullImageButton );
  regionLayout->addWidget( d->selectionImageButton );

  QGridLayout *topLayout = new QGridLayout;
  topLayout->addWidget( channelLabel, 1, 0 );
  topLayout->addWidget( d->channelCB, 1, 1 );
  topLayout->addLayout( scaleLayout, 1, 3 );
  topLayout->addWidget( d->colorsLabel, 2, 0 );
  topLayout->addWidget( d->colorsCB, 2, 1 );
  topLayout->addLayout( regionLayout, 2, 3 );
  topLayout->setColumnStretch( 2, 10 );

  // -------------------------------------------------------------

  QLabel *rangeLabel = new QLabel( tr( "Range:" ) );
  rangeLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  d->minInterv = new QSpinBox;
  d->minInterv->setRange( 0, 255 );
  d->minInterv->setSingleStep( 1 );
  d->minInterv->setValue( 0 );
  d->minInterv->setWhatsThis( tr( "<p>Select here the minimal intensity "
      "value of the histogram selection." ) );

  d->maxInterv = new QSpinBox;
  d->maxInterv->setRange( 0, 255 );
  d->maxInterv->setSingleStep( 1 );
  d->maxInterv->setValue( 0 );
  d->minInterv->setWhatsThis( tr( "<p>Select here the maximal intensity value"
      " of the histogram selection." ) );

  QHBoxLayout *rangeLayout = new QHBoxLayout;
  rangeLayout->addWidget( rangeLabel );
  rangeLayout->addWidget( d->minInterv );
  rangeLayout->addWidget( d->maxInterv );

  topLayout->addLayout( rangeLayout, 4, 0, 1, 4 );

  // -------------------------------------------------------------

  QLabel *pixelsLabel = new QLabel( tr( "Pixels:" ) );
  pixelsLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->labelPixelsValue = new QLabel;
  d->labelPixelsValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *countLabel = new QLabel( tr( "Selected:" ) );
  countLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->labelCountValue = new QLabel;
  d->labelCountValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *meanLabel = new QLabel( tr( "Mean:" ) );
  meanLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->labelMeanValue = new QLabel;
  d->labelMeanValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *stdDevLabel = new QLabel( tr( "Std. deviation:" ) );
  stdDevLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->labelStdDevValue = new QLabel;
  d->labelStdDevValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *medianLabel = new QLabel( tr( "Median:" ) );
  medianLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->labelMedianValue = new QLabel;
  d->labelMedianValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *percentileLabel = new QLabel( tr( "Percentile:" ) );
  percentileLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->labelPercentileValue = new QLabel;
  d->labelPercentileValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QGridLayout *statisticsLayout = new QGridLayout;
  statisticsLayout->addWidget( pixelsLabel, 0, 0 );
  statisticsLayout->addWidget( d->labelPixelsValue, 0, 1 );
  statisticsLayout->addWidget( countLabel, 1, 0 );
  statisticsLayout->addWidget( d->labelCountValue, 1, 1 );
  statisticsLayout->addWidget( meanLabel, 2, 0 );
  statisticsLayout->addWidget( d->labelMeanValue, 2, 1 );
  statisticsLayout->addWidget( stdDevLabel, 3, 0 );
  statisticsLayout->addWidget( d->labelStdDevValue, 3, 1 );
  statisticsLayout->addWidget( medianLabel, 4, 0 );
  statisticsLayout->addWidget( d->labelMedianValue, 4, 1 );
  statisticsLayout->addWidget( percentileLabel, 5, 0 );
  statisticsLayout->addWidget( d->labelPercentileValue, 5, 1 );

  QGroupBox *statisticsGroup = new QGroupBox( tr( "Statistics" ) );
  statisticsGroup->setLayout( statisticsLayout );
  statisticsGroup->setWhatsThis( tr( "<p>Here you can see the statistical results calculated from the "
      "selected histogram part. These values are available for all "
      "channels." ) );

  topLayout->addWidget( statisticsGroup, 5, 0, 1, 4 );

  // -------------------------------------------------------------

  QLabel *formatLabel = new QLabel( tr( "File Format:" ) );
  formatLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->formatValueLabel = new QLabel;
  d->formatValueLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *sizeLabel = new QLabel( tr( "Size (pixels):" ) );
  sizeLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->sizeValueLabel = new QLabel;
  d->sizeValueLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *colorSpaceLabel = new QLabel( tr( "Colorspace:" ) );
  colorSpaceLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->colorSpaceValueLabel = new QLabel;
  d->colorSpaceValueLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *depthLabel = new QLabel( tr( "Color depth:" ) );
  depthLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->depthValueLabel = new QLabel;
  d->depthValueLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *alphaLabel = new QLabel( tr( "Alpha Channel:" ) );
  alphaLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  d->alphaValueLabel = new QLabel;
  d->alphaValueLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QGridLayout *propertiesLayout = new QGridLayout;
  propertiesLayout->addWidget( formatLabel, 0, 0 );
  propertiesLayout->addWidget( d->formatValueLabel, 0, 1 );
  propertiesLayout->addWidget( sizeLabel, 1, 0 );
  propertiesLayout->addWidget( d->sizeValueLabel, 1, 1 );
  propertiesLayout->addWidget( colorSpaceLabel, 2, 0 );
  propertiesLayout->addWidget( d->colorSpaceValueLabel, 2, 1 );
  propertiesLayout->addWidget( depthLabel, 4, 0 );
  propertiesLayout->addWidget( d->depthValueLabel, 4, 1 );
  propertiesLayout->addWidget( alphaLabel, 5, 0 );
  propertiesLayout->addWidget( d->alphaValueLabel, 5, 1 );

  QGroupBox *propertiesGroup = new QGroupBox( tr( "Properties" ) );
  propertiesGroup->setLayout( propertiesLayout );
  topLayout->addWidget( propertiesGroup, 6, 0, 1, 4 );

  topLayout->setRowStretch( 8, 10 );

  setLayout( topLayout );

  setEnabled( false );

  // -------------------------------------------------------------
  connect( d->minInterv, SIGNAL( valueChanged (int) ), this, SLOT( slotMinValueChanged(int) ) );
  connect( d->maxInterv, SIGNAL( valueChanged (int) ), this, SLOT( slotMaxValueChanged(int) ) );

}

////////////////////////////////////////////////////////////////////////////////
//                               Destructor
////////////////////////////////////////////////////////////////////////////////

PropertiesSidebar::~PropertiesSidebar()
{
  // If there is a currently histogram computation when dialog is closed,
  // stop it before the d->image data are deleted automatically!
  delete d;
}
////////////////////////////////////////////////////////////////////////////////
//                             Set Data Function
////////////////////////////////////////////////////////////////////////////////

void PropertiesSidebar::setData( InputStream* m_pcStream )
{
  // We might be getting duplicate events from AlbumIconView,
  // which will cause all sorts of duplicate work.
  // More importantly, while the loading thread can handle this pretty well,
  // this will completely mess up the timing of progress info in the histogram
  // widget. So filter here, before the stopHistogramComputation!

  // Clear information.
  d->labelMeanValue->clear();
  d->labelPixelsValue->clear();
  d->labelStdDevValue->clear();
  d->labelCountValue->clear();
  d->labelMedianValue->clear();
  d->labelPercentileValue->clear();
  d->formatValueLabel->clear();
  d->sizeValueLabel->clear();
  d->colorSpaceValueLabel->clear();
  d->depthValueLabel->clear();
  d->alphaValueLabel->clear();

  if( !m_pcStream || !m_pcStream->getCurrFrame() )
  {
    setEnabled( false );
    return;
  }
  else
  {
    d->m_pcStream = m_pcStream;
    d->m_pcFrame = m_pcStream->getCurrFrame();

    if( d->m_pcFrame->getPelFormat() == PlaYUVerFrame::RGB8 && ( d->channelCB->count() == 1 ) )
    {
      d->channelCB->addItem( tr( "Red" ) );
      d->channelCB->setItemIcon( 1, QIcon( ":/images/channel-red.png" ) );
      d->channelCB->addItem( tr( "Green" ) );
      d->channelCB->setItemIcon( 2, QIcon( ":/images/channel-green.png" ) );
      d->channelCB->addItem( tr( "Blue" ) );
      d->channelCB->setItemIcon( 3, QIcon( ":/images/channel-blue.png" ) );

      d->channelCB->addItem( QIcon( ":/images/channel-all.png" ), tr( "Colors" ) );
      d->colorsCB->show();
      d->colorsLabel->show();
    }
    if( d->m_pcFrame->getPelFormat() != PlaYUVerFrame::RGB8 && ( d->channelCB->count() != 1 ) )
    {
      d->channelCB->clear();
      d->channelCB->addItem( tr( "Luminance" ) );
      d->channelCB->setItemIcon( 0, QIcon( ":/images/channel-luma.png" ) );
      d->colorsCB->hide();
      d->colorsLabel->hide();
    }

    setEnabled( true );

    if( d->m_pcFrame )
    {
      // If a selection area is done in Image Editor and if the current
      // image is the same in Image Viewer, then compute too the histogram
      // for this selection.

//                 d->histogramWidget->updateData( d->image.bits(),
//                                                 d->image.width(),
//                                                 d->image.height(),
//                                                 d->image.bitsPerChannel(),
//                                                 d->image.colorSpace() );

//    d->maxInterv->blockSignals( true );
//    if(d->image.bitsPerChannel()==16)
//        d->maxInterv->setMaximum( 65535 );
//    else
//        d->maxInterv->setMaximum( 255 );
//    d->maxInterv->blockSignals( false );
      d->fullImageButton->hide();
      d->selectionImageButton->hide();
      updateInformations();

    }
  }
}

////////////////////////////////////////////////////////////////////////////////
//                                  SLOTS
////////////////////////////////////////////////////////////////////////////////

void PropertiesSidebar::slotMinValueChanged( int min )
{
  // Called when user changes values of spin box.
  // Communicate the change to histogram widget.

  // make the one control "push" the other
  if( min == d->maxInterv->value() + 1 )
    d->maxInterv->setValue( min );

  d->maxInterv->setMinimum( min - 1 );
  updateStatistiques();
}

void PropertiesSidebar::slotMaxValueChanged( int max )
{
  if( max == d->minInterv->value() - 1 )
    d->minInterv->setValue( max );

  d->minInterv->setMaximum( max + 1 );
  updateStatistiques();
}

void PropertiesSidebar::slotUpdateInterval( int min, int max )
{
  // Called when value is set from within histogram widget.
  // Block signals to prevent slotMinValueChanged and
  // slotMaxValueChanged being called.
  d->minInterv->blockSignals( true );
  d->minInterv->setMaximum( max + 1 );
  d->minInterv->setValue( min );
  d->minInterv->blockSignals( false );

  d->maxInterv->blockSignals( true );
  d->maxInterv->setMinimum( min - 1 );
  d->maxInterv->setValue( max );
  d->maxInterv->blockSignals( false );

  updateStatistiques();
}

void PropertiesSidebar::slotUpdateIntervRange( int range )
{
  d->maxInterv->blockSignals( true );
  d->maxInterv->setMaximum( range );
  d->maxInterv->blockSignals( false );
}
////////////////////////////////////////////////////////////////////////////////
//                             Informations
////////////////////////////////////////////////////////////////////////////////

void PropertiesSidebar::updateInformations()
{
  QString value;

  uint width = d->m_pcFrame->getWidth();
  uint height = d->m_pcFrame->getHeight();
  value = QString( tr( "%1 x %2" ) ).arg( width ).arg( height );
  d->sizeValueLabel->setText( value );

  d->formatValueLabel->setText( d->m_pcStream->getFormatName() );

  value = QString( PlaYUVerFrame::supportedPixelFormatList().at( d->m_pcFrame->getPelFormat() ) );
  d->colorSpaceValueLabel->setText( value );

}
////////////////////////////////////////////////////////////////////////////////
//                                Statistiques
////////////////////////////////////////////////////////////////////////////////

void PropertiesSidebar::updateStatistiques()
{

}

}   // NAMESPACE

