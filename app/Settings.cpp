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

#include "Settings.h"

namespace plaYUVer
{

Settings::Settings() :
        m_settings( "pixLRA", "plaYUVer" )
{
}
// - - - - - - - - - - - -  Main Window Settings - - - - - - - - - - - - - - - - 

QPoint Settings::mainWindowPos()
{
  return m_settings.value( "MainWindow/pos", QPoint( 200, 200 ) ).toPoint();
}

Void Settings::setMainWindowPos( const QPoint & pos )
{
  m_settings.setValue( "MainWindow/pos", pos );
}

QSize Settings::mainWindowSize()
{
  return m_settings.value( "MainWindow/size", QSize( 500, 400 ) ).toSize();
}

Void Settings::setMainWindowSize( const QSize & size )
{
  m_settings.setValue( "MainWindow/size", size );
}

QString Settings::lastOpenPath()
{
  return m_settings.value( "MainWindow/lastOpenPath", QDir::homePath() ).toString();
}

Void Settings::setLastOpenPath( const QString & path )
{
  m_settings.setValue( "MainWindow/lastOpenPath", path );
}

Int Settings::getSelectedTool()
{
  return m_settings.value( "MainWindow/SelectedTool", ViewArea::NavigationTool ).toInt();
}

Void Settings::setSelectedTool( Int tool )
{
  m_settings.setValue( "MainWindow/SelectedTool", tool );
}

Void Settings::setRecentFileList( PlaYUVerStreamInfoVector recentFileList )
{
  QVariant var;
  var.setValue<PlaYUVerStreamInfoVector>( recentFileList );
  m_settings.setValue("MainWindow/RecentFileList", var );
}

QVector<PlaYUVerStreamInfo> Settings::getRecentFileList()
{
  QVariant value = m_settings.value("MainWindow/RecentFileList");
  return value.value<PlaYUVerStreamInfoVector>();
}

// - - - - - - - - - - - - - - Grid Settings - - - - - - - - - - - - - - - - - -

GridManager::Style Settings::gridStyle()
{
  return ( GridManager::Style )m_settings.value( "Grid/style", 0 ).toInt();
}

void Settings::setGridStyle( GridManager::Style v )
{
  m_settings.setValue( "Grid/style", v );
}

QColor Settings::gridColor()
{
  return m_settings.value( "Grid/color", QColor( Qt::white ) ).value<QColor>();
}

Void Settings::setGridColor( const QColor & v )
{
  m_settings.setValue( "Grid/color", v );
}

Int Settings::gridHSpacing()
{
  return m_settings.value( "Grid/hSpacing", 16 ).toInt();
}

Void Settings::setGridHSpacing( Int v )
{
  m_settings.setValue( "Grid/hSpacing", v );
}

Int Settings::gridVSpacing()
{
  return m_settings.value( "Grid/vSpacing", 16 ).toInt();
}

Void Settings::setGridVSpacing( Int v )
{
  m_settings.setValue( "Grid/vSpacing", v );
}

}  // NAMESPACE
