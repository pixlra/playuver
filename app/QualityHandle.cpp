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

#include <QtGui>
#include "lib/LibMemory.h"
#include "PlaYUVerSubWindowHandle.h"
#include "QualityHandle.h"
#include "VideoSubWindow.h"
#include "PlotSubWindow.h"
#include "DialogSubWindowSelector.h"
#include "ProgressBar.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include "QtConcurrent/qtconcurrentrun.h"
#endif

namespace plaYUVer
{

QualityHandle::QualityHandle( QWidget* parent, PlaYUVerSubWindowHandle* windowManager ) :
        m_pcParet( parent ),
        m_pcMainWindowManager( windowManager )
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

  QAction* currAction;
  for( UInt i = 0; i < PlaYUVerFrame::supportedQualityMetricsList().size(); i++ )
  {
    currAction = new QAction( PlaYUVerFrame::supportedQualityMetricsList()[i].c_str(), this );
    currAction->setCheckable( true );
    m_actionGroupQualityMetric->addAction( currAction );
    connect( currAction, SIGNAL( triggered() ), m_mapperQualityMetric, SLOT( map() ) );
    m_mapperQualityMetric->setMapping( currAction, i );
  }

  m_arrayActions.resize( TOTAL_ACT );

  m_arrayActions[SELECT_CURR_REF_ACT] = new QAction( "Mark as Reference", this );
  connect( m_arrayActions[SELECT_CURR_REF_ACT], SIGNAL( triggered() ), this, SLOT( slotSelectCurrentAsReference() ) );
  m_arrayActions[SELECT_REF_ACT] = new QAction( "Select Reference", this );
  connect( m_arrayActions[SELECT_REF_ACT], SIGNAL( triggered() ), this, SLOT( slotSelectReference() ) );

  m_arrayActions[PLOT_QUALITY] = new QAction( "Plot Window's Quality", this );
  connect( m_arrayActions[PLOT_QUALITY], SIGNAL( triggered() ), this, SLOT( slotPlotQualitySingle() ) );
  m_arrayActions[PLOT_SEVERAL_QUALITY] = new QAction( "Plot Several Quality", this );
  connect( m_arrayActions[PLOT_SEVERAL_QUALITY], SIGNAL( triggered() ), this, SLOT( slotPlotQualitySeveral() ) );
}

QMenu* QualityHandle::createMenu()
{
  m_pcMenuQuality = new QMenu( "Quality", this );
  m_pcSubMenuQualityMetrics = m_pcMenuQuality->addMenu( "Quality Metrics" );
  m_pcSubMenuQualityMetrics->addActions( m_actionGroupQualityMetric->actions() );
  m_pcMenuQuality->addAction( m_arrayActions[SELECT_CURR_REF_ACT] );
  //m_pcMenuQuality->addAction( m_arrayActions[SELECT_REF_ACT] );
  m_pcMenuQuality->addSeparator();
  m_pcMenuQuality->addAction( m_arrayActions[PLOT_QUALITY] );
  m_pcMenuQuality->addAction( m_arrayActions[PLOT_SEVERAL_QUALITY] );
  return m_pcMenuQuality;
}

QDockWidget* QualityHandle::createDock()
{
  m_pcQualityHandleSideBar = new QualityMeasurementSidebar( m_pcParet, m_pcMainWindowManager );
  m_pcQualityHandleDock = new QDockWidget( tr( "Quality Measurement" ), this );
  m_pcQualityHandleDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  m_pcQualityHandleDock->setWidget( m_pcQualityHandleSideBar );

  connect( m_pcQualityHandleSideBar, SIGNAL( signalQualityMetricChanged(int) ), this, SLOT( slotQualityMetricChanged(int) ) );
  connect( m_mapperQualityMetric, SIGNAL( mapped(int) ), this, SLOT( slotQualityMetricChanged(int) ) );

  slotQualityMetricChanged( 0 );

  return m_pcQualityHandleDock;
}

