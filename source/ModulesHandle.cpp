/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by plaYUVer developers
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
#include "FilterFrame.h"

namespace plaYUVer
{

ModulesHandle::ModulesHandle( QWidget * parent )
{
  // Register Modules
  REGISTER_MODULE( FilterFrame );


  // configure class
  setParent( parent );
  m_uiModulesCount = 0;
  m_uiModuleSelected= -1;
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

PlaYUVerModuleIf* ModulesHandle::getSelectedModuleIf()
{
  PlaYUVerModuleIf* currModuleIf = NULL;

  if( m_uiModuleSelected >= 0 )
    //if( m_arrayModulesActions.at(m_uiModuleSelected)->isChecked() )
      currModuleIf =  m_pcPlaYUVerModules.at( m_uiModuleSelected );

  m_uiModuleSelected = -1;
  return currModuleIf;
}

QMenu* ModulesHandle::createMenus( QMenuBar *MainAppMenuBar )
{
  PlaYUVerModuleIf* currModuleIf;
  QAction* currAction;
  Bool bCategoryExists = false;

  m_pcActionMapper = new QSignalMapper( this );
  connect( m_pcActionMapper, SIGNAL( mapped(int) ), this, SLOT( selectModule(int) ) );

  m_pcModulesMenu = MainAppMenuBar->addMenu( "&Modules" );

  //m_arrayModulesActions.resize( m_uiModulesCount );
Int total = 0;
  for( Int i = 0; i < m_pcPlaYUVerModules.size(); i++ )
  {
    currModuleIf = m_pcPlaYUVerModules.at( i );

    for( Int j = 0; j < total; j++ )
    {

    }

    currAction = new QAction( tr( currModuleIf->m_cModuleDef.m_pchModuleName ), parent() );
    currAction->setStatusTip( tr( currModuleIf->m_cModuleDef.m_pchModuleTooltip ) );
    currAction->setCheckable( true );
    connect( currAction, SIGNAL( triggered() ), m_pcActionMapper, SLOT( map() ) );
    m_pcActionMapper->setMapping( currAction, i );
    m_arrayModulesActions.append( currAction );

    m_pcModulesMenu->addAction( currAction );

    currModuleIf->m_pcAction = currAction;
  }
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

