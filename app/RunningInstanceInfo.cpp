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
 * \file     RunningInstanceInfo.cpp
 * \brief    Control running instances
 */

#include "RunningInstanceInfo.h"
#include <QDBusReply>
#include <QStringList>
#include <QCoreApplication>
#include <QDBusConnectionInterface>
#include <QDebug>

namespace plaYUVer
{

int RunningInstanceInfo::dummy_session = 0;

bool fillinRunningAppInstances(RunningInstanceInfoMap *map)
{
    QDBusConnectionInterface *i = QDBusConnection::sessionBus().interface();

    // look up all running kate instances and there sessions
    QDBusReply<QStringList> servicesReply = i->registeredServiceNames();
    QStringList services;
    if (servicesReply.isValid()) {
        services = servicesReply.value();
    }

    QString serviceName;

    QString my_pid = QString::number(QCoreApplication::applicationPid());

    foreach(const QString & s, services) {
        if (s.startsWith(QStringLiteral("org.pixlra.PlaYUVer"))) {
            if (s.contains(my_pid)) {
                continue;
            }
            qDebug() << s ;
            RunningInstanceInfo *rii = new RunningInstanceInfo(s);
            if (rii->valid) {
                if (map->contains(rii->sessionName)) {
                    return false;    //ERROR no two instances may have the same session name
                }
                map->insert(rii->sessionName, rii);
                //std::cerr<<qPrintable(s)<<"running instance:"<< rii->sessionName.toUtf8().data()<<std::endl;
            } else {
                delete rii;
            }
        }
    }
    return true;
}

void cleanupRunningAppInstanceMap(RunningInstanceInfoMap *map)
{
    for (RunningInstanceInfoMap::const_iterator it = map->constBegin(); it != map->constEnd(); ++it) {
        delete it.value();
    }
    map->clear();
}

}  // NAMESPACE


