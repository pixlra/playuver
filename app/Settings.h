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
 * \file     Settings.h
 * \brief    App settings
 */

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#include <QtCore>
#include "GridManager.h"
#include "SubWindowHandle.h"

namespace plaYUVer
{

class Settings
{

public:
  Settings();

  // - - - - - - - - - - Main Window Settings - - - - - - - - - - - - - 
  QPoint mainWindowPos();
  Void setMainWindowPos( const QPoint &v );

  QSize mainWindowSize();
  Void setMainWindowSize( const QSize &v );

  QString lastOpenPath();
  Void setLastOpenPath( const QString &v );

  Int getSelectedTool();
  Void setSelectedTool( Int);

  Bool getRepeat();
  Bool getVideoLock();
  Void setPlayingSettings( Bool repeat, Bool lock );

  Void setDockVisibility( Bool statusStreamProperties, Bool statusFrameProperties, Bool statusQualityMeasurement );
  Void getDockVisibility( Bool& statusStreamProperties, Bool& statusFrameProperties, Bool& statusQualityMeasurement );

  // - - - - - - - - - - Recent Files Settings - - - - - - - - - - - - -
  Void setRecentFileList( PlaYUVerStreamInfoVector recentFileList );
  PlaYUVerStreamInfoVector getRecentFileList();

  // - - - - - - - - - - - - Grid Settings - - - - - - - - - - - - - - -
  GridManager::Style gridStyle();
  Void setGridStyle( GridManager::Style v );

  QColor gridColor();
  Void setGridColor( const QColor &v );

  Int gridHSpacing();
  Void setGridHSpacing( Int v );

  Int gridVSpacing();
  Void setGridVSpacing( Int v );

private:
  QSettings m_settings;
};

}  // NAMESPACE

#endif  // __Settings_H__
