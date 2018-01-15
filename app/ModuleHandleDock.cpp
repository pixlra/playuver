/*    This file is a part of PlaYUVer project
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
 * \file     ModuleHandleDock.cpp
 * \brief    Module handle dock
 */

#include "ModuleHandleDock.h"

#include "ModulesHandle.h"
#include "PlaYUVerAppModuleIf.h"

#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QtGui>

ModuleHandleDock::ModuleHandleDock( QWidget* parent, PlaYUVerAppModuleIf* moduleIf )
    : QWidget( parent ), m_pcCurrModuleIf( moduleIf )
{
  // ----------------- Dock definition -----------------

  QString labelStr;
  labelStr.append( moduleIf->m_pcModule->m_pchModuleName );
  labelStr.append( " Result:" );
  labelModulueValueLabel = new QLabel( labelStr );
  labelModulueValueLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );

  labelModulueReturnValue = new QLabel;
  labelModulueReturnValue->setAlignment( Qt::AlignRight | Qt::AlignVCenter );

  QGridLayout* modulesPropertiesLayout = new QGridLayout;
  Int layout_line = 0;
  modulesPropertiesLayout->addWidget( labelModulueValueLabel, layout_line, 0 );
  modulesPropertiesLayout->addWidget( labelModulueReturnValue, layout_line, 1 );
  layout_line++;

  modulesPropertiesLayout->setRowStretch( 8, 10 );
  setLayout( modulesPropertiesLayout );

  setEnabled( true );
}

ModuleHandleDock::~ModuleHandleDock() {}

Void ModuleHandleDock::setModulueReturnValue( Double value )
{
  labelModulueReturnValue->clear();
  QString strValue = QString( "%1" ).arg( value );
  labelModulueReturnValue->setText( strValue );
}

Void ModuleHandleDock::visibilityChangedSlot( bool visiblity )
{
  if( !visiblity && m_pcCurrModuleIf )
  {
    PlaYUVerAppModuleIf* pcCurrentModule = m_pcCurrModuleIf;
    m_pcCurrModuleIf = NULL;
    ModulesHandle::destroyModuleIf( pcCurrentModule );
  }
}

QSize ModuleHandleDock::sizeHint() const
{
  QSize currSize = size();
  QSize bestSize( 180, currSize.height() );
  if( currSize.width() < bestSize.width() )
    return bestSize;
  else
    return currSize;
}
