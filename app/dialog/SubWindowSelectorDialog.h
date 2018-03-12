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
 * \file     DialogSubWindowSelector.h
 * \brief    Dialog box to select sub windows
 */

#ifndef __DIALOGSUBWINDOWSELECTOR_H__
#define __DIALOGSUBWINDOWSELECTOR_H__

#include "CommonDefs.h"
#include "config.h"

#include <QtCore>
#include <QtWidgets>

#include <QVector>

class QPixmap;
class QColor;
class QCheckBox;
class QGroupBox;

class SubWindowHandle;
class SubWindowAbstract;

/**
 * Class to define the dialog box to select sub windows
 * This class enable the selection of several group of windows
 */
class SubWindowSelectorDialog : public QDialog
{
  Q_OBJECT

public:
  SubWindowSelectorDialog( QWidget* parent, SubWindowHandle* windowManager, unsigned int uiCategory,
                           int minWindowsSelected = 1, int maxWindowsSelected = -1 );

  void selectSubWindow( SubWindowAbstract* subWindow );
  QList<SubWindowAbstract*> getSelectedWindows() { return m_apcSelectedSubWindowList; }

private:
  unsigned int m_uiCategory;
  int m_iMinSelectedWindows;
  int m_iMaxSlectedWindows;
  SubWindowHandle* m_pcMainWindowManager;

  QList<SubWindowAbstract*> m_apcSubWindowList;
  QList<SubWindowAbstract*> m_apcSelectedSubWindowList;

  QSignalMapper* m_mapperWindowsList;
  QGroupBox* m_pcGroupCheckBox;
  QVector<QCheckBox*> m_apcWindowsListCheckBox;

  QComboBox* m_comboBoxWindowList;
  QDialogButtonBox* m_pushButtonOkCancel;
  QPushButton* m_pushButtonAddAll;
  QPushButton* m_pushButtonRemoveAll;

  void updateSubWindowList();
  void update();

private Q_SLOTS:
  void toggleSubWindow( int );
  void addAllSubWindow();
  void removeAllSubWindow();
};

#endif  // __DIALOGSUBWINDOWSELECTOR_H__
