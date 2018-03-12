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
 * \file     DBusAppAdaptor.cpp
 * \brief    Define function to manage the interface in DBus
 */

#include "DBusAppAdaptor.h"

#include "MainWindow.h"

//#include <QDBusConnectioninterface>
//#include <QDBusinterface>
#include <QDBusMessage>
#include <QDBusReply>

DBusAppAdaptor::DBusAppAdaptor( MainWindow* app )
    : QDBusAbstractAdaptor( app ), m_app( app )
{
  // application dbus interface
  bool isRegistered = QDBusConnection::sessionBus().registerObject( QStringLiteral( CALYP_DBUS_PATH ), this,
                                                                    QDBusConnection::ExportAllSlots );

  Q_ASSERT( isRegistered == true );
}

DBusAppAdaptor::~DBusAppAdaptor()
{
  QDBusConnection::sessionBus().unregisterObject( QStringLiteral( CALYP_DBUS_PATH ) );
}

void DBusAppAdaptor::activate()
{
  if( !m_app )
  {
    return;
  }
  m_app->raise();
}

QString DBusAppAdaptor::activeSession()
{
  // return m_app->sessionManager()->activeSession()->name();
  return QString( "Calyp" );
}

bool DBusAppAdaptor::loadFile( const QString& filename )
{
  m_app->loadFile( filename );
  return true;
}
