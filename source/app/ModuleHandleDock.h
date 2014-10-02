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
 * \file     ModuleHandleDock.h
 * \brief    Module handle dock
 */

#ifndef __MODULEHANDLEDOCK_H__
#define __MODULEHANDLEDOCK_H__

#include "config.h"
#include "PlaYUVerDefs.h"
#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#include <QWidget>
#endif


namespace plaYUVer
{

class PlaYUVerModuleIf;

class ModuleHandleDock: public QWidget
{
Q_OBJECT
public:
  ModuleHandleDock( QWidget* parent, PlaYUVerModuleIf* moduleIf );
  ~ModuleHandleDock();

  Void setModulueReturnValue( Double value );

  QSize sizeHint() const;

private:

  PlaYUVerModuleIf* m_pcCurrModuleIf;

  Bool m_bIsVisible;
  QLabel* labelModulueValueLabel;
  QLabel* labelModulueReturnValue;

public Q_SLOTS:
  void visibilityChangedSlot( bool );

};

}   // NAMESPACE

#endif // __MODULEHANDLEDOCK_H__
