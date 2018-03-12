/*    This file is a part of Calyp project
 *    Copyright (C) 2014-2018  by Joao Carreira   (jfmcarreira@gmail.com)
 *                                Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     CalypModuleFactory.h
 * \brief    Calyp modules factory
 */

#ifndef __CALYPMODULESFACTORY_H__
#define __CALYPMODULESFACTORY_H__

#include <map>

// CalypLib
#include "lib/CalypModuleIf.h"

#define REGISTER_MODULE( X ) Register( "X", &( X::Create ) );

typedef CalypModuleIf* ( *CreateModuleFn )( void );
typedef std::map<const char*, CreateModuleFn> CalypModulesFactoryMap;

// Factory for creating instances of CalypModuleIf
class CalypModulesFactory
{
private:
  CalypModulesFactory();
  CalypModulesFactory& operator=( const CalypModulesFactory& ) { return *this; }
  CalypModulesFactoryMap m_FactoryMap;

public:
  ~CalypModulesFactory();

  static CalypModulesFactory* Get()
  {
    static CalypModulesFactory instance;
    return &instance;
  }

  void Register( const char* moduleName, CreateModuleFn pfnCreate );
  bool RegisterDl( const char* dlName );

  CalypModuleIf* CreateModule( const char* moduleName );
  CalypModulesFactoryMap& getMap() { return m_FactoryMap; }
};

#endif  // __CALYPMODULESFACTORY_H__
