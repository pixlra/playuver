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
 * \file     main.cpp
 * \brief    main file
 */

#include "CommonDefs.h"
#include "ConfigureFormatDialog.h"
#include "MainWindow.h"
#include "VideoSubWindow.h"
#include "config.h"
#ifdef USE_QTDBUS
#include "DBusAppAdaptor.h"
#endif
#include <QApplication>
#ifdef USE_FERVOR
#include "fvupdater.h"
#endif

#if _WIN32
#pragma comment( linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup" )
#endif

int main( int argc, char* argv[] )
{
  qRegisterMetaType<CalypFileInfoVector>();
  qRegisterMetaTypeStreamOperators<CalypFileInfoVector>();
  qRegisterMetaType<CalypStandardResolutionVector>();
  qRegisterMetaTypeStreamOperators<CalypStandardResolutionVector>();

  QApplication application( argc, argv );
  QApplication::setApplicationName( "Calyp" );
  QApplication::setApplicationVersion( CALYP_VERSION_STRING );
  QApplication::setOrganizationName( "PixLRA" );

#ifdef USE_QTDBUS
  /**
   * use dbus, if available
   * allows for resuse of running Calyp instances
   */
  if( QDBusConnectionInterface* const sessionBusinterface = QDBusConnection::sessionBus().interface() )
  {
    bool force_new = false;
    QStringList filenameList;
    for( int i = 1; i < argc; i++ )
    {
      filenameList.append( QFileInfo( QString( argv[i] ) ).absoluteFilePath() );
    }
    if( filenameList.isEmpty() )
    {
      force_new = true;
    }

    // check again if service is still running
    bool foundRunningService = false;
    if( !force_new )
    {
      QDBusReply<bool> there = sessionBusinterface->isServiceRegistered( CALYP_DBUS_SESSION_NAME );
      foundRunningService = there.isValid() && there.value();

      if( foundRunningService )
      {
        qInfo() << "Found running instance... Re-using";

        // open given session
        QDBusMessage m = QDBusMessage::createMethodCall(
            CALYP_DBUS_SESSION_NAME, QStringLiteral( CALYP_DBUS_PATH ),
            QStringLiteral( CALYP_DBUS_SESSION_NAME ), QStringLiteral( "activate" ) );

        QDBusConnection::sessionBus().call( m );

        // only block, if files to open there....
        bool needToBlock = false;

        QStringList tokens;

        // open given files...
        foreach( const QString& file, filenameList )
        {
          QDBusMessage m = QDBusMessage::createMethodCall(
              CALYP_DBUS_SESSION_NAME, QStringLiteral( CALYP_DBUS_PATH ),
              QStringLiteral( CALYP_DBUS_SESSION_NAME ), QStringLiteral( "loadFile" ) );

          QList<QVariant> dbusargs;
          dbusargs.append( file );
          m.setArguments( dbusargs );

          QDBusMessage res = QDBusConnection::sessionBus().call( m );
          if( res.type() == QDBusMessage::ReplyMessage )
          {
            if( res.arguments().count() == 1 )
            {
              QVariant v = res.arguments()[0];
              if( v.isValid() )
              {
                QString s = v.toString();
                if( ( !s.isEmpty() ) && ( s != QStringLiteral( "ERROR" ) ) )
                {
                  tokens << s;
                }
              }
            }
          }
        }
        // this will wait until exiting is emitted by the used instance, if
        // wanted...
        return needToBlock ? application.exec() : 0;
      }
    }
  }

  /**
   * if we arrive here, we need to start a new calyp instance!
   */
  QDBusConnection::sessionBus().registerService( CALYP_DBUS_SESSION_NAME );
#endif
  MainWindow mainwindow;
  mainwindow.show();
  if( mainwindow.parseArgs( argc, argv ) )
  {
    return 0;
  }

#ifdef USE_FERVOR
  FvUpdater::sharedUpdater()->SetFeedURL( "http://192.168.96.201/share/CalypProject/"
                                          "CalypUpdate-" UPDATE_CHANNEL ".xml" );
  FvUpdater::sharedUpdater()->SetDependencies( "ALL" );
#endif

  return application.exec();
}