Void QualityHandle::updateMenus()
{
  VideoSubWindow* pcSubWindow = qobject_cast<VideoSubWindow *>( m_pcMainWindowManager->activeSubWindow() );
  Bool hasSubWindow = pcSubWindow ? true : false;

  m_pcSubMenuQualityMetrics->setEnabled( hasSubWindow );
  m_arrayActions[SELECT_CURR_REF_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[SELECT_REF_ACT]->setEnabled( hasSubWindow );

  m_arrayActions[PLOT_QUALITY]->setEnabled( hasSubWindow );
  m_arrayActions[PLOT_SEVERAL_QUALITY]->setEnabled( hasSubWindow );

  m_pcQualityHandleSideBar->updateSideBar( hasSubWindow );
}

Void QualityHandle::readSettings()
{
  QSettings appSettings;
  Int metric = appSettings.value( "QualityHandle/Metric", 0 ).toInt();
  slotQualityMetricChanged( metric );
  if( !appSettings.value( "QualityHandle/QualitySideBar", true ).toBool() )
    m_pcQualityHandleDock->close();
}

Void QualityHandle::writeSettings()
{
  QSettings appSettings;
  appSettings.setValue( "QualityHandle/Metric", m_iQualityMetricIdx );
  appSettings.setValue( "QualityHandle/QualitySideBar", m_pcQualityHandleDock->isVisible() );
}

Void QualityHandle::update( VideoSubWindow* currSubWindow )
{
  m_pcQualityHandleSideBar->updateCurrentWindow( currSubWindow );
}

Void QualityHandle::measureQuality( QVector<VideoSubWindow*> apcWindowList )
{
  QVector<Double>* padQualityValues;
  UInt numberOfWindows = apcWindowList.size();
  UInt64 currFrames = 0;
  UInt64 numberOfFrames = INT_MAX;

  //! Check reference window
  for( UInt i = 0; i < numberOfWindows; i++ )
  {
    if( !apcWindowList.at( i )->getRefSubWindow() )
    {
      return;
    }
  }
  VideoSubWindow* pcReferenceWindow = apcWindowList.at( 0 )->getRefSubWindow();

  numberOfFrames = pcReferenceWindow->getInputStream()->getFrameNum();
  for( UInt i = 0; i < numberOfWindows; i++ )
  {
    currFrames = apcWindowList.at( i )->getInputStream()->getFrameNum();
    if( currFrames < numberOfFrames )
      numberOfFrames = currFrames;
    apcWindowList.at( i )->stop();
  }
  pcReferenceWindow->stop();

  ProgressBar* pcProgressBar = new ProgressBar( m_pcParet, numberOfFrames );

  padQualityValues = new QVector<Double> [numberOfWindows + 1];

  PlaYUVerFrame* pcReferenceFrame;
  PlaYUVerFrame* pcCurrFrame;
  for( UInt f = 0; f < numberOfFrames; f++ )
  {
    padQualityValues[0].append( f );
    pcReferenceFrame = pcReferenceWindow->getCurrFrame();
    for( UInt i = 0; i < numberOfWindows; i++ )
    {
      pcCurrFrame = apcWindowList.at( i )->getCurrFrame();
      padQualityValues[i + 1].append( pcCurrFrame->getQuality( m_iQualityMetricIdx, pcReferenceFrame, LUMA ) );
      apcWindowList.at( i )->seekRelativeEvent( true );
    }
    pcReferenceWindow->seekRelativeEvent( true );
    pcProgressBar->incrementProgress( 1 );
  }
  for( UInt i = 0; i < numberOfWindows; i++ )
  {
    apcWindowList.at( i )->stop();
  }
  pcReferenceWindow->stop();

  QString plotWindowTitle( QStringLiteral( "Quality" ) );
  if( apcWindowList.size() == 1 )
  {
    plotWindowTitle += " - " + apcWindowList.at( 0 )->getWindowName();
  }
  PlotSubWindow* pcPlotWindow = new PlotSubWindow( m_pcParet, plotWindowTitle );
  pcPlotWindow->setAxisName( "Frame Number", QString::fromStdString( PlaYUVerFrame::supportedQualityMetricsList()[m_iQualityMetricIdx] ) );

  if( apcWindowList.size() > 1 )
  {
    QString key;
    for( UInt i = 0; i < numberOfWindows; i++ )
    {
      key = apcWindowList.at( i )->getWindowName();
      pcPlotWindow->addPlot( padQualityValues[0], padQualityValues[i + 1], key );
    }
  }
  else
  {
    pcPlotWindow->addPlot( padQualityValues[0], padQualityValues[1] );
  }

  m_pcMainWindowManager->addSubWindow( pcPlotWindow );
  pcPlotWindow->show();

  pcProgressBar->close();

}

Void QualityHandle::slotQualityMetricChanged( Int idx )
{
  m_actionGroupQualityMetric->actions().at( idx )->setChecked( true );
  m_pcQualityHandleSideBar->updateQualityMetric( idx );
  m_pcQualityHandleDock->show();
  m_iQualityMetricIdx = idx;
}

Void QualityHandle::slotSelectCurrentAsReference()
{
  VideoSubWindow* pcRefSubWindow = qobject_cast<VideoSubWindow*>( m_pcMainWindowManager->activeSubWindow() );
  if( pcRefSubWindow )
  {
    VideoSubWindow* pcVideoSubWindow;
    QList<SubWindowHandle*> subWindowList = m_pcMainWindowManager->findSubWindow( SubWindowHandle::VIDEO_SUBWINDOW );
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      pcVideoSubWindow = qobject_cast<VideoSubWindow*>( subWindowList.at( i ) );
      if( pcVideoSubWindow != pcRefSubWindow )
        pcVideoSubWindow->setRefSubWindow( pcRefSubWindow );
    }
    m_pcQualityHandleSideBar->updateSidebarData();
    m_pcQualityHandleDock->show();
  }
}

