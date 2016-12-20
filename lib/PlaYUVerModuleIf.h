/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2016  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     PlaYUVerModuleIf.h
 * \ingroup  PlaYUVerLib_Modules
 * \brief    PlaYUVer modules interface
 */

#ifndef __PLAYUVERMODULESIF_H__
#define __PLAYUVERMODULESIF_H__

/**
 * \ingroup  PlaYUVerLib
 * @defgroup PlaYUVerLib_Modules PlaYUVer Modules
 * @{
 * PlaYUVerLib supports the creation of independent
 * frame processing modules
 * @}
 */

#include "PlaYUVerDefs.h"
#include "PlaYUVerFrame.h"
#include "PlaYUVerOptions.h"

#define REGISTER_CLASS_MAKER(X) \
  extern "C" PlaYUVerModuleIf* Maker() { return new X; }

#define REGISTER_CLASS_FACTORY(X) \
public: \
  static PlaYUVerModuleIf* Create() { return new X(); } \
  void Delete() { delete this; }

#define _BASIC_MODULE_API_2_CHECK_ \
if( apcFrameList.size() != m_uiNumberOfFrames ) \
  return false; \
for( UInt i = 0; i < apcFrameList.size(); i++ ) \
  if( !apcFrameList[i] ) \
    return false;

/** Module_API_Version Enum
 * \ingroup PlaYUVerLib_Modules
 * Version of the modules API required
 * @see m_iModuleAPI
 */
enum Module_API_Version
{
  MODULE_API_1,
  MODULE_API_2,
};

/** Module_Type Enum
 * \ingroup PlaYUVerLib_Modules
 * Type of module
 * @see m_iModuleType
 */
enum Module_Type
{
  FRAME_PROCESSING_MODULE,
  FRAME_MEASUREMENT_MODULE,
};

/** Module_NumberOfFrames Enum
 * \ingroup PlaYUVerLib_Modules
 * Number of frames requires by a module
 * @see m_uiNumberOfFrames
 */
enum Module_NumberOfFrames
{
  MODULE_REQUIRES_ONE_FRAME = 1,
  MODULE_REQUIRES_TWO_FRAMES,
  MODULE_REQUIRES_THREE_FRAMES,
  MODULE_REQUIRES_FOUR_FRAMES,
  MODULE_REQUIRES_FIVE_FRAMES,
  MODULE_REQUIRES_SIX_FRAMES,
  MODULE_REQUIRES_SEVERAL_FRAMES = 255,
  MODULE_REQUIRES_MAX_NUM_FRAMES,
};

/** Module_Features Enum
 * \ingroup PlaYUVerLib_Modules
 * Features/Requirements of a module
 * @see m_uiModuleRequirements
 */
enum Module_Features
{
  MODULE_REQUIRES_NOTHING = 0,
  MODULE_REQUIRES_SKIP_WHILE_PLAY = 1,
  MODULE_REQUIRES_OPTIONS = 2,
  MODULE_REQUIRES_NEW_WINDOW = 4,
  MODULE_USES_KEYS = 8,
};

enum Module_Key_Supported
{
  MODULE_KEY_LEFT,
  MODULE_KEY_RIGHT,
  MODULE_KEY_UP,
  MODULE_KEY_DOWN,
};

/**
 * \class    PlaYUVerModuleIf
 * \ingroup  PlaYUVerLib PlaYUVerLib_Modules
 * \brief    Abstract class for modules
 */
class PlaYUVerModuleIf
{
public:

  Int m_iModuleAPI;
  Int m_iModuleType;
  const Char* m_pchModuleCategory;
  const Char* m_pchModuleName;
  const Char* m_pchModuleTooltip;

  //! Number of frames
  enum Module_NumberOfFrames m_uiNumberOfFrames;
  //! Features/Requirements
  UInt m_uiModuleRequirements;

  PlaYUVerOptions m_cModuleOptions;

  PlaYUVerModuleIf()
  {
    m_iModuleAPI = MODULE_API_1;
    m_uiModuleRequirements = MODULE_REQUIRES_NOTHING;
  }
  virtual ~PlaYUVerModuleIf()
  {
  }

  virtual void Delete() = 0;
  virtual Void destroy() = 0;

  /**
   * Module API version 1
   */
  virtual Void create( PlaYUVerFrame* )
  {
  }
  virtual PlaYUVerFrame* process( PlaYUVerFrame* )
  {
    return NULL;
  }
  virtual Double measure( PlaYUVerFrame* )
  {
    return 0;
  }

  /**
   * Module API version 2
   */
  virtual Bool create( std::vector<PlaYUVerFrame*> apcFrameList )
  {
    return false;
  }
  virtual PlaYUVerFrame* process( std::vector<PlaYUVerFrame*> )
  {
    return NULL;
  }
  virtual Double measure( std::vector<PlaYUVerFrame*> )
  {
    return 0;
  }

  virtual Bool keyPressed( enum Module_Key_Supported )
  {
    return false;
  }

};

#endif // __PLAYUVERMODULESIF_H__

