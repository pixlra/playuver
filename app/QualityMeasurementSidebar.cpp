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
 * \file     QualityMeasurementSidebar.cpp
 * \brief    Definition of the quality measurement sidebar
 */

#include <QtGui>
#include "PlaYUVerSubWindowHandle.h"
#include "QualityMeasurementSidebar.h"
#include "VideoSubWindow.h"

QualityMeasurementSidebar::QualityMeasurementSidebar( QWidget* parent, PlaYUVerSubWindowHandle *windowManager ) :
        QWidget( parent ),
        m_pcMainWindowManager( windowManager ),
        m_pcCurrentVideoSubWindow( NULL )
{

  // Side bar area -----------------------------------------------------

  QLabel *MetricLabel = new QLabel( tr( "Metric:" ) );
  MetricLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  QLabel *RefLabel = new QLabel( tr( "Ref:" ) );
  RefLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  QLabel *RecLabel = new QLabel( tr( "Rec:" ) );
  RecLabel->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

  m_comboBoxMetric = new QComboBox;
  for( UInt i = 0; i < PlaYUVerFrame::supportedQualityMetricsList().size(); i++ )
  {
    m_comboBoxMetric->insertItem( i, PlaYUVerFrame::supportedQualityMetricsList()[i].c_str() );
  }
  m_comboBoxMetric->setSizeAdjustPolicy( QComboBox::AdjustToContents );
  m_comboBoxMetric->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

  m_comboBoxRef = new QComboBox;
  QSpacerItem* spaceQuality = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed );
  m_comboBoxRef->setSizeAdjustPolicy( QComboBox::AdjustToContents );
  m_comboBoxRef->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  updateSubWindowList();

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget( MetricLabel, 0, 0, Qt::AlignLeft );
  mainLayout->addWidget( m_comboBoxMetric, 0, 1, Qt::AlignLeft );
  mainLayout->addItem( spaceQuality, 0, 2, Qt::AlignRight );
  mainLayout->addWidget( RefLabel, 1, 0, Qt::AlignLeft );
  mainLayout->addWidget( m_comboBoxRef, 1, 1, 1, 2, Qt::AlignLeft );

  m_ppcLabelQualityLabel[LUMA] = new QLabel( "PSNR Y:" );
  m_ppcLabelQualityLabel[LUMA]->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  m_ppcLabelQualityValue[LUMA] = new QLabel;
  m_ppcLabelQualityValue[LUMA]->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  m_ppcLabelQualityLabel[CHROMA_U] = new QLabel( "PSNR U:" );
  m_ppcLabelQualityLabel[CHROMA_U]->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  m_ppcLabelQualityValue[CHROMA_U] = new QLabel;
  m_ppcLabelQualityValue[CHROMA_U]->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  m_ppcLabelQualityLabel[CHROMA_V] = new QLabel( "PSNR V:" );
  m_ppcLabelQualityLabel[CHROMA_V]->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  m_ppcLabelQualityValue[CHROMA_V] = new QLabel;
  m_ppcLabelQualityValue[CHROMA_V]->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QGridLayout *qualityResultsLayout = new QGridLayout;
  qualityResultsLayout->addWidget( m_ppcLabelQualityLabel[LUMA], 0, 0 );
  qualityResultsLayout->addWidget( m_ppcLabelQualityValue[LUMA], 0, 1 );
  qualityResultsLayout->addWidget( m_ppcLabelQualityLabel[CHROMA_U], 1, 0 );
  qualityResultsLayout->addWidget( m_ppcLabelQualityValue[CHROMA_U], 1, 1 );
  qualityResultsLayout->addWidget( m_ppcLabelQualityLabel[CHROMA_V], 2, 0 );
  qualityResultsLayout->addWidget( m_ppcLabelQualityValue[CHROMA_V], 2, 1 );

  //QGroupBox *statisticsGroup = new QGroupBox( tr( "Quality" ) );
  QGroupBox *statisticsGroup = new QGroupBox;
  statisticsGroup->setLayout( qualityResultsLayout );
  statisticsGroup->setWhatsThis( tr( "<p>Here you can see the quality results calculated for the "
      "selected windows." ) );

  mainLayout->addWidget( statisticsGroup, 4, 0, 3, 3 );

  mainLayout->setRowStretch( 8, 10 );
  setLayout( mainLayout );

  connect( m_comboBoxRef, SIGNAL( activated(int) ), this, SLOT( slotReferenceChanged(int) ) );
  connect( m_comboBoxMetric, SIGNAL( currentIndexChanged(int) ), this, SLOT( slotQualityMetricChanged(int) ) );

}

QualityMeasurementSidebar::~QualityMeasurementSidebar()
{

}

QSize QualityMeasurementSidebar::sizeHint() const
{
  QSize currSize = size();
  QSize bestSize( 240, currSize.height() );
  if( currSize.width() < bestSize.width() )
    return bestSize;
  else
    return currSize;
}

Void QualityMeasurementSidebar::updateSideBar( Bool hasSubWindow )
{
  if( !hasSubWindow )
    m_comboBoxRef->clear();
  setEnabled( hasSubWindow );
}

