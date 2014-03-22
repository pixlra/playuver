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
 * \file     PlaYUVerSettings.h
 * \brief    plaYUVer app settings
 */

#include "PlaYUVerSettings.h"

namespace plaYUVer
{

PlaYUVerSettings::PlaYUVerSettings() :
    m_settings( "IT", "YUV Player" )
{
}
// - - - - - - - - - - - -  Main Window Settings - - - - - - - - - - - - - - - - 

QPoint PlaYUVerSettings::mainWindowPos()
{
  return m_settings.value( "MainWindow/pos", QPoint( 200, 200 ) ).toPoint();
}

void PlaYUVerSettings::setMainWindowPos( const QPoint & pos )
{
  m_settings.setValue( "MainWindow/pos", pos );
}

QSize PlaYUVerSettings::mainWindowSize()
{
  return m_settings.value( "MainWindow/size", QSize( 500, 400 ) ).toSize();
}

void PlaYUVerSettings::setMainWindowSize( const QSize & size )
{
  m_settings.setValue( "MainWindow/size", size );
}

QString PlaYUVerSettings::lastOpenPath()
{
  return m_settings.value( "MainWindow/lastOpenPath", QDir::homePath() ).toString();
}

void PlaYUVerSettings::setLastOpenPath( const QString & path )
{
  m_settings.setValue( "MainWindow/lastOpenPath", path );
}

// - - - - - - - - - - - - - - Grid Settings - - - - - - - - - - - - - - - - - -

GridManager::Style PlaYUVerSettings::gridStyle()
{
  return ( GridManager::Style )m_settings.value( "Grid/style", 0 ).toInt();
}

void PlaYUVerSettings::setGridStyle( GridManager::Style v )
{
  m_settings.setValue( "Grid/style", v );
}

QColor PlaYUVerSettings::gridColor()
{
  return m_settings.value( "Grid/color", QColor( Qt::white ) ).value<QColor>();
}

void PlaYUVerSettings::setGridColor( const QColor & v )
{
  m_settings.setValue( "Grid/color", v );
}

int PlaYUVerSettings::gridHSpacing()
{
  return m_settings.value( "Grid/hSpacing", 16 ).toInt();
}

void PlaYUVerSettings::setGridHSpacing( int v )
{
  m_settings.setValue( "Grid/hSpacing", v );
}

int PlaYUVerSettings::gridVSpacing()
{
  return m_settings.value( "Grid/vSpacing", 16 ).toInt();
}

void PlaYUVerSettings::setGridVSpacing( int v )
{
  m_settings.setValue( "Grid/vSpacing", v );
}

}  // NAMESPACE
