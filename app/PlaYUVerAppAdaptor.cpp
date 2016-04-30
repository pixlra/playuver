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
 * \file     PlaYUVerAppAdaptor.h
 * \brief    Define function to manage the interface in DBus
 */

#include <QDBusAbstractAdaptor>
#include "PlaYUVerAppAdaptor.h"
#include "PlaYUVerApp.h"

namespace plaYUVer
{

PlaYUVerAppAdaptor::PlaYUVerAppAdaptor( PlaYUVerApp *app ) :
        QDBusAbstractAdaptor( app ),
        m_app( app )
{
  // application dbus interface
  QDBusConnection::sessionBus().registerObject( QStringLiteral( PLAYUVER_DBUS_PATH ), this, QDBusConnection::ExportAllSlots );
}

PlaYUVerAppAdaptor::~PlaYUVerAppAdaptor()
{
  QDBusConnection::sessionBus().unregisterObject( QStringLiteral( PLAYUVER_DBUS_PATH ) );
}

void PlaYUVerAppAdaptor::activate()
{
  if( !m_app )
  {
    return;
  }
  m_app->raise();

}

QString PlaYUVerAppAdaptor::activeSession()
{
  //return m_app->sessionManager()->activeSession()->name();
  return QString( "PlaYUVer" );
}

bool PlaYUVerAppAdaptor::loadFile( const QString& filename )
{
  m_app->loadFile( filename );
  return true;
}

}  // NAMESPACE
