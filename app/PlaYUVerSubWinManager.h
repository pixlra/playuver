/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014-2015  by Luis Lucas      (luisfrlucas@gmail.com)
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
 * \file     PlaYUVerSubWinManager.h
 * \brief
 */

#ifndef __PLAYUVERSUBWINMANAGER_H__
#define __PLAYUVERSUBWINMANAGER_H__

#include "config.h"
#include "lib/PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif

namespace plaYUVer
{

class SubWindowHandle;
class PlaYUVerMdiArea;
class PlaYUVerMdiSubWindow;

class PlaYUVerSubWinManager : public QWidget
{
	Q_OBJECT

private:
  Bool m_bMdiModeEnabled;
	QList<SubWindowHandle*> m_apcSubWindowList;

	QHBoxLayout* m_pcWindowManagerLayout;
	PlaYUVerMdiArea* m_pcMdiArea;
	QList<PlaYUVerMdiSubWindow*> m_apcMdiSubWindowList;

	SubWindowHandle* m_pcActiveWindow;

public:
	PlaYUVerSubWinManager(QWidget *parent = 0);

	Void addSubWindow(SubWindowHandle *widget, Qt::WindowFlags flags = 0);
	Void removeSubWindow( Int windowIdx );
	Void removeSubWindow( SubWindowHandle *window);

  Void removeAllSubWindow();

	SubWindowHandle *activeSubWindow() const;

	QList<SubWindowHandle*> findSubWindow( const QString &aName = QString() ) const;
	QList<SubWindowHandle*> findSubWindow( const UInt uiCategory ) const;
	SubWindowHandle* findSubWindow( const SubWindowHandle* subWindow );

public Q_SLOTS:
  void updateActiveSubWindow();
  void setActiveSubWindow(QWidget *window);
  void removeMdiSubWindow( PlaYUVerMdiSubWindow* window );
  void tileSubWindows();
  void cascadeSubWindows();

Q_SIGNALS:
  void windowActivated();

};

}  // NAMESPACE

#endif // __PLAYUVERSUBWINMANAGER_H__
