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
 * \file     QualityMeasurement.cpp
 * \brief    Definition of the quality measurement sidebar
 */

#include "lib/PlaYUVerDefs.h"
#include <QtGui>
#include "QualityMeasurement.h"
#include "DialogSubWindowSelector.h"

namespace plaYUVer
{

QualityMeasurement::QualityMeasurement( QWidget* parent, QMdiArea* mdiArea ) :
        m_pcParet( parent ),
        m_pcMainWindowMdiArea( mdiArea )
{

}

QualityMeasurement::~QualityMeasurement()
{

}

Void QualityMeasurement::createActions()
{
  m_actionGroupQualityMetric = new QActionGroup( this );
  m_actionGroupQualityMetric->setExclusive( true );

  m_mapperQualityMetric = new QSignalMapper( this );

  QStringList qualityMetrics = PlaYUVerFrame::supportedQualityMetricsList();
  QAction* currAction;
  for( Int i = 0; i < qualityMetrics.size(); i++ )
  {
    currAction = new QAction( qualityMetrics.at( i ), this );
    currAction->setCheckable( true );
    m_actionGroupQualityMetric->addAction( currAction );
    connect( currAction, SIGNAL( triggered() ), m_mapperQualityMetric, SLOT( map() ) );
    m_mapperQualityMetric->setMapping( currAction, i );
  }

  m_arrayActions.resize( TOTAL_ACT );
  m_arrayActions[SELECT_REF_ACT] = new QAction( "Select Reference", this );
  connect( m_arrayActions[SELECT_REF_ACT], SIGNAL( triggered() ), this, SLOT( slotSelectReference() ) );
}

QMenu* QualityMeasurement::createMenu()
{
  m_pcMenuQuality = new QMenu( "Quality", this );
  QMenu* metricsSubMenu = m_pcMenuQuality->addMenu( "Quality Metrics" );
  metricsSubMenu->addActions( m_actionGroupQualityMetric->actions() );
  //m_pcMenuQuality->addActions( m_actionGroupQualityMetric->actions() );
  //m_pcMenuQuality->addSeparator();
  m_pcMenuQuality->addAction( m_arrayActions[SELECT_REF_ACT] );
  return m_pcMenuQuality;
}

QDockWidget* QualityMeasurement::createDock()
{
  m_pcQualityMeasurementSideBar = new QualityMeasurementSidebar( m_pcParet, m_pcMainWindowMdiArea );
  m_pcQualityMeasurementDock = new QDockWidget( tr( "Quality Measurement" ), this );
  m_pcQualityMeasurementDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  m_pcQualityMeasurementDock->setWidget( m_pcQualityMeasurementSideBar );

  connect( m_pcQualityMeasurementSideBar, SIGNAL( signalQualityMetricChanged(int) ), this, SLOT( slotQualityMetricChanged(int) ) );
  connect( m_mapperQualityMetric, SIGNAL( mapped(int) ), this, SLOT( slotQualityMetricChanged(int) ) );

  return m_pcQualityMeasurementDock;
}

Void QualityMeasurement::updateMenus()
{

}

Void QualityMeasurement::update( VideoSubWindow* currSubWindow )
{
  m_pcQualityMeasurementSideBar->updateSubWindowList();
  m_pcQualityMeasurementSideBar->updateCurrentWindow( currSubWindow );
}

Void QualityMeasurement::slotQualityMetricChanged( Int idx )
{
  m_actionGroupQualityMetric->actions().at( idx )->setChecked( true );
  m_pcQualityMeasurementSideBar->updateQualityMetric( idx );
  m_pcQualityMeasurementDock->show();
}

Void QualityMeasurement::slotSelectReference()
{
  DialogSubWindowSelector dialogWindowSelection( this, m_pcMainWindowMdiArea, 1, 1 );
  if( dialogWindowSelection.exec() == QDialog::Accepted )
  {
    VideoSubWindow* pcVideoSubWindow;
    VideoSubWindow* pcRefSubWindow;
    QStringList selectedWindows = dialogWindowSelection.getSelectedWindows();
    QList<VideoSubWindow*> subWindowList = m_pcMainWindowMdiArea->findChildren<VideoSubWindow*>();
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      pcVideoSubWindow = subWindowList.at( i );
      if( pcVideoSubWindow->getWindowName() == selectedWindows.at( 0 ) )
      {
        pcRefSubWindow = pcVideoSubWindow;
        break;
      }
    }
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      pcVideoSubWindow = subWindowList.at( i );
      pcVideoSubWindow->setRefSubWindow( pcRefSubWindow );
    }
    m_pcQualityMeasurementSideBar->updateSidebarData();
    m_pcQualityMeasurementDock->show();
  }
}

}   // NAMESPACE