Void QualityHandle::slotSelectReference()
{
  DialogSubWindowSelector dialogWindowSelection( this, m_pcMainWindowManager, SubWindowHandle::VIDEO_SUBWINDOW, 1, 1 );
  if( dialogWindowSelection.exec() == QDialog::Accepted )
  {
    VideoSubWindow* pcVideoSubWindow;
    VideoSubWindow* pcRefSubWindow = NULL;
    QStringList selectedWindows = dialogWindowSelection.getSelectedWindows();
    QList<SubWindowHandle*> subWindowList = m_pcMainWindowManager->findSubWindow( SubWindowHandle::VIDEO_SUBWINDOW );
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      pcVideoSubWindow = qobject_cast<VideoSubWindow*>( subWindowList.at( i ) );
      if( pcVideoSubWindow->getWindowName() == selectedWindows.at( 0 ) )
      {
        pcRefSubWindow = pcVideoSubWindow;
        break;
      }
    }
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      pcVideoSubWindow = qobject_cast<VideoSubWindow*>( subWindowList.at( i ) );
      if( pcVideoSubWindow != pcRefSubWindow )
        pcVideoSubWindow->setRefSubWindow( pcRefSubWindow );
    }
    m_pcQualityHandleSideBar->updateSidebarData();
    m_pcQualityHandleDock->show();
  }
}

Void QualityHandle::slotPlotQualitySingle()
{
  VideoSubWindow* pcSubWindow = qobject_cast<VideoSubWindow *>( m_pcMainWindowManager->activeSubWindow() );
  if( pcSubWindow )
  {
    if( pcSubWindow->getRefSubWindow() )
    {
      QVector<VideoSubWindow*> apcWindowList;
      apcWindowList.append( pcSubWindow );

//#ifndef QT_NO_CONCURRENT
//      m_cMeasurementResult.waitForFinished();
//      m_cMeasurementResult = QtConcurrent::run( this, &QualityHandle::measureQuality, apcWindowList );
//#else
      QApplication::setOverrideCursor( Qt::WaitCursor );
      measureQuality( apcWindowList );
      QApplication::restoreOverrideCursor();
//#endif

    }
  }
}

Void QualityHandle::slotPlotQualitySeveral()
{
  VideoSubWindow* pcSubWindow = qobject_cast<VideoSubWindow *>( m_pcMainWindowManager->activeSubWindow() );
  if( pcSubWindow )
  {
    QVector<VideoSubWindow*> apcWindowList;

    VideoSubWindow* pcRefSubWindow = pcSubWindow->getRefSubWindow();
    if( !pcRefSubWindow )
    {
      pcRefSubWindow = pcSubWindow;
    }

    VideoSubWindow* pcVideoSubWindow;
    QList<SubWindowHandle*> subWindowList = m_pcMainWindowManager->findSubWindow( SubWindowHandle::VIDEO_STREAM_SUBWINDOW );
    for( Int i = 0; i < subWindowList.size(); i++ )
    {
      pcVideoSubWindow = qobject_cast<VideoSubWindow *>( subWindowList.at( i ) );
      if( pcVideoSubWindow->getRefSubWindow() == pcRefSubWindow )
      {
        apcWindowList.append( pcVideoSubWindow );
      }
    }

    if( apcWindowList.size() > 0 )
    {
//#ifndef QT_NO_CONCURRENT
//      m_cMeasurementResult.waitForFinished();
//      m_cMeasurementResult = QtConcurrent::run( this, &QualityHandle::measureQuality, apcWindowList );
//#else
      QApplication::setOverrideCursor( Qt::WaitCursor );
      measureQuality( apcWindowList );
      QApplication::restoreOverrideCursor();
//#endif

    }
  }
}

}   // NAMESPACE

