/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2017  by Luis Lucas      (luisfrlucas@gmail.com)
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

#ifndef __PlaYUVerAppAdaptor_H__
#define __PlaYUVerAppAdaptor_H__

#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QtCore>
#include <QtDBus>

#define PLAYUVER_DBUS_SESSION_NAME "org.pixlra.PlaYUVer.application"
#define PLAYUVER_DBUS_PATH "/"

class PlaYUVerApp;

class PlaYUVerAppAdaptor : public QDBusAbstractAdaptor
{
  Q_OBJECT
	Q_CLASSINFO( "D-Bus Interface", PLAYUVER_DBUS_SESSION_NAME )
  Q_PROPERTY( QString activeSession READ activeSession )
 public:
  PlaYUVerAppAdaptor( PlaYUVerApp* app );
  ~PlaYUVerAppAdaptor();
  /**
   * emit the exiting signal
   */
  // void emitExiting();
  // void emitDocumentClosed(const QString &token);
 public Q_SLOTS:
  /**
   * open a file with given url and encoding
   * will get view created
   * @param url url of the file
   * @param encoding encoding name
   * @return success
   */
  bool loadFile( const QString& filename );

  /**
   * activate this kate instance
   */
  void activate();

 Q_SIGNALS:
  /**
   * Notify the world that this kate instance is exiting.
   * All apps should stop using the dbus interface of this instance after this
   * signal got emitted.
   */
  // void exiting();
  // void documentClosed(const QString &token);
 public:
  QString activeSession();

 private:
  PlaYUVerApp* m_app;
};

#endif  // __PlaYUVerAppAdaptor_H__
