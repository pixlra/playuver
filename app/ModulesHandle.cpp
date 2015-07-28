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
 * \file     ModulesHandle.cpp
 * \brief    PlaYUVer modules handle
 */

#include <cstdio>
#include "PlaYUVerApp.h"
#include "ModulesHandle.h"
#include "PlaYUVerSubWindowHandle.h"
#include "VideoSubWindow.h"
#include "VideoHandle.h"
#include "PlaYUVerModuleFactory.h"
#include "DialogSubWindowSelector.h"
#include "ModulesHandleOptDialog.h"

namespace plaYUVer
{

ModulesHandle::ModulesHandle( QWidget* parent, PlaYUVerSubWindowHandle *windowManager, VideoHandle *moduleVideo ) :
        QWidget( parent ),
        m_pcParent( parent ),
        m_pcMainWindowManager( windowManager ),
        m_appModuleVideo( moduleVideo )
{
  setParent( m_pcParent );
}

ModulesHandle::~ModulesHandle()
{
}

Void ModulesHandle::createActions()
{
  m_arrayActions.resize( MODULES_TOTAL_ACT );

  m_pcActionMapper = new QSignalMapper( this );
  connect( m_pcActionMapper, SIGNAL( mapped(int) ), this, SLOT( processOpt(int) ) );

  m_arrayActions[FORCE_NEW_WINDOW_ACT] = new QAction( "Use New Window", parent() );
  m_arrayActions[FORCE_NEW_WINDOW_ACT]->setStatusTip( "Show module result in a new window. Some modules already force this feature" );
  m_arrayActions[FORCE_NEW_WINDOW_ACT]->setCheckable( true );
  m_arrayActions[FORCE_PLAYING_REFRESH_ACT] = new QAction( "Refresh while playing", parent() );
  m_arrayActions[FORCE_PLAYING_REFRESH_ACT]->setStatusTip( "Force module refreshing while playing" );
  m_arrayActions[FORCE_PLAYING_REFRESH_ACT]->setCheckable( true );

  m_arrayActions[APPLY_ALL_ACT] = new QAction( "Apply to All", parent() );
  m_arrayActions[APPLY_ALL_ACT]->setStatusTip( "Apply module to all frames and save the result" );
  connect( m_arrayActions[APPLY_ALL_ACT], SIGNAL( triggered() ), m_pcActionMapper, SLOT( map() ) );
  m_pcActionMapper->setMapping( m_arrayActions[APPLY_ALL_ACT], APPLY_ALL_ACT );

  m_arrayActions[SWAP_FRAMES_ACT] = new QAction( "Swap frames", parent() );
  m_arrayActions[SWAP_FRAMES_ACT]->setStatusTip( "Swap Sub Window order" );
  connect( m_arrayActions[SWAP_FRAMES_ACT], SIGNAL( triggered() ), m_pcActionMapper, SLOT( map() ) );
  m_pcActionMapper->setMapping( m_arrayActions[SWAP_FRAMES_ACT], SWAP_FRAMES_ACT );

  m_arrayActions[DISABLE_ACT] = new QAction( "Disable Modules", parent() );
  connect( m_arrayActions[DISABLE_ACT], SIGNAL( triggered() ), this, SLOT( destroyWindowModules() ) );

  m_arrayActions[DISABLE_ALL_ACT] = new QAction( "Disable All Modules", parent() );
  connect( m_arrayActions[DISABLE_ALL_ACT], SIGNAL( triggered() ), this, SLOT( destroyAllModulesIf() ) );

}

QMenu* ModulesHandle::createMenu()
{
  PlaYUVerModuleIf* pcCurrModuleIf;
  QString ModuleIfInternalName;
  QAction* currAction;
  QMenu* currSubMenu;

  m_pcModulesMenu = new QMenu( "&Modules", this );

  m_pcModulesActionMapper = new QSignalMapper( this );
  //connect( m_pcModulesActionMapper, SIGNAL( mapped(int) ), this, SLOT( activateModule(int) ) );

  UInt i = 0;
  PlaYUVerModuleFactoryMap& PlaYUVerModuleFactoryMap = PlaYUVerModuleFactory::Get()->getMap();
  PlaYUVerModuleFactoryMap::iterator it = PlaYUVerModuleFactoryMap.begin();
  for( ; it != PlaYUVerModuleFactoryMap.end(); ++it, i++ )
  {
    pcCurrModuleIf = it->second();
    ModuleIfInternalName = QString::fromLocal8Bit( it->first );

    currSubMenu = NULL;
    if( pcCurrModuleIf->m_pchModuleCategory )
    {
      for( Int j = 0; j < m_pcModulesSubMenuList.size(); j++ )
      {
        if( m_pcModulesSubMenuList.at( j )->title() == QString( pcCurrModuleIf->m_pchModuleCategory ) )
        {
          currSubMenu = m_pcModulesSubMenuList.at( j );
          break;
        }
      }
      if( !currSubMenu )
      {
        currSubMenu = m_pcModulesMenu->addMenu( pcCurrModuleIf->m_pchModuleCategory );
        m_pcModulesSubMenuList.append( currSubMenu );
      }
    }

    currAction = new QAction( pcCurrModuleIf->m_pchModuleName, parent() );
    currAction->setStatusTip( pcCurrModuleIf->m_pchModuleTooltip );
    currAction->setData( ModuleIfInternalName );
    currAction->setCheckable( false );
    connect( currAction, SIGNAL( triggered() ), this, SLOT( activateModule() ) );
    //connect( currAction, SIGNAL( triggered() ), m_pcModulesActionMapper, SLOT( map() ) );
    m_pcModulesActionMapper->setMapping( currAction, i );
    m_arrayModulesActions.append( currAction );

    if( currSubMenu )
      currSubMenu->addAction( currAction );
    else
      m_pcModulesMenu->addAction( currAction );

    pcCurrModuleIf->Delete();
  }

  m_pcModulesMenu->addSeparator();
  m_pcModulesMenu->addAction( m_arrayActions[APPLY_ALL_ACT] );
  m_pcModulesMenu->addAction( m_arrayActions[SWAP_FRAMES_ACT] );
  m_pcModulesMenu->addAction( m_arrayActions[DISABLE_ACT] );
  m_pcModulesMenu->addAction( m_arrayActions[DISABLE_ALL_ACT] );
  m_pcModulesMenu->addAction( m_arrayActions[FORCE_NEW_WINDOW_ACT] );

  return m_pcModulesMenu;
}

Void ModulesHandle::updateMenus()
{
  VideoSubWindow* pcSubWindow = qobject_cast<VideoSubWindow *>( m_pcMainWindowManager->activeSubWindow() );
  Bool hasSubWindow = pcSubWindow ? true : false;
  QAction* currModuleAction;

  for( Int i = 0; i < m_arrayModulesActions.size(); i++ )
  {
    currModuleAction = m_arrayModulesActions.at( i );
    currModuleAction->setEnabled( hasSubWindow );
    currModuleAction->setChecked( false );
  }
  for( Int i = 0; i < m_pcModulesSubMenuList.size(); i++ )
  {
    m_pcModulesSubMenuList.at( i )->setEnabled( hasSubWindow );
  }

  for( Int i = 0; i < m_arrayActions.size(); i++ )
  {
    m_arrayActions.at( i )->setEnabled( false );
  }
  m_arrayActions[FORCE_NEW_WINDOW_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[DISABLE_ALL_ACT]->setEnabled( m_pcPlaYUVerAppModuleIfList.size() > 0 );

  if( pcSubWindow )
  {
    QList<PlaYUVerAppModuleIf*> apcCurrentModule = pcSubWindow->getModuleArray();
    m_arrayActions[DISABLE_ACT]->setEnabled( apcCurrentModule.size() > 0 );
    if( pcSubWindow->getDisplayModule() )
    {
      m_arrayActions[APPLY_ALL_ACT]->setEnabled( true );
      m_arrayActions[SWAP_FRAMES_ACT]->setEnabled( true );
    }
    for( Int i = 0; i < apcCurrentModule.size(); i++ )
    {
      currModuleAction = apcCurrentModule.at( i )->m_pcModuleAction;
      currModuleAction->setChecked( true );
    }
  }
}

Void ModulesHandle::readSettings()
{
  QSettings appSettings;
  m_arrayActions[FORCE_NEW_WINDOW_ACT]->setChecked( appSettings.value( "ModulesHandle/UseNewWindow", false ).toBool() );
}

Void ModulesHandle::writeSettings()
{
  QSettings appSettings;
  appSettings.setValue( "ModulesHandle/UseNewWindow", m_arrayActions[FORCE_NEW_WINDOW_ACT]->isChecked() );
}

Void ModulesHandle::processOpt( Int index )
{
  VideoSubWindow* pcSubWindow = qobject_cast<VideoSubWindow *>( m_pcMainWindowManager->activeSubWindow() );
  if( pcSubWindow )
  {
    PlaYUVerAppModuleIf* pcCurrentModule = pcSubWindow->getDisplayModule();
    if( pcCurrentModule )
    {
      PlaYUVerAppModuleIf* pcCurrModule = pcCurrentModule;
      switch( index )
      {
      case INVALID_OPT:
        break;
      case SWAP_FRAMES_ACT:
        swapModulesWindowsIf( pcCurrModule );
        break;
      case APPLY_ALL_ACT:
        applyAllModuleIf( pcCurrModule );
        break;
      default:
        Q_ASSERT( 0 );
      }
    }
    emit changed();
  }
}

Void ModulesHandle::activateModule()
{
  QAction *pcAction = qobject_cast<QAction *>( sender() );
  VideoSubWindow* pcVideoSubWindow = qobject_cast<VideoSubWindow *>( m_pcMainWindowManager->activeSubWindow() );
  if( pcVideoSubWindow )
  {
    QString ModuleIfName = pcAction->data().toString();
    PlaYUVerModuleIf* pcCurrModuleIf = PlaYUVerModuleFactory::Get()->CreateModule( ModuleIfName.toLocal8Bit().constData() );

    PlaYUVerAppModuleIf* pcCurrAppModuleIf = new PlaYUVerAppModuleIf( this, pcAction, pcCurrModuleIf );
    enableModuleIf( pcCurrAppModuleIf );
    m_pcPlaYUVerAppModuleIfList.append( pcCurrAppModuleIf );
  }
  emit changed();
}

Void ModulesHandle::enableModuleIf( PlaYUVerAppModuleIf *pcCurrModuleIf )
{
  VideoSubWindow* pcModuleSubWindow = NULL;
  Bool bShowModulesNewWindow = m_arrayActions[FORCE_NEW_WINDOW_ACT]->isChecked();
  VideoSubWindow* pcVideoSubWindow = qobject_cast<VideoSubWindow *>( m_pcMainWindowManager->activeSubWindow() );

  QString windowName;

  QList<VideoSubWindow*> videoSubWindowList;
  Int numberOfFrames = pcCurrModuleIf->m_pcModule->m_uiNumberOfFrames;
  if( numberOfFrames > MODULE_REQUIRES_ONE_FRAME )  // Show dialog to select sub windows
  {
    DialogSubWindowSelector dialogWindowsSelection( m_pcParent, m_pcMainWindowManager, SubWindowAbstract::VIDEO_SUBWINDOW, numberOfFrames, numberOfFrames );
    QList<SubWindowAbstract*> windowsList = m_pcMainWindowManager->findSubWindow( SubWindowAbstract::VIDEO_SUBWINDOW );

    if( windowsList.size() <= numberOfFrames )
    {
      for( Int i = 0; i < windowsList.size(); i++ )
      {
        dialogWindowsSelection.selectSubWindow( windowsList.at( i ) );
      }
    }
    else
    {
      dialogWindowsSelection.selectSubWindow( pcVideoSubWindow );
    }
    if( dialogWindowsSelection.exec() == QDialog::Accepted )
    {
      VideoSubWindow *videoSubWindow;
      QList<SubWindowAbstract*> subWindowList = dialogWindowsSelection.getSelectedWindows();
      for( Int i = 0; i < subWindowList.size(); i++ )
      {
        videoSubWindow = qobject_cast<VideoSubWindow*>( subWindowList.at( i ) );
        videoSubWindowList.append( videoSubWindow );
      }
    }
    // Check for same fmt in more than one frame modules
    for( Int i = 1; i < videoSubWindowList.size(); i++ )
    {
      if( !videoSubWindowList.at( i )->getCurrFrame()->haveSameFmt( videoSubWindowList.at( 0 )->getCurrFrame() ) )
      {
        videoSubWindowList.clear();
        break;
      }
    }
  }
  else
  {
    videoSubWindowList.append( pcVideoSubWindow );
  }

  if( videoSubWindowList.size() == 0 )
  {
    qobject_cast<PlaYUVerApp*>( m_pcParent )->printMessage( "Error! Module cannot be applied", LOG_ERROR );
    destroyModuleIf( pcCurrModuleIf );
    return;
  }

  windowName.append( QStringLiteral( "Module " ) );
  windowName.append( pcCurrModuleIf->m_pcModule->m_pchModuleName );

  for( Int i = 0; i < videoSubWindowList.size(); i++ )
  {
    pcCurrModuleIf->m_pcSubWindow[i] = videoSubWindowList.at( i );
  }

  if( pcCurrModuleIf->m_pcModule->m_uiModuleRequirements & MODULE_REQUIRES_OPTIONS )
  {
    ModulesHandleOptDialog moduleOptDialog( m_pcParent, pcCurrModuleIf );
    if( moduleOptDialog.runConfiguration() == QDialog::Rejected )
    {
      qobject_cast<PlaYUVerApp*>( m_pcParent )->printMessage( "Module canceled by user!", LOG_WARNINGS );
      destroyModuleIf( pcCurrModuleIf );
      return;
    }
  }

  if( pcCurrModuleIf->m_pcModule->m_iModuleType == FRAME_PROCESSING_MODULE )
  {
    if( ( pcCurrModuleIf->m_pcModule->m_uiModuleRequirements & MODULE_REQUIRES_NEW_WINDOW ) || bShowModulesNewWindow )
    {
      pcModuleSubWindow = new VideoSubWindow( VideoSubWindow::MODULE_SUBWINDOW );
      pcModuleSubWindow->setWindowShortName( windowName );
      pcModuleSubWindow->setWindowName( windowName );

      connect( pcModuleSubWindow->getViewArea(), SIGNAL( selectionChanged( QRect ) ), m_appModuleVideo, SLOT( updateSelectionArea( QRect ) ) );
      connect( pcModuleSubWindow, SIGNAL( zoomFactorChanged_SWindow( const double, const QPoint ) ), m_appModuleVideo,
          SLOT( zoomToFactorAll( double, QPoint ) ) );
      connect( pcModuleSubWindow, SIGNAL( scrollBarMoved_SWindow( const QPoint ) ), m_appModuleVideo, SLOT( moveAllScrollBars( const QPoint ) ) );

      pcCurrModuleIf->m_pcDisplaySubWindow = pcModuleSubWindow;
    }
  }
  else if( pcCurrModuleIf->m_pcModule->m_iModuleType == FRAME_MEASUREMENT_MODULE )
  {
    pcCurrModuleIf->m_pcDisplaySubWindow = NULL;
    pcCurrModuleIf->m_pcModuleDock = new ModuleHandleDock( m_pcParent, pcCurrModuleIf );
    QString titleDockWidget;
    titleDockWidget.append( pcCurrModuleIf->m_pcModule->m_pchModuleName );
    titleDockWidget.append( " Information" );
    pcCurrModuleIf->m_pcDockWidget = new QDockWidget( titleDockWidget, m_pcParent );
    pcCurrModuleIf->m_pcDockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    pcCurrModuleIf->m_pcDockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    pcCurrModuleIf->m_pcDockWidget->setWidget( pcCurrModuleIf->m_pcModuleDock );
    qobject_cast<QMainWindow*>( m_pcParent )->addDockWidget( Qt::RightDockWidgetArea, pcCurrModuleIf->m_pcDockWidget );
  }

  // Associate module with subwindows
  if( !pcCurrModuleIf->m_pcDisplaySubWindow && !pcCurrModuleIf->m_pcModuleDock )
  {
    pcCurrModuleIf->m_pcSubWindow[0]->enableModule( pcCurrModuleIf );
  }
  else
  {
    if( pcCurrModuleIf->m_pcDisplaySubWindow )
      pcCurrModuleIf->m_pcDisplaySubWindow->enableModule( pcCurrModuleIf );
    for( Int i = 0; i < numberOfFrames; i++ )
    {
      pcCurrModuleIf->m_pcSubWindow[i]->associateModule( pcCurrModuleIf );
    }
  }

  // Create Module
  Bool moduleCreated = false;
  if( pcCurrModuleIf->m_pcModule->m_iModuleAPI == MODULE_API_2 )
  {
    std::vector<PlaYUVerFrame*> apcFrameList;
    for( UInt i = 0; i < pcCurrModuleIf->m_pcModule->m_uiNumberOfFrames; i++ )
    {
      apcFrameList.push_back( pcCurrModuleIf->m_pcSubWindow[i]->getCurrFrame() );
    }
    moduleCreated = pcCurrModuleIf->m_pcModule->create( apcFrameList );
  }
  else if( pcCurrModuleIf->m_pcModule->m_iModuleAPI == MODULE_API_1 )
  {
    pcCurrModuleIf->m_pcModule->create( pcCurrModuleIf->m_pcSubWindow[0]->getCurrFrame() );
    moduleCreated = true;
  }

  if( !moduleCreated )
  {
    qobject_cast<PlaYUVerApp*>( m_pcParent )->printMessage( "Error! Module cannot be applied", LOG_ERROR );
    destroyModuleIf( pcCurrModuleIf );
    return;
  }

  applyModuleIf( pcCurrModuleIf, false, true );
  QCoreApplication::processEvents();

  if( pcModuleSubWindow )
  {
    m_pcMainWindowManager->addSubWindow( pcModuleSubWindow );
    pcModuleSubWindow->show();
  }
  return;
}

Void ModulesHandle::destroyModuleIf( PlaYUVerAppModuleIf *pcCurrModuleIf )
{
  if( pcCurrModuleIf )
  {
    pcCurrModuleIf->destroy();
  }
}

Void ModulesHandle::destroyWindowModules()
{
  VideoSubWindow* pcVideoSubWindow = qobject_cast<VideoSubWindow *>( m_pcMainWindowManager->activeSubWindow() );
  if( pcVideoSubWindow )
  {
    pcVideoSubWindow->disableModule();
  }
}

Void ModulesHandle::destroyAllModulesIf()
{
  for( Int i = 0; i < m_pcPlaYUVerAppModuleIfList.size(); i++ )
  {
    destroyModuleIf( m_pcPlaYUVerAppModuleIfList.at( i ) );
  }
}

Bool ModulesHandle::applyModuleIf( PlaYUVerAppModuleIf *pcCurrModuleIf, Bool isPlaying, Bool disableThreads )
{
  Bool bRet = false;
  QApplication::setOverrideCursor( Qt::WaitCursor );
  if( pcCurrModuleIf->m_pcDisplaySubWindow )
  {
    pcCurrModuleIf->m_pcDisplaySubWindow->setFillWindow( true );
  }
  else
  {
    pcCurrModuleIf->m_pcSubWindow[0]->setFillWindow( true );
  }
  if( !( isPlaying && ( pcCurrModuleIf->m_pcModule->m_uiModuleRequirements & MODULE_REQUIRES_SKIP_WHILE_PLAY ) ) )
  {
#ifdef PLAYUVER_THREADED_MODULES
    if( !disableThreads )
      pcCurrModuleIf->start();
    else
#endif
      pcCurrModuleIf->run();

    if( pcCurrModuleIf->m_pcDisplaySubWindow || pcCurrModuleIf->m_pcModule->m_iModuleType == FRAME_MEASUREMENT_MODULE )
    {
      bRet = true;
    }
  }
  else
  {
    bRet = true;
  }
  QApplication::restoreOverrideCursor();
  return bRet;
}

Void ModulesHandle::applyAllModuleIf( PlaYUVerAppModuleIf *pcCurrModuleIf )
{
  if( pcCurrModuleIf )
  {
    if( !pcCurrModuleIf->m_pcModuleStream )
    {
      UInt Width = 0, Height = 0, FrameRate = 30;
      Int BitsPixel, InputFormat = -1;

      QString supported = tr( "Supported Files (" );
      QStringList formatsList;
      std::vector<std::string> formatsExt = PlaYUVerStream::supportedWriteFormatsExt();
      std::vector<std::string> formatsName = PlaYUVerStream::supportedWriteFormatsName();

      for( UInt i = 0; i < formatsName.size(); i++ )
      {
        QString currFmt;
        supported.append( " *." );
        supported.append( QString::fromStdString( formatsExt[i] ) );
        currFmt.append( QString::fromStdString( formatsName[i] ) );
        currFmt.append( " (*." );
        currFmt.append( QString::fromStdString( formatsExt[i] ) );
        currFmt.append( ")" );
        formatsList << currFmt;
      }
      supported.append( " )" );

      QStringList filter;
      filter << supported
             << formatsList
             << tr( "All Files (*)" );

      QString fileName = QFileDialog::getSaveFileName( m_pcParent, tr( "Open File" ), QString(), filter.join( ";;" ) );

      Width = pcCurrModuleIf->m_pcProcessedFrame->getWidth();
      Height = pcCurrModuleIf->m_pcProcessedFrame->getHeight();
      InputFormat = pcCurrModuleIf->m_pcProcessedFrame->getPelFormat();
      BitsPixel = pcCurrModuleIf->m_pcProcessedFrame->getBitsPel();
      FrameRate = pcCurrModuleIf->m_pcSubWindow[0]->getInputStream()->getFrameRate();

      pcCurrModuleIf->m_pcModuleStream = new PlaYUVerStream;
      if( !pcCurrModuleIf->m_pcModuleStream->open( fileName.toStdString(), Width, Height, InputFormat, BitsPixel, FrameRate, false ) )
      {
        delete pcCurrModuleIf->m_pcModuleStream;
        pcCurrModuleIf->m_pcModuleStream = NULL;
        return;
      }
    }
  }
  if( pcCurrModuleIf->m_pcModuleStream )
  {
    UInt numberOfWindows = pcCurrModuleIf->m_pcModule->m_uiNumberOfFrames;
    UInt64 currFrames = 0;
    UInt64 numberOfFrames = INT_MAX;
    for( UInt i = 0; i < numberOfWindows; i++ )
    {
      currFrames = pcCurrModuleIf->m_pcSubWindow[i]->getInputStream()->getFrameNum();
      if( currFrames < numberOfFrames )
        numberOfFrames = currFrames;
      pcCurrModuleIf->m_pcSubWindow[i]->stop();
    }
    QApplication::setOverrideCursor( Qt::WaitCursor );
    for( UInt f = 1; f < numberOfFrames; f++ )
    {
      applyModuleIf( pcCurrModuleIf, false, true );
      QCoreApplication::processEvents();
      pcCurrModuleIf->m_pcModuleStream->writeFrame( pcCurrModuleIf->m_pcProcessedFrame );
      for( UInt i = 0; i < numberOfWindows; i++ )
      {
        pcCurrModuleIf->m_pcSubWindow[i]->play();
        pcCurrModuleIf->m_pcSubWindow[i]->playEvent();
      }
    }
    for( UInt i = 0; i < numberOfWindows; i++ )
    {
      pcCurrModuleIf->m_pcSubWindow[i]->stop();
    }
    QApplication::restoreOverrideCursor();
  }
}

Void ModulesHandle::showModuleIf( PlaYUVerAppModuleIf *pcCurrModuleIf )
{
  switch( pcCurrModuleIf->m_pcModule->m_iModuleType )
  {
  case FRAME_PROCESSING_MODULE:
    if( pcCurrModuleIf->m_pcDisplaySubWindow )
    {
      pcCurrModuleIf->m_pcDisplaySubWindow->setCurrFrame( pcCurrModuleIf->m_pcProcessedFrame );
      pcCurrModuleIf->m_pcDisplaySubWindow->setFillWindow( false );
      pcCurrModuleIf->m_pcDisplaySubWindow->clearWindowBusy();
    }
    else
    {
      pcCurrModuleIf->m_pcSubWindow[0]->setCurrFrame( pcCurrModuleIf->m_pcProcessedFrame );
      pcCurrModuleIf->m_pcSubWindow[0]->setFillWindow( false );
      pcCurrModuleIf->m_pcSubWindow[0]->clearWindowBusy();
    }
    break;
  case FRAME_MEASUREMENT_MODULE:
    pcCurrModuleIf->m_pcModuleDock->setModulueReturnValue( pcCurrModuleIf->m_dMeasurementResult );
    break;
  }
}

Void ModulesHandle::swapModulesWindowsIf( PlaYUVerAppModuleIf *pcCurrModuleIf )
{
  if( pcCurrModuleIf->m_pcModule->m_uiNumberOfFrames == MODULE_REQUIRES_TWO_FRAMES )
  {
    VideoSubWindow* auxWindowHandle = pcCurrModuleIf->m_pcSubWindow[0];
    pcCurrModuleIf->m_pcSubWindow[0] = pcCurrModuleIf->m_pcSubWindow[1];
    pcCurrModuleIf->m_pcSubWindow[1] = auxWindowHandle;
    applyModuleIf( pcCurrModuleIf );
  }
}

Void ModulesHandle::customEvent( QEvent *event )
{
  if( !event )
  {
    return;
  }
  PlaYUVerAppModuleIf::EventData *eventData = ( PlaYUVerAppModuleIf::EventData* )event;
  if( eventData->m_bSuccess )
  {
    showModuleIf( eventData->m_pcModule );
  }
}

}  // NAMESPACE

