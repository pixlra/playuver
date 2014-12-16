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
  m_arrayActions.resize( TOTAL_ACT );
  m_arrayActions[SELECT_REF_ACT] = new QAction( "Select Reference", this );
  connect( m_arrayActions[SELECT_REF_ACT], SIGNAL( triggered() ), this, SLOT( slotSelectReference() ) );
}

QMenu* QualityMeasurement::createMenu()
{
  m_pcMenuQuality = new QMenu( "Quality", this );
  m_pcMenuQuality->addAction( m_arrayActions[SELECT_REF_ACT] );
  return m_pcMenuQuality;
}

QDockWidget* QualityMeasurement::createDock()
{
  m_pcQualityMeasurementSideBar = new QualityMeasurementSidebar( m_pcParet, m_pcMainWindowMdiArea );
  m_pcQualityMeasurementDock = new QDockWidget( tr( "Quality Measurement" ), this );
  m_pcQualityMeasurementDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  m_pcQualityMeasurementDock->setWidget( m_pcQualityMeasurementSideBar );

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

