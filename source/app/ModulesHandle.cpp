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
#include "SubWindowHandle.h"
#include "ModulesListHeader.h"
#include "ModulesListMacro.h"

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
  Bool bShowModulesNewWindow = m_arrayActions[FORCE_NEW_WINDOW_ACT]->isChecked();
  SubWindowHandle* pcSubWindow = qobject_cast<SubWindowHandle *>( m_pcMdiArea->activeSubWindow() );
  SubWindowHandle* interfaceChild = NULL;
  PlaYUVerModuleIf* currModuleIf = NULL;

  if( m_uiModuleSelected >= 0 )
    currModuleIf = m_pcPlaYUVerModules.at( m_uiModuleSelected );
  m_uiModuleSelected = -1;

  if( !currModuleIf )
    return NULL;

  if( currModuleIf->m_pcAction->isChecked() )
  {
    currModuleIf->m_pcDisplaySubWindow = NULL;
    if( ( currModuleIf->m_cModuleDef.m_uiModuleRequirements & MODULE_REQUIRES_NEW_WINDOW ) || bShowModulesNewWindow )
    {
      QString windowName;
      interfaceChild = new SubWindowHandle( m_pcParent );
      windowName.append( pcSubWindow->userFriendlyCurrentFile() );
      windowName.append( " [" );
      windowName.append( currModuleIf->m_cModuleDef.m_pchModuleName );
      windowName.append( "]" );
      interfaceChild->setWindowName( windowName );
      connect( interfaceChild->getViewArea(), SIGNAL( positionChanged(const QPoint &, PlaYUVerFrame *) ), this,
          SLOT( updatePixelValueStatusBar(const QPoint &, PlaYUVerFrame *) ) );
      currModuleIf->m_pcDisplaySubWindow = interfaceChild;
      currModuleIf->m_pcDisplaySubWindow->setModule( currModuleIf );
    }
    currModuleIf->m_pcSubWindow[0] = pcSubWindow;
    pcSubWindow->enableModule( currModuleIf );
  }
  else
  {
    QCoreApplication::processEvents();
    destroyModuleIf( currModuleIf );
  }
  return interfaceChild;
}

PlaYUVerFrame* ModulesHandle::applyModuleIf( PlaYUVerModuleIf *pcCurrModuleIf )
{
  PlaYUVerFrame* processedFrame;

  switch( pcCurrModuleIf->m_cModuleDef.m_uiNumberOfFrames )
  {
  case MODULE_REQUIRES_ONE_FRAME:
    processedFrame = pcCurrModuleIf->process( pcCurrModuleIf->m_pcSubWindow[0]->getCurrFrame() );
    break;
  case MODULE_REQUIRES_TWO_FRAMES:
    break;
  case MODULE_REQUIRES_THREE_FRAMES:
    break;
  }
  if( pcCurrModuleIf->m_pcDisplaySubWindow )
    pcCurrModuleIf->m_pcDisplaySubWindow->setCurrFrame( processedFrame );
  else
    pcCurrModuleIf->m_pcSubWindow[0]->setCurrFrame( processedFrame );
  return processedFrame;
}

Void ModulesHandle::destroyModuleIf( PlaYUVerModuleIf *pcCurrModuleIf )
{
  pcCurrModuleIf->m_pcAction->setChecked( false );
  if( pcCurrModuleIf->m_pcDisplaySubWindow )
    pcCurrModuleIf->m_pcDisplaySubWindow->close();

  for( Int i = 0; i < MAX_NUMBER_FRAMES; i++ )
  {
    if( pcCurrModuleIf->m_pcSubWindow[i] )
    {
      pcCurrModuleIf->m_pcSubWindow[i]->disableModule();
      pcCurrModuleIf->m_pcSubWindow[i] = NULL;
    }
  }
  pcCurrModuleIf->m_pcDisplaySubWindow = NULL;
  pcCurrModuleIf->destroy();
}

Void ModulesHandle::destroyAllModulesIf()
{
  for( Int i = 0; i < m_pcPlaYUVerModules.size(); i++ )
  {
    destroyModuleIf( m_pcPlaYUVerModules.at( i ) );
  }
}

QMenu* ModulesHandle::createMenus( QMenuBar *MainAppMenuBar )
{
  PlaYUVerModuleIf* currModuleIf;
  QAction* currAction;
  QMenu* currSubMenu;

  m_arrayActions.resize( MODULES_TOTAL_ACT );

  m_pcActionMapper = new QSignalMapper( this );
  connect( m_pcActionMapper, SIGNAL( mapped(int) ), this, SLOT( selectModule(int) ) );

  m_pcModulesMenu = MainAppMenuBar->addMenu( "&Modules" );

//m_arrayModulesActions.resize( m_uiModulesCount );

  for( Int i = 0; i < m_pcPlaYUVerModules.size(); i++ )
  {
    currModuleIf = m_pcPlaYUVerModules.at( i );

    currSubMenu = NULL;
    if( currModuleIf->m_cModuleDef.m_pchModuleCategory )
    {
      for( Int j = 0; j < m_pcModulesSubMenuList.size(); j++ )
      {
        if( m_pcModulesSubMenuList.at( j )->title() == QString( currModuleIf->m_cModuleDef.m_pchModuleCategory ) )
        {
          currSubMenu = m_pcModulesSubMenuList.at( j );
          break;
        }
      }
      if( !currSubMenu )
      {
        currSubMenu = m_pcModulesMenu->addMenu( currModuleIf->m_cModuleDef.m_pchModuleCategory );
        m_pcModulesSubMenuList.append( currSubMenu );
      }
    }

    currAction = new QAction( currModuleIf->m_cModuleDef.m_pchModuleName, parent() );
    currAction->setStatusTip( currModuleIf->m_cModuleDef.m_pchModuleTooltip );
    currAction->setCheckable( true );
    connect( currAction, SIGNAL( triggered() ), m_pcActionMapper, SLOT( map() ) );
    m_pcActionMapper->setMapping( currAction, i );
    m_arrayModulesActions.append( currAction );

    if( currSubMenu )
      currSubMenu->addAction( currAction );
    else
      m_pcModulesMenu->addAction( currAction );

    currModuleIf->m_pcAction = currAction;
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

