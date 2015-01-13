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
 * \file     QualityHandle.cpp
 * \brief    Definition of the quality measurement sidebar
 */

#include "lib/PlaYUVerDefs.h"
#include <QtGui>
#include "QualityHandle.h"
#include "DialogSubWindowSelector.h"

namespace plaYUVer
{

QualityHandle::QualityHandle( QWidget* parent, QMdiArea* mdiArea ) :
        m_pcParet( parent ),
        m_pcMainWindowMdiArea( mdiArea )
{

}

QualityHandle::~QualityHandle()
{

}

Void QualityHandle::createActions()
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

QMenu* QualityHandle::createMenu()
{
  m_pcMenuQuality = new QMenu( "Quality", this );
  QMenu* metricsSubMenu = m_pcMenuQuality->addMenu( "Quality Metrics" );
  metricsSubMenu->addActions( m_actionGroupQualityMetric->actions() );
  //m_pcMenuQuality->addActions( m_actionGroupQualityMetric->actions() );
  //m_pcMenuQuality->addSeparator();
  m_pcMenuQuality->addAction( m_arrayActions[SELECT_REF_ACT] );
  return m_pcMenuQuality;
}

QDockWidget* QualityHandle::createDock()
{
  m_pcQualityHandleSideBar = new QualityMeasurementSidebar( m_pcParet, m_pcMainWindowMdiArea );
  m_pcQualityHandleDock = new QDockWidget( tr( "Quality Measurement" ), this );
  m_pcQualityHandleDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  m_pcQualityHandleDock->setWidget( m_pcQualityHandleSideBar );

  connect( m_pcQualityHandleSideBar, SIGNAL( signalQualityMetricChanged(int) ), this, SLOT( slotQualityMetricChanged(int) ) );
  connect( m_mapperQualityMetric, SIGNAL( mapped(int) ), this, SLOT( slotQualityMetricChanged(int) ) );

  return m_pcQualityHandleDock;
}

Void QualityHandle::updateMenus()
{

}

Void QualityHandle::readSettings()
{

}

Void QualityHandle::writeSettings()
{

}

Void QualityHandle::update( VideoSubWindow* currSubWindow )
{
  m_pcQualityHandleSideBar->updateCurrentWindow( currSubWindow );
}

Void QualityHandle::slotQualityMetricChanged( Int idx )
{
  m_actionGroupQualityMetric->actions().at( idx )->setChecked( true );
  m_pcQualityHandleSideBar->updateQualityMetric( idx );
  m_pcQualityHandleDock->show();
}

Void QualityHandle::slotSelectReference()
{
  DialogSubWindowSelector dialogWindowSelection( this, m_pcMainWindowMdiArea, 1, 1 );
  if( dialogWindowSelection.exec() == QDialog::Accepted )
  {
    VideoSubWindow* pcVideoSubWindow;
    VideoSubWindow* pcRefSubWindow = NULL;
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
    m_pcQualityHandleSideBar->updateSidebarData();
    m_pcQualityHandleDock->show();
  }
}

}   // NAMESPACE

