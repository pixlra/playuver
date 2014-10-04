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
 * \file     PlaYUVerSettings.h
 * \brief    plaYUVer app settings
 */

#include "PlaYUVerSettings.h"

namespace plaYUVer
{

PlaYUVerSettings::PlaYUVerSettings() :
        m_settings( "pixLRA", "plaYUVer" )
{
}
// - - - - - - - - - - - -  Main Window Settings - - - - - - - - - - - - - - - - 

QPoint PlaYUVerSettings::mainWindowPos()
{
  return m_settings.value( "MainWindow/pos", QPoint( 200, 200 ) ).toPoint();
}

Void PlaYUVerSettings::setMainWindowPos( const QPoint & pos )
{
  m_settings.setValue( "MainWindow/pos", pos );
}

QSize PlaYUVerSettings::mainWindowSize()
{
  return m_settings.value( "MainWindow/size", QSize( 500, 400 ) ).toSize();
}

Void PlaYUVerSettings::setMainWindowSize( const QSize & size )
{
  m_settings.setValue( "MainWindow/size", size );
}

QString PlaYUVerSettings::lastOpenPath()
{
  return m_settings.value( "MainWindow/lastOpenPath", QDir::homePath() ).toString();
}

Void PlaYUVerSettings::setLastOpenPath( const QString & path )
{
  m_settings.setValue( "MainWindow/lastOpenPath", path );
}

Int PlaYUVerSettings::getSelectedTool()
{
  return m_settings.value( "MainWindow/SelectedTool", NavigationTool ).toInt();
}

Void PlaYUVerSettings::setSelectedTool( Int tool )
{
  m_settings.setValue( "MainWindow/SelectedTool", tool );
}

Void PlaYUVerSettings::setDockVisibility( Bool statusStreamProperties, Bool statusFrameProperties, Bool statusQualityMeasurement )
{
  m_settings.setValue( "Dock/StreamProperties", statusStreamProperties );
  m_settings.setValue( "Dock/FrameProperties", statusFrameProperties );
  m_settings.setValue( "Dock/QualityMeasurement", statusQualityMeasurement );
}

Void PlaYUVerSettings::getDockVisibility( Bool& statusStreamProperties, Bool& statusFrameProperties, Bool& statusQualityMeasurement )
{
  statusStreamProperties = m_settings.value( "Dock/StreamProperties", true ).toBool();
  statusFrameProperties = m_settings.value( "Dock/FrameProperties", true ).toBool();
  statusQualityMeasurement = m_settings.value( "Dock/QualityMeasurement", true ).toBool();
}

// - - - - - - - - - - - -  PLaying Settings - - - - - - - - - - - - - - - -

Bool PlaYUVerSettings::getRepeat()
{
  return m_settings.value( "Playing/Repeat", false ).toBool();
}
Bool PlaYUVerSettings::getVideoLock()
{
  return m_settings.value( "Playing/VideoLock", false ).toBool();
}
Void PlaYUVerSettings::setPlayingSettings( Bool repeat, Bool lock )
{
  m_settings.setValue( "Playing/Repeat", repeat );
  m_settings.setValue( "Playing/VideoLock", lock );
}

Void PlaYUVerSettings::setRecentFileList( PlaYUVerStreamInfoVector recentFileList )
{
  QVariant var;
  var.setValue<PlaYUVerStreamInfoVector>( recentFileList );
  m_settings.setValue("MainWindow/RecentFileList", var );
}

QVector<PlaYUVerStreamInfo> PlaYUVerSettings::getRecentFileList()
{
  QVariant value = m_settings.value("MainWindow/RecentFileList");
  return value.value<PlaYUVerStreamInfoVector>();
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

Void PlaYUVerSettings::setGridColor( const QColor & v )
{
  m_settings.setValue( "Grid/color", v );
}

Int PlaYUVerSettings::gridHSpacing()
{
  return m_settings.value( "Grid/hSpacing", 16 ).toInt();
}

Void PlaYUVerSettings::setGridHSpacing( Int v )
{
  m_settings.setValue( "Grid/hSpacing", v );
}

Int PlaYUVerSettings::gridVSpacing()
{
  return m_settings.value( "Grid/vSpacing", 16 ).toInt();
}

Void PlaYUVerSettings::setGridVSpacing( Int v )
{
  m_settings.setValue( "Grid/vSpacing", v );
}

}  // NAMESPACE
