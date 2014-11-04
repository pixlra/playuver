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
 * \file     ModulesHandle.cpp
 * \brief    PlaYUVer modules handle
 */

#include <cstdio>

#include "ModulesHandle.h"
#include "ModulesListHeader.h"
#include "ModulesListMacro.h"
#include "DialogSubWindowSelector.h"
#include "SubWindowHandle.h"

namespace plaYUVer
{

ModulesHandle::ModulesHandle( QMainWindow * parent, QMdiArea *mdiArea ) :
        QObject( parent ),
        m_pcParent( parent ),
        m_pcMdiArea( mdiArea ),
        m_uiModulesCount( 0 ),
        m_iOptionSelected( INVALID_OPT )
{
  // configure class
  setParent( m_pcParent );
  // Register Modules
  REGISTER_ALL_MODULES
}

ModulesHandle::~ModulesHandle()
{
}

Void ModulesHandle::appendModule( PlaYUVerModuleIf* pIfModule )
{
  m_pcPlaYUVerModules.append( pIfModule );
  m_uiModulesCount++;
}

Void ModulesHandle::selectModule( Int index )
{
  m_iOptionSelected = index;
}

SubWindowHandle* ModulesHandle::processModuleHandlingOpt()
{
  if( m_iOptionSelected >= 0 )
  {
    PlaYUVerModuleIf* pcCurrModuleIf = m_pcPlaYUVerModules.at( m_iOptionSelected );
    m_iOptionSelected = INVALID_OPT;
    if( pcCurrModuleIf->m_pcAction->isChecked() )
    {
      return enableModuleIf( pcCurrModuleIf );
    }
    else
    {
      destroyModuleIf( pcCurrModuleIf );
    }
  }
  else
  {
    SubWindowHandle* pcSubWindow = qobject_cast<SubWindowHandle *>( m_pcMdiArea->activeSubWindow() );
    switch( m_iOptionSelected )
    {
    case INVALID_OPT:
      break;
    case SWAP_FRAMES_OPT:
      pcSubWindow->swapModuleFrames();
      break;
    case APPLY_ALL_OPT:
      openModuleIfStream( pcSubWindow->getModule() );
      pcSubWindow->applyModuleAllFrames();
      break;
    default:
      Q_ASSERT( 0 );
    }
  }
  m_iOptionSelected = INVALID_OPT;
  return NULL;
}

SubWindowHandle* ModulesHandle::enableModuleIf( PlaYUVerModuleIf *pcCurrModuleIf )
{
  Bool bShowModulesNewWindow = m_arrayActions[FORCE_NEW_WINDOW_ACT]->isChecked();
  SubWindowHandle* pcSubWindow = qobject_cast<SubWindowHandle *>( m_pcMdiArea->activeSubWindow() );
  SubWindowHandle* interfaceChild = NULL;
  QString windowName;

  QVector<SubWindowHandle*> subWindowList;
  UInt numberOfFrames = pcCurrModuleIf->m_cModuleDef.m_uiNumberOfFrames;
  if( numberOfFrames > MODULE_REQUIRES_ONE_FRAME )  // Show dialog to select sub windows
  {
    DialogSubWindowSelector dialogWindowsSelection( m_pcParent, m_pcMdiArea, numberOfFrames, numberOfFrames );
    if( dialogWindowsSelection.exec() == QDialog::Accepted )
    {
      QStringList selectedWindows = dialogWindowsSelection.getSelectedWindows();
      SubWindowHandle *subWindow;

      for( Int j = 0; j < selectedWindows.size(); j++ )
      {
        for( Int i = 0; i < m_pcMdiArea->subWindowList().size(); i++ )
        {
          subWindow = qobject_cast<SubWindowHandle *>( m_pcMdiArea->subWindowList().at( i ) );
          if( subWindow->getWindowName() == selectedWindows.at( j ) )
          {
            subWindowList.append( subWindow );
            pcCurrModuleIf->m_pcSubWindow[j] = subWindow;
          }
        }
      }
    }
    // Check for same fmt in more than one frame modules
    for( Int i = 1; i < subWindowList.size(); i++ )
    {
      if( !subWindowList.at( i )->getCurrFrame()->haveSameFmt( subWindowList.at( 0 )->getCurrFrame()) )
      {
        subWindowList.clear();
        break;
      }
    }
  }
  else
  {
    windowName.append( pcSubWindow->getWindowName() );
    subWindowList.append( pcSubWindow );
  }

  if( subWindowList.size() == 0 )
  {
    pcCurrModuleIf->m_pcAction->setChecked( false );
    m_pcParent->statusBar()->showMessage( "Error! Module cannot be applied", 2000 );
    return interfaceChild;
  }

  windowName.append( " <" );
  windowName.append( pcCurrModuleIf->m_cModuleDef.m_pchModuleName );
  windowName.append( ">" );

  for( Int i = 0; i < subWindowList.size(); i++ )
  {
    pcCurrModuleIf->m_pcSubWindow[i] = subWindowList.at( i );
    pcCurrModuleIf->m_pcSubWindow[i]->disableModule();
  }

  pcCurrModuleIf->m_pcDisplaySubWindow = NULL;
  if( pcCurrModuleIf->m_cModuleDef.m_iModuleType == FRAME_PROCESSING_MODULE )
  {
    if( ( pcCurrModuleIf->m_cModuleDef.m_uiModuleRequirements & MODULE_REQUIRES_NEW_WINDOW ) || bShowModulesNewWindow )
    {
      interfaceChild = new SubWindowHandle( m_pcParent, true );
      interfaceChild->setWindowName( windowName );
      connect( interfaceChild->getViewArea(), SIGNAL( positionChanged(const QPoint &, PlaYUVerFrame *) ), m_pcParent,
          SLOT( updatePixelValueStatusBar(const QPoint &, PlaYUVerFrame *) ) );
      pcCurrModuleIf->m_pcDisplaySubWindow = interfaceChild;
      pcCurrModuleIf->m_pcDisplaySubWindow->setModule( pcCurrModuleIf );
    }
  }
  else if( pcCurrModuleIf->m_cModuleDef.m_iModuleType == FRAME_MEASUREMENT_MODULE )
  {
    if( pcCurrModuleIf->m_cModuleDef.m_uiModuleRequirements & MODULE_REQUIRES_SIDEBAR )
    {
      pcCurrModuleIf->m_pcModuleDock = new ModuleHandleDock( m_pcParent, pcCurrModuleIf );
      QString titleDockWidget;
      titleDockWidget.append( pcCurrModuleIf->m_cModuleDef.m_pchModuleName );
      titleDockWidget.append( " Information" );
      pcCurrModuleIf->m_pcDockWidget = new QDockWidget( titleDockWidget, m_pcParent );
      pcCurrModuleIf->m_pcDockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
      pcCurrModuleIf->m_pcDockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
      pcCurrModuleIf->m_pcDockWidget->setWidget( pcCurrModuleIf->m_pcModuleDock );
      m_pcParent->addDockWidget( Qt::RightDockWidgetArea, pcCurrModuleIf->m_pcDockWidget );
    }
  }

  // Create Module
  pcCurrModuleIf->create( pcCurrModuleIf->m_pcSubWindow[0]->getCurrFrame() );

  // Associate module with subwindows
  for( UInt i = 0; i < numberOfFrames; i++ )
  {
    pcCurrModuleIf->m_pcSubWindow[i]->enableModule( pcCurrModuleIf );
  }
  applyModuleIf( pcCurrModuleIf, false );

  return interfaceChild;
}

Void ModulesHandle::destroyModuleIf( PlaYUVerModuleIf *pcCurrModuleIf )
{
  if( pcCurrModuleIf->m_pcModuleDock )
  {
    pcCurrModuleIf->m_pcModuleDock->close();
    pcCurrModuleIf->m_pcModuleDock = NULL;
  }
  if( pcCurrModuleIf->m_pcDockWidget )
  {
    pcCurrModuleIf->m_pcDockWidget->close();
    pcCurrModuleIf->m_pcDockWidget = NULL;
  }
  if( pcCurrModuleIf->m_pcDisplaySubWindow )
    pcCurrModuleIf->m_pcDisplaySubWindow->close();
  pcCurrModuleIf->m_pcDisplaySubWindow = NULL;

  for( Int i = 0; i < MAX_NUMBER_FRAMES; i++ )
  {
    if( pcCurrModuleIf->m_pcSubWindow[i] )
    {
      pcCurrModuleIf->m_pcSubWindow[i]->disableModule();
      pcCurrModuleIf->m_pcSubWindow[i] = NULL;
    }
  }
  if( pcCurrModuleIf->m_pcModuleStream )
  {
    pcCurrModuleIf->m_pcModuleStream->close();
    delete pcCurrModuleIf->m_pcModuleStream;
    pcCurrModuleIf->m_pcModuleStream = NULL;
  }
  pcCurrModuleIf->m_pcAction->setChecked( false );
  pcCurrModuleIf->destroy();
}

Void ModulesHandle::destroyAllModulesIf()
{
  for( Int i = 0; i < m_pcPlaYUVerModules.size(); i++ )
  {
    destroyModuleIf( m_pcPlaYUVerModules.at( i ) );
  }
}

Bool ModulesHandle::applyModuleIf( PlaYUVerModuleIf *pcCurrModuleIf, Bool isPlaying, Bool disableThreads )
{
  Bool bRet = false;
  if( !isPlaying || ( isPlaying && pcCurrModuleIf->m_cModuleDef.m_bApplyWhilePlaying ) )
  {
#ifdef PLAYUVER_THREADED_MODULES
    if( !disableThreads )
      pcCurrModuleIf->start();
    else
#endif
      pcCurrModuleIf->run();

    if( pcCurrModuleIf->m_pcDisplaySubWindow || pcCurrModuleIf->m_cModuleDef.m_iModuleType == FRAME_MEASUREMENT_MODULE )
    {
      bRet = true;
    }
  }
  else
  {
    bRet = true;
  }
  return bRet;
}

Void ModulesHandle::applyAllModuleIf( PlaYUVerModuleIf *pcCurrModuleIf )
{
  if( pcCurrModuleIf->m_pcModuleStream )
  {
    UInt numberOfWindows = pcCurrModuleIf->m_cModuleDef.m_uiNumberOfFrames;
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

Void ModulesHandle::openModuleIfStream( PlaYUVerModuleIf *pcCurrModuleIf )
{
  if( pcCurrModuleIf )
  {
    if( !pcCurrModuleIf->m_pcModuleStream )
    {
      UInt Width = 0, Height = 0, FrameRate = 30;
      Int InputFormat = -1;

      QString supported = tr( "Supported Files (" );
      QStringList formatsList;
      QStringList formatsExt = PlaYUVerStream::supportedWriteFormatsExt();
      QStringList formatsName = PlaYUVerStream::supportedWriteFormatsName();

      for( Int i = 0; i < formatsName.size(); i++ )
      {
        QString currFmt;
        supported.append( " *." );
        supported.append( formatsExt[i] );
        currFmt.append( formatsName[i] );
        currFmt.append( " (*." );
        currFmt.append( formatsExt[i] );
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
      FrameRate = pcCurrModuleIf->m_pcSubWindow[0]->getInputStream()->getFrameRate();

      pcCurrModuleIf->m_pcModuleStream = new PlaYUVerStream;
      if( !pcCurrModuleIf->m_pcModuleStream->open( fileName, Width, Height, InputFormat, FrameRate, false ) )
      {
        delete pcCurrModuleIf->m_pcModuleStream;
        pcCurrModuleIf->m_pcModuleStream = NULL;
        return;
      }
    }
  }
}

Bool ModulesHandle::showModuleIf( PlaYUVerModuleIf *pcCurrModuleIf, PlaYUVerFrame* processedFrame )
{
  Bool bRet = false;
  //if( processedFrame  )
  {
    if( pcCurrModuleIf->m_pcDisplaySubWindow )
    {
      pcCurrModuleIf->m_pcDisplaySubWindow->setCurrFrame( processedFrame );
      bRet = true;
    }
    else
    {
      pcCurrModuleIf->m_pcSubWindow[0]->setCurrFrame( processedFrame );
    }
  }
  return bRet;
}

Bool ModulesHandle::showModuleIf( PlaYUVerModuleIf *pcCurrModuleIf, Double moduleResult )
{
  Bool bRet = false;
  pcCurrModuleIf->m_pcModuleDock->setModulueReturnValue( moduleResult );
  return bRet;
}

Void ModulesHandle::swapModulesWindowsIf( PlaYUVerModuleIf *pcCurrModuleIf )
{
  if( pcCurrModuleIf->m_cModuleDef.m_uiNumberOfFrames == MODULE_REQUIRES_TWO_FRAMES )
  {
    SubWindowHandle* auxWindowHandle = pcCurrModuleIf->m_pcSubWindow[0];
    pcCurrModuleIf->m_pcSubWindow[0] = pcCurrModuleIf->m_pcSubWindow[1];
    pcCurrModuleIf->m_pcSubWindow[1] = auxWindowHandle;
  }
}

Void ModulesHandle::customEvent( QEvent *event )
{
  if( !event )
  {
    return;
  }
  PlaYUVerModuleIf::EventData *eventData = ( PlaYUVerModuleIf::EventData* )event;
  if( eventData->m_bSuccess )
  {
    switch( eventData->m_pcModule->m_cModuleDef.m_iModuleType )
    {
    case FRAME_PROCESSING_MODULE:
      showModuleIf( eventData->m_pcModule, eventData->m_pcModule->m_pcProcessedFrame );
      break;
    case FRAME_MEASUREMENT_MODULE:
      showModuleIf( eventData->m_pcModule, eventData->m_pcModule->m_dMeasurementResult );
      break;
    }
  }
}

QMenu* ModulesHandle::createMenus( QMenuBar *MainAppMenuBar )
{
  PlaYUVerModuleIf* pcCurrModuleIf;
  QAction* currAction;
  QMenu* currSubMenu;

  m_arrayActions.resize( MODULES_TOTAL_ACT );

  m_pcActionMapper = new QSignalMapper( this );
  connect( m_pcActionMapper, SIGNAL( mapped(int) ), this, SLOT( selectModule(int) ) );

  m_pcModulesMenu = MainAppMenuBar->addMenu( "&Modules" );

  for( Int i = 0; i < m_pcPlaYUVerModules.size(); i++ )
  {
    pcCurrModuleIf = m_pcPlaYUVerModules.at( i );
    pcCurrModuleIf->setParent( this );

    currSubMenu = NULL;
    if( pcCurrModuleIf->m_cModuleDef.m_pchModuleCategory )
    {
      for( Int j = 0; j < m_pcModulesSubMenuList.size(); j++ )
      {
        if( m_pcModulesSubMenuList.at( j )->title() == QString( pcCurrModuleIf->m_cModuleDef.m_pchModuleCategory ) )
        {
          currSubMenu = m_pcModulesSubMenuList.at( j );
          break;
        }
      }
      if( !currSubMenu )
      {
        currSubMenu = m_pcModulesMenu->addMenu( pcCurrModuleIf->m_cModuleDef.m_pchModuleCategory );
        m_pcModulesSubMenuList.append( currSubMenu );
      }
    }

    currAction = new QAction( pcCurrModuleIf->m_cModuleDef.m_pchModuleName, parent() );
    currAction->setStatusTip( pcCurrModuleIf->m_cModuleDef.m_pchModuleTooltip );
    currAction->setCheckable( true );
    connect( currAction, SIGNAL( triggered() ), m_pcActionMapper, SLOT( map() ) );
    m_pcActionMapper->setMapping( currAction, i );
    m_arrayModulesActions.append( currAction );

    if( currSubMenu )
      currSubMenu->addAction( currAction );
    else
      m_pcModulesMenu->addAction( currAction );

    pcCurrModuleIf->m_pcAction = currAction;
  }

  m_arrayActions[FORCE_NEW_WINDOW_ACT] = new QAction( "Use New Window", parent() );
  m_arrayActions[FORCE_NEW_WINDOW_ACT]->setStatusTip( "Show module result in a new window. Some modules already force this feature" );
  m_arrayActions[FORCE_NEW_WINDOW_ACT]->setCheckable( true );
  m_arrayActions[FORCE_PLAYING_REFRESH_ACT] = new QAction( "Refresh while playing", parent() );
  m_arrayActions[FORCE_PLAYING_REFRESH_ACT]->setStatusTip( "Force module refreshing while playing" );
  m_arrayActions[FORCE_PLAYING_REFRESH_ACT]->setCheckable( true );

  m_arrayActions[APPLY_ALL_ACT] = new QAction( "Apply to All", parent() );
  m_arrayActions[APPLY_ALL_ACT]->setStatusTip( "Apply module to all frames and save the result" );
  connect( m_arrayActions[APPLY_ALL_ACT], SIGNAL( triggered() ), m_pcActionMapper, SLOT( map() ) );
  m_pcActionMapper->setMapping( m_arrayActions[APPLY_ALL_ACT], APPLY_ALL_OPT );

  m_arrayActions[SWAP_FRAMES_ACT] = new QAction( "Swap frames", parent() );
  m_arrayActions[SWAP_FRAMES_ACT]->setStatusTip( "Swap Sub Window order" );
  connect( m_arrayActions[SWAP_FRAMES_ACT], SIGNAL( triggered() ), m_pcActionMapper, SLOT( map() ) );
  m_pcActionMapper->setMapping( m_arrayActions[SWAP_FRAMES_ACT], SWAP_FRAMES_OPT );

  m_arrayActions[DISABLE_ALL_ACT] = new QAction( "Disable All Modules", parent() );
  connect( m_arrayActions[DISABLE_ALL_ACT], SIGNAL( triggered() ), this, SLOT( destroyAllModulesIf() ) );

  m_pcModulesMenu->addSeparator();
  m_pcModulesMenu->addAction( m_arrayActions[APPLY_ALL_ACT] );
  m_pcModulesMenu->addAction( m_arrayActions[SWAP_FRAMES_ACT] );
  m_pcModulesMenu->addAction( m_arrayActions[DISABLE_ALL_ACT] );
  m_pcModulesMenu->addAction( m_arrayActions[FORCE_NEW_WINDOW_ACT] );

  return m_pcModulesMenu;
}

Void ModulesHandle::updateMenus( Bool hasSubWindow )
{
  QAction* currModuleAction;
  for( Int i = 0; i < m_pcPlaYUVerModules.size(); i++ )
  {
    currModuleAction = m_arrayModulesActions.at( i );
    currModuleAction->setEnabled( hasSubWindow );
  }
  m_arrayActions[APPLY_ALL_ACT]->setEnabled( hasSubWindow );
  m_arrayActions[SWAP_FRAMES_ACT]->setEnabled( hasSubWindow );
}

}  // NAMESPACE

