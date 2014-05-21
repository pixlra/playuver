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
 * \file     DialogSubWindowSelector.h
 * \brief    Dialog box to select sub windows
 */

#ifndef __DIALOGSUBWINDOWSELECTOR_H__
#define __DIALOGSUBWINDOWSELECTOR_H__

#include "config.h"
#include "PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif
#include <QtCore>

class QPixmap;
class QColor;

namespace plaYUVer
{

/**
 * Class to define the dialog box to select sub windows
 * This class enable the selection of several group of windows
 */
class DialogSubWindowSelector: public QDialog
{
Q_OBJECT

public:
  DialogSubWindowSelector( QWidget *parent = 0, QMdiArea *mdiArea = 0, Int numberWindowsSelected = -1 );

  Void setSubWindowList( QStringList cWindowListNames );
  QStringList getSelectedWindows()
  {
    return m_pcSelectedWindowListNames;
  }
private:
  Int m_iNumberOfSelectedWindows;
  QMdiArea* m_pcMainWindowMdiArea;
  QStringList m_pcWindowListNames;
  QStringList m_pcSelectedWindowListNames;

  QComboBox* m_comboBoxWindowList;
  QPushButton* m_pushButtonAdd;
  QPushButton* m_pushButtonRemove;
  QListWidget* m_listSelectedWindows;

  Void updateSubWindowList();

private Q_SLOTS:
  void addSubWindow();
  void removeSubWindow();

};

}  // NAMESPACE

#endif // __DIALOGSUBWINDOWSELECTOR_H__