Void QualityMeasurementSidebar::updateSubWindowList()
{
  QString currSubWindowName;
  m_pcWindowListNames.clear();
  m_pcVideoWindowList.clear();

  VideoSubWindow* pcVideoSubWindow;
  QList<SubWindowAbstract*> subWindowList = m_pcMainWindowManager->findSubWindow( SubWindowAbstract::VIDEO_SUBWINDOW );
  for( Int i = 0; i < subWindowList.size(); i++ )
  {
    pcVideoSubWindow = qobject_cast<VideoSubWindow*>( subWindowList.at( i ) );
    if( m_pcCurrentVideoSubWindow == pcVideoSubWindow )
    {
      continue;
    }
    if( m_pcCurrentVideoSubWindow )
    {
      if( !m_pcCurrentVideoSubWindow->getCurrFrame()->haveSameFmt( pcVideoSubWindow->getCurrFrame() ) )
      {
        continue;
      }
    }
    currSubWindowName = pcVideoSubWindow->getWindowName();
    m_pcWindowListNames.append( currSubWindowName );
    m_pcVideoWindowList.append( pcVideoSubWindow );
  }
  m_comboBoxRef->clear();
  m_comboBoxRef->insertItems( -1, m_pcWindowListNames );
  m_comboBoxRef->setCurrentIndex( -1 );
  if( m_pcCurrentVideoSubWindow )
  {
    if( VideoSubWindow* refSubWindow = m_pcCurrentVideoSubWindow->getRefSubWindow() )
    {
      Int index = m_pcVideoWindowList.indexOf( refSubWindow );
      m_comboBoxRef->setCurrentIndex( index );
    }
  }
//  m_comboBoxRef->setSizeAdjustPolicy( QComboBox::AdjustToContents );
}

Void QualityMeasurementSidebar::updateCurrentWindow( VideoSubWindow *subWindow )
{
  m_pcCurrentVideoSubWindow = subWindow;
  if( m_pcCurrentVideoSubWindow )
  {
    Int index = -1;
    if( VideoSubWindow* refSubWindow = m_pcCurrentVideoSubWindow->getRefSubWindow() )
    {
      index = m_pcVideoWindowList.indexOf( refSubWindow );
    }
    m_comboBoxRef->setCurrentIndex( index );
    updateSubWindowList();
    updateSidebarData();
  }
}

Void QualityMeasurementSidebar::updateSidebarData()
{
  QString zeroValue( "0.0000" );
  if( m_pcCurrentVideoSubWindow )
  {
    VideoSubWindow* refSubWindow = m_pcCurrentVideoSubWindow->getRefSubWindow();
    if( refSubWindow )
    {
      PlaYUVerFrame* currFrame = m_pcCurrentVideoSubWindow->getCurrFrame();
      PlaYUVerFrame* refFrame = refSubWindow->getCurrFrame();
      Double quality;
      QString value;
      UInt component = 0;
      for( ; component < currFrame->getNumberChannels(); component++ )
      {
        quality = currFrame->getQuality( m_comboBoxMetric->currentIndex(), refFrame, component );
        m_ppcLabelQualityValue[component]->setText( value.setNum( quality, 'f', 4 ) );
      }
      for( ; component < 3; component++ )
      {
        quality = currFrame->getQuality( m_comboBoxMetric->currentIndex(), refFrame, component );
        m_ppcLabelQualityValue[component]->setText( zeroValue );
      }
      return;
    }
    else
    {
      m_comboBoxRef->setCurrentIndex( -1 );
    }
  }
  for( Int component = 0; component < 3; component++ )
  {
    m_ppcLabelQualityValue[component]->setText( zeroValue );
  }

}

Void QualityMeasurementSidebar::updateQualityMetric( Int idx )
{
  m_comboBoxMetric->setCurrentIndex( idx );
  QString labelQuality( PlaYUVerFrame::supportedQualityMetricsList()[idx].c_str() );
  m_ppcLabelQualityLabel[LUMA]->setText( labelQuality + " Y" );
  m_ppcLabelQualityLabel[CHROMA_U]->setText( labelQuality + " U" );
  m_ppcLabelQualityLabel[CHROMA_V]->setText( labelQuality + " V" );
  updateSidebarData();
}

Void QualityMeasurementSidebar::slotReferenceChanged( Int index )
{
  if( index > -1 && m_pcCurrentVideoSubWindow )
  {
    VideoSubWindow *pcRefSubWindow = m_pcVideoWindowList.at( index );
    if( pcRefSubWindow )
    {
      if( m_pcCurrentVideoSubWindow->getCurrFrame()->haveSameFmt( pcRefSubWindow->getCurrFrame() ) )
      {
        m_pcCurrentVideoSubWindow->setRefSubWindow( pcRefSubWindow );
        updateSidebarData();
        m_comboBoxRef->setCurrentIndex( index );
        return;
      }
    }
  }
  else
  {
    m_comboBoxRef->setCurrentIndex( -1 );
  }
}

Void QualityMeasurementSidebar::slotQualityMetricChanged( Int idx )
{
  emit signalQualityMetricChanged( idx );
  //updateQualityMetric( idx );
}

