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
  setParent( parent );
  m_uiModulesCount = 0;

  // Register Modules
  REGISTER_MODULE( FilterFrame );
}

ModulesHandle::~ModulesHandle()
{

}

Void ModulesHandle::appendModule( PlaYUVerModuleIf* pIfModule )
{
  m_pcPlaYUVerModules.append( pIfModule );
    m_uiModulesCount++;

}

Void ModulesHandle::createMenus( QMenuBar *MainAppMenuBar )
{
  PlaYUVerModuleIf* currModuleIf;
  QAction* currAction;

  m_pcModulesMenu = MainAppMenuBar->addMenu( "&Modules" );

  //m_arrayModulesActions.resize( m_uiModulesCount );

  for( Int i = 0; i < m_pcPlaYUVerModules.size(); i++ )
  {

    currModuleIf = m_pcPlaYUVerModules.at( i );

    currAction = new QAction( tr( currModuleIf->m_pchModuleName ), parent() );
    currAction->setStatusTip( tr( currModuleIf->m_pchModuleTooltip ) );

    m_arrayModulesActions.append( currAction );

    m_pcModulesMenu->addAction( currAction );
  }

  //MainAppMenuBar->
}

}  // NAMESPACE

