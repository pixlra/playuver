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
 * \file     PlaYUVerModuleFactory.cpp
 * \brief    PlaYUVer modules factory
 */

#include "config.h"
#include <functional>
#include <cstring>
#ifdef USE_DYNLOAD
#include <dlfcn.h>
#endif
#include "PlaYUVerModuleFactory.h"
#include "ModulesListHeader.h"

namespace plaYUVer
{

PlaYUVerModuleFactory::PlaYUVerModuleFactory()
{
  REGISTER_ALL_MODULES
}

PlaYUVerModuleFactory::~PlaYUVerModuleFactory()
{
  m_FactoryMap.clear();
}

Void PlaYUVerModuleFactory::Register( const char* moduleName, CreateModuleFn pfnCreate )
{
  m_FactoryMap[moduleName] = pfnCreate;
}

Bool PlaYUVerModuleFactory::RegisterDl( const char* dlName )
{
#ifdef USE_DYNLOAD
  void *pHndl = dlopen( dlName, RTLD_NOW );
  if( pHndl == NULL )
  {
    std::cerr << dlerror()
              << std::endl;
    return false;
  }
  CreateModuleFn pfnCreate = ( CreateModuleFn )dlsym( pHndl, "Maker" );
  if( pfnCreate == NULL )
  {
    return false;
  }
  Register( dlName, pfnCreate );
  return true;
#else
  return false;
#endif
}

PlaYUVerModuleIf *PlaYUVerModuleFactory::CreateModule( const char* moduleName )
{
  PlaYUVerModuleFactoryMap& PlaYUVerModuleFactoryMap = PlaYUVerModuleFactory::Get()->getMap();
  PlaYUVerModuleFactoryMap::iterator it = PlaYUVerModuleFactoryMap.begin();
  for( ; it != PlaYUVerModuleFactoryMap.end(); ++it )
  {
    if( !strcmp( it->first, moduleName ) )
    {
      return it->second();
    }
  }
  return NULL;
}

}  // NAMESPACE
