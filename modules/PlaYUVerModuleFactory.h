/*    This file is a part of PlaYUVer project
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
 * \file     PlaYUVerModuleFactory.h
 * \brief    PlaYUVer modules factory
 */

#ifndef __PLAYUVERMODULESFACTORY_H__
#define __PLAYUVERMODULESFACTORY_H__

#include <map>

// PlaYUVerLib
#include "lib/PlaYUVerModuleIf.h"

#define REGISTER_MODULE( X ) Register( "X", &( X::Create ) );

typedef PlaYUVerModuleIf* ( *CreateModuleFn )( void );
typedef std::map<const char*, CreateModuleFn> PlaYUVerModuleFactoryMap;

// Factory for creating instances of PlaYUVerModuleIf
class PlaYUVerModuleFactory
{
private:
  PlaYUVerModuleFactory();
  PlaYUVerModuleFactory& operator=( const PlaYUVerModuleFactory& ) { return *this; }
  PlaYUVerModuleFactoryMap m_FactoryMap;

public:
  ~PlaYUVerModuleFactory();

  static PlaYUVerModuleFactory* Get()
  {
    static PlaYUVerModuleFactory instance;
    return &instance;
  }

  Void Register( const char* moduleName, CreateModuleFn pfnCreate );
  Bool RegisterDl( const char* dlName );

  PlaYUVerModuleIf* CreateModule( const char* moduleName );
  PlaYUVerModuleFactoryMap& getMap() { return m_FactoryMap; }
};

#endif  // __PLAYUVERMODULESFACTORY_H__
