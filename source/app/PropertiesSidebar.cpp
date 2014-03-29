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

////////////////////////////////////////////////////////////////////////////////
//                       StreamPropertiesSideBar
////////////////////////////////////////////////////////////////////////////////

StreamPropertiesSideBar::StreamPropertiesSideBar( QWidget* parent ) :
    QWidget( parent )
{
  // -------------- Variables definition --------------
  m_pcStream = NULL;

  // ----------------- GUI definition -----------------

  QGridLayout *mainLayout = new QGridLayout;

  QLabel *formatLabel = new QLabel( tr( "File Format:" ) );
  formatLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelFormatValue = new QLabel;
  labelFormatValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *sizeLabel = new QLabel( tr( "Size (pixels):" ) );
  sizeLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelSizeValue = new QLabel;
  labelSizeValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QLabel *colorSpaceLabel = new QLabel( tr( "Colorspace:" ) );
  colorSpaceLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  labelColorSpaceValue = new QLabel;
  labelColorSpaceValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QGridLayout *propertiesLayout = new QGridLayout;
  propertiesLayout->addWidget( formatLabel, 0, 0 );
  propertiesLayout->addWidget( labelFormatValue, 0, 1 );
  propertiesLayout->addWidget( sizeLabel, 1, 0 );
  propertiesLayout->addWidget( labelSizeValue, 1, 1 );
  propertiesLayout->addWidget( colorSpaceLabel, 2, 0 );
  propertiesLayout->addWidget( labelColorSpaceValue, 2, 1 );

  QGroupBox *propertiesGroup = new QGroupBox( tr( "Properties" ) );
  propertiesGroup->setLayout( propertiesLayout );
  mainLayout->addWidget( propertiesGroup, 6, 0, 1, 4 );

  mainLayout->setRowStretch( 8, 10 );
  setLayout( mainLayout );
  setEnabled( false );

}

StreamPropertiesSideBar::~StreamPropertiesSideBar()
{

}

QSize StreamPropertiesSideBar::sizeHint() const
{
  QSize isize = QSize( size().width(), size().height() );
  isize += QSize( 80, 0 );
  return isize;
}

Void StreamPropertiesSideBar::setData( InputStream* pcStream )
{
  // Clear information.
  labelFormatValue->clear();
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
    uint width = m_pcStream->getWidth();
    uint height = m_pcStream->getHeight();
    value = QString( tr( "%1 x %2" ) ).arg( width ).arg( height );
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

  // ----------------- GUI definition -----------------

  QGridLayout *mainLayout = new QGridLayout;

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

}

FramePropertiesSideBar::~FramePropertiesSideBar()
{

}

QSize FramePropertiesSideBar::sizeHint() const
{
  QSize isize = QSize( size().width(), size().height() );
  isize += QSize( 80, 0 );
  return isize;
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
    setEnabled( false );
    return;
  }
  else
  {
    m_pcFrame = pcFrame;
    setEnabled( true );
  }
}

}   // NAMESPACE

