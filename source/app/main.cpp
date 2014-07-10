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
 * \file     main.cpp
 * \brief    main file
 */

#include <QApplication>
#include "plaYUVerApp.h"
#ifdef USE_FERVOR
#include "fvupdater.h"
#endif
using namespace plaYUVer;

#if _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main( int argc, char *argv[] )
{
  QApplication a( argc, argv );
  QApplication::setApplicationName("playuver");
  QApplication::setApplicationVersion("PLAYUVER_VERSION");
  QApplication::setOrganizationName("pixlra");
  QApplication::setOrganizationDomain("playuver.pixlra");
  plaYUVerApp w;
  w.show();
  w.parseArgs( argc, argv );
#ifdef USE_FERVOR
  FvUpdater::sharedUpdater()->SetFeedURL("http://192.168.96.201/share/jcarreira.it.pub/plaYUVer/playuver-update.xml");
#endif
  return a.exec();
}
