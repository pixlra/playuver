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

ModulesHandle::ModulesHandle( QWidget * parent, QMdiArea *mdiArea ) :
        QObject( parent ),
        m_pcParent( parent ),
        m_pcMdiArea( mdiArea ),
        m_uiModulesCount( 0 ),
        m_uiModuleSelected( -1 )
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

void ModulesHandle::selectModule( int index )
{
  m_uiModuleSelected = index;
}

SubWindowHandle* ModulesHandle::toggleSelectedModuleIf()
{
  PlaYUVerModuleIf* pcCurrModuleIf = NULL;

  if( m_uiModuleSelected >= 0 )
    pcCurrModuleIf = m_pcPlaYUVerModules.at( m_uiModuleSelected );
  m_uiModuleSelected = -1;

  if( !pcCurrModuleIf )
    return NULL;

  if( pcCurrModuleIf->m_pcAction->isChecked() )
  {
    return enableModuleIf( pcCurrModuleIf );
  }
  else
  {
    destroyModuleIf( pcCurrModuleIf );
    return NULL;
  }
  return NULL;
}

SubWindowHandle* ModulesHandle::enableModuleIf( PlaYUVerModuleIf *pcCurrModuleIf )
{
  Bool bShowModulesNewWindow = m_arrayActions[FORCE_NEW_WINDOW_ACT]->isChecked();
  SubWindowHandle* pcSubWindow = qobject_cast<SubWindowHandle *>( m_pcMdiArea->activeSubWindow() );
  SubWindowHandle* interfaceChild = NULL;
  QString windowName;

  UInt numberOfFrames = pcCurrModuleIf->m_cModuleDef.m_uiNumberOfFrames;
  if( numberOfFrames > MODULE_REQUIRES_ONE_FRAME )
  {
    DialogSubWindowSelector dialogWindowsSelection( m_pcParent, m_pcMdiArea, numberOfFrames );
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
            pcCurrModuleIf->m_pcSubWindow[j] = subWindow;
          }
        }
      }
    }
    else
    {
      pcCurrModuleIf->m_pcAction->setChecked( false );
      return interfaceChild;
    }
    windowName.append( " [" );
    windowName.append( pcCurrModuleIf->m_cModuleDef.m_pchModuleName );
    windowName.append( "]" );
  }
  else
  {
    windowName.append( pcSubWindow->userFriendlyCurrentFile() );
    windowName.append( " [" );
    windowName.append( pcCurrModuleIf->m_cModuleDef.m_pchModuleName );
    windowName.append( "]" );
    pcCurrModuleIf->m_pcSubWindow[0] = pcSubWindow;
  }

  pcCurrModuleIf->m_pcDisplaySubWindow = NULL;
  if( ( pcCurrModuleIf->m_cModuleDef.m_uiModuleRequirements & MODULE_REQUIRES_NEW_WINDOW ) || bShowModulesNewWindow )
  {
    interfaceChild = new SubWindowHandle( m_pcParent );
    interfaceChild->setWindowName( windowName );
    connect( interfaceChild->getViewArea(), SIGNAL( positionChanged(const QPoint &, PlaYUVerFrame *) ), this,
        SLOT( updatePixelValueStatusBar(const QPoint &, PlaYUVerFrame *) ) );
    pcCurrModuleIf->m_pcDisplaySubWindow = interfaceChild;
    pcCurrModuleIf->m_pcDisplaySubWindow->setModule( pcCurrModuleIf );
  }

  pcCurrModuleIf->create( pcCurrModuleIf->m_pcSubWindow[0]->getCurrFrame() );

  for( UInt i = 0; i < numberOfFrames; i++ )
  {
    pcCurrModuleIf->m_pcSubWindow[i]->enableModule( pcCurrModuleIf );
  }
  return interfaceChild;
}

Void ModulesHandle::destroyModuleIf( PlaYUVerModuleIf *pcCurrModuleIf )
{
  pcCurrModuleIf->m_pcAction->setChecked( false );
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

  pcCurrModuleIf->destroy();
}

Void ModulesHandle::destroyAllModulesIf()
{
  for( Int i = 0; i < m_pcPlaYUVerModules.size(); i++ )
  {
    destroyModuleIf( m_pcPlaYUVerModules.at( i ) );
  }
}

Bool ModulesHandle::applyModuleIf( PlaYUVerModuleIf *pcCurrModuleIf, Bool isPlaying )
{
  PlaYUVerFrame* processedFrame = NULL;
  Bool bRet = false;
  if( !isPlaying || ( isPlaying && pcCurrModuleIf->m_cModuleDef.m_bApplyWhilePlaying ) )
  {
    switch( pcCurrModuleIf->m_cModuleDef.m_uiNumberOfFrames )
    {
    case MODULE_REQUIRES_ONE_FRAME:
      processedFrame = pcCurrModuleIf->process( pcCurrModuleIf->m_pcSubWindow[0]->getCurrFrame() );
      break;
    case MODULE_REQUIRES_TWO_FRAMES:
      processedFrame = pcCurrModuleIf->process( pcCurrModuleIf->m_pcSubWindow[0]->getCurrFrame(), pcCurrModuleIf->m_pcSubWindow[1]->getCurrFrame() );
      break;
    case MODULE_REQUIRES_THREE_FRAMES:
      processedFrame = pcCurrModuleIf->process( pcCurrModuleIf->m_pcSubWindow[0]->getCurrFrame(), pcCurrModuleIf->m_pcSubWindow[1]->getCurrFrame(),
          pcCurrModuleIf->m_pcSubWindow[2]->getCurrFrame() );
      break;
    }
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
  else
  {
    bRet = true;
  }
  return bRet;
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

//m_arrayModulesActions.resize( m_uiModulesCount );

  for( Int i = 0; i < m_pcPlaYUVerModules.size(); i++ )
  {
    pcCurrModuleIf = m_pcPlaYUVerModules.at( i );

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

  m_arrayActions[DISABLE_ALL_ACT] = new QAction( "Disable All Modules", parent() );
  connect( m_arrayActions[DISABLE_ALL_ACT], SIGNAL( triggered() ), this, SLOT( destroyAllModulesIf() ) );
  m_arrayActions[FORCE_NEW_WINDOW_ACT] = new QAction( "Use New Window", parent() );
  m_arrayActions[FORCE_NEW_WINDOW_ACT]->setStatusTip( "Show module result in a new window. Some modules already force this feature" );
  m_arrayActions[FORCE_NEW_WINDOW_ACT]->setCheckable( true );

  m_pcModulesMenu->addSeparator();
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
}

}  // NAMESPACE

