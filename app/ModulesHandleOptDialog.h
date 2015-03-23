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
 * \file     ModulesHandleOptDialog.h
 * \brief    Dialog box to config modules opts
 */

#ifndef __MODULEHANDLEOPTDIALOG_H__
#define __MODULEHANDLEOPTDIALOG_H__

#include "config.h"
#include "PlaYUVerAppDefs.h"
#include "PlaYUVerAppModuleIf.h"
#include <QDialog>
#include <QVector>

namespace plaYUVer
{

class OpionConfiguration;

/**
 *
 */
class ModulesHandleOptDialog: public QDialog
{
Q_OBJECT

public:
  ModulesHandleOptDialog( QWidget *parent, PlaYUVerAppModuleIf *pcCurrModuleIf );
  Int runConfiguration();
private:
  PlaYUVerAppModuleIf *m_pcCurrModuleIf;
  QVector<OpionConfiguration*> m_apcOptionList;
};

}  // NAMESPACE

#endif // __MODULEHANDLEOPTDIALOG_H__
