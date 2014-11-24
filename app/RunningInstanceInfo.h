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
 * \file     RunningInstanceInfo.h
 * \brief    Control running instances
 */

#ifndef _PLAYUVER_RUNNING_INSTANCE_INFO_
#define _PLAYUVER_RUNNING_INSTANCE_INFO_

#include <iostream>
#include <QMap>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QVariant>
#include "lib/PlaYUVerDefs.h"
#include "PlaYUVerAppAdaptor.h"

namespace plaYUVer
{

class RunningInstanceInfo: public QObject
{
Q_OBJECT

public:
  RunningInstanceInfo( const QString &serviceName_ ) :
          QObject(),
          valid( false ),
          serviceName( serviceName_ ),
          dbus_if( new QDBusInterface( serviceName_, QStringLiteral( PLAYUVER_DBUS_PATH ), QString(),  //I don't know why it does not work if I specify org.pixlra.PlaYUVer.application here
              QDBusConnection::sessionBus(), this ) )
  {
    if( !dbus_if->isValid() )
    {
      std::cerr << qPrintable( QDBusConnection::sessionBus().lastError().message() )
                << std::endl;
    }
//    QVariant a_s = dbus_if->property( "activeSession" );
//          std::cerr<<a_s.isValid()<<std::endl;
//     std::cerr<<"property:"<<qPrintable(a_s.toString())<<std::endl;
//     std::cerr<<qPrintable(QDBusConnection::sessionBus().lastError().message())<<std::endl;
//    if( !a_s.isValid() )
//    {
//      sessionName = QString::fromLatin1( "___NO_SESSION_OPENED__%1" ).arg( dummy_session++ );
//      valid = false;
//    }
//    else
    {
//      if( a_s.toString().isEmpty() )
//      {
        sessionName = QString::fromLatin1( "___DEFAULT_CONSTRUCTED_SESSION__%1" ).arg( dummy_session++ );
//      }
//      else
//      {
//        sessionName = a_s.toString();
//      }
      valid = true;
    }
  }
  ;
  virtual ~RunningInstanceInfo()
  {
    delete dbus_if;
  }
  Bool valid;
  const QString serviceName;
  QDBusInterface *dbus_if;
  QString sessionName;

private:
  static Int dummy_session;
};

typedef QMap<QString, RunningInstanceInfo *> RunningInstanceInfoMap;

Q_DECL_EXPORT bool fillinRunningAppInstances( RunningInstanceInfoMap *map );
Q_DECL_EXPORT void cleanupRunningAppInstanceMap( RunningInstanceInfoMap *map );

}  // NAMESPACE

#endif // _PLAYUVER_RUNNING_INSTANCE_INFO_

