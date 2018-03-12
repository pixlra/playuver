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
 * \file     CalypModuleIf.h
 * \ingroup  CalypLibGrp
 * \brief    Calyp modules interface
 */

#ifndef __CALYPMODULESIF_H__
#define __CALYPMODULESIF_H__

/**
 * @defgroup Calyp_Modules Calyp Modules
 * @{
 * CalypLib supports the creation of independent
 * frame processing modules
 * @}
 */

#include "CalypFrame.h"
#include "CalypOptions.h"

#define REGISTER_CLASS_MAKER( X ) \
  extern "C" CalypModuleIf* Maker() { return new X; }
#define REGISTER_CLASS_FACTORY( X )                  \
public:                                              \
  static CalypModuleIf* Create() { return new X(); } \
  void Delete() { delete this; }
#define _BASIC_MODULE_API_2_CHECK_                        \
  if( apcFrameList.size() != m_uiNumberOfFrames )         \
    return false;                                         \
  for( unsigned int i = 0; i < apcFrameList.size(); i++ ) \
    if( !apcFrameList[i] )                                \
      return false;

/** Module_API_Version Enum
 * \ingroup Calyp_Modules
 * Version of the modules API required
 * @see m_iModuleAPI
 */
enum Module_API_Version
{
  CLP_MODULE_API_1,
  CLP_MODULE_API_2,
};

/** Module_Type Enum
 * \ingroup Calyp_Modules
 * Type of module
 * @see m_iModuleType
 */
enum Module_Type
{
  CLP_FRAME_PROCESSING_MODULE,
  CLP_FRAME_MEASUREMENT_MODULE,
  CLP_MODULE_TYPE_MAX = 255,
};

/** Module_Features Enum
 * \ingroup Calyp_Modules
 * Features/Requirements of a module
 * @see m_uiModuleRequirements
 */
enum Module_Features
{
  CLP_MODULE_REQUIRES_NOTHING = 0,
  CLP_MODULE_REQUIRES_SKIP_WHILE_PLAY = 1,
  CLP_MODULE_REQUIRES_OPTIONS = 2,
  CLP_MODULE_REQUIRES_NEW_WINDOW = 4,
  CLP_MODULE_USES_KEYS = 8,
  CLP_MODULES_VARIABLE_NUM_FRAMES = 16,
  CLP_MODULE_REQURES_MAX = 1024,
};

enum Module_Key_Supported
{
  MODULE_KEY_LEFT,
  MODULE_KEY_RIGHT,
  MODULE_KEY_UP,
  MODULE_KEY_DOWN,
};

/**
 * \class    CalypModuleIf
 * \ingroup  CalypLib Calyp_Modules
 * \brief    Abstract class for modules
 */
class CalypModuleIf
{
public:
  int m_iModuleAPI;
  int m_iModuleType;
  const char* m_pchModuleCategory;
  const char* m_pchModuleName;
  const char* m_pchModuleTooltip;
  const char* m_pchModuleLongName;

  //! Number of frames
  unsigned int m_uiNumberOfFrames;
  //! Features/Requirements
  unsigned int m_uiModuleRequirements;

  CalypOptions m_cModuleOptions;

  CalypModuleIf()
  {
    m_iModuleAPI = CLP_MODULE_API_1;
    m_uiModuleRequirements = CLP_MODULE_REQUIRES_NOTHING;
    m_pchModuleLongName = NULL;
  }
  virtual ~CalypModuleIf() {}
  virtual void Delete() = 0;
  virtual void destroy() = 0;

  const char* getModuleLongName()
  {
    return m_pchModuleLongName ? m_pchModuleLongName : m_pchModuleName;
  }

  /**
   * Module API version 1
   */
  virtual void create( CalypFrame* ) {}
  virtual CalypFrame* process( CalypFrame* ) { return NULL; }
  virtual double measure( CalypFrame* ) { return 0; }
  /**
   * Module API version 2
   */
  virtual bool create( std::vector<CalypFrame*> apcFrameList ) { return false; }
  virtual CalypFrame* process( std::vector<CalypFrame*> ) { return NULL; }
  virtual double measure( std::vector<CalypFrame*> ) { return 0; }
  virtual bool keyPressed( enum Module_Key_Supported ) { return false; }
};

#endif  // __CALYPMODULESIF_H__
