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
 * \file     playuversettings.h
 * \brief    plaYUVer app settings
 */

#ifndef __PLAYUVERSETTINGS_H__
#define __PLAYUVERSETTINGS_H__

#include "config.h"

#include <QtCore>

#include "GridManager.h"

namespace plaYUVer
{

class PlaYUVerSettings
{

public:
  PlaYUVerSettings();

  // - - - - - - - - - - Main Window Settings - - - - - - - - - - - - - 
  QPoint mainWindowPos();
  void setMainWindowPos( const QPoint &v );

  QSize mainWindowSize();
  void setMainWindowSize( const QSize &v );

  QString lastOpenPath();
  void setLastOpenPath( const QString &v );

  // - - - - - - - - - - - - Grid Settings - - - - - - - - - - - - - - -
  GridManager::Style gridStyle();
  void setGridStyle( GridManager::Style v );

  QColor gridColor();
  void setGridColor( const QColor &v );

  int gridHSpacing();
  void setGridHSpacing( int v );

  int gridVSpacing();
  void setGridVSpacing( int v );

private:
  QSettings m_settings;
};

}  // NAMESPACE

#endif  // __PLAYUVERSETTINGS_H__
