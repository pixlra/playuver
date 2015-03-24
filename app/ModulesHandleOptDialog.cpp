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
 * \file     ModulesHandleOptDialog.cpp
 * \brief    Dialog box to config modules opts
 */

#include "ModulesHandleOptDialog.h"
#include "lib/ProgramOptions.h"
#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QVBoxLayout>
#include <QCheckBox>

namespace plaYUVer
{

class OpionConfiguration: public QWidget
{
public:
  OpionConfiguration( QWidget *parent, OptionBase* option ) :
          QWidget( parent )
  {
    m_cName = QString::fromStdString( option->opt_string );
    m_pcDescription = new QLabel( this );
    m_pcDescription->setText( QString::fromStdString( option->opt_desc ) );
    m_pcValue = new QLineEdit( this );
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->addWidget( m_pcDescription );
    layout->addWidget( m_pcValue );
  }
  QString getValue()
  {
    return m_pcValue->text();
  }
  QString& getName()
  {
    return m_cName;
  }
private:
  QLineEdit* m_pcValue;
  QLabel* m_pcDescription;
  QString m_cName;
};

ModulesHandleOptDialog::ModulesHandleOptDialog( QWidget *parent, PlaYUVerAppModuleIf *pcCurrModuleIf ) :
        QDialog( parent ),
        m_pcCurrModuleIf( pcCurrModuleIf )
{
  resize( 300, 10 );
  setWindowTitle( "Select module parameters" );
  setWindowIcon( QIcon( ":/images/configureformat.png" ) );

  const Options::OptionsList& moduleOptions = m_pcCurrModuleIf->m_pcModule->m_cModuleOptions.getOptionList();

  QVBoxLayout* mainLayout = new QVBoxLayout( this );

  OpionConfiguration* pcOption;
  for( Options::OptionsList::const_iterator it = moduleOptions.begin(); it != moduleOptions.end(); ++it )
  {
    pcOption = new OpionConfiguration( this, ( *it )->opt );
    m_apcOptionList.append( pcOption );
    mainLayout->addWidget( pcOption );
  }

  QDialogButtonBox* dialogButtonOkCancel = new QDialogButtonBox();
  dialogButtonOkCancel->setObjectName( QString::fromUtf8( "dialogButtonBox" ) );
  dialogButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
  dialogButtonOkCancel->setCenterButtons( false );

  mainLayout->addWidget( dialogButtonOkCancel );

  connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );
}

Int ModulesHandleOptDialog::runConfiguration()
{
  if( exec() == QDialog::Rejected )
  {
    return QDialog::Rejected;
  }
  QString optionString( "" );
  QString valueString( "" );
  Int argsCount = 0;
  for( Int i = 0; i < m_apcOptionList.size(); i++ )
  {
    valueString = m_apcOptionList.at( i )->getValue();
    if( !valueString.isEmpty() )
    {
      optionString.append( QString( "--%1=%2" ).arg( m_apcOptionList.at( i )->getName() ).arg( valueString ) );
      argsCount++;
    }
  }
  if( argsCount > 0 )
  {
    std::string optionStdString = optionString.toStdString();
    scanLine( m_pcCurrModuleIf->m_pcModule->m_cModuleOptions, optionStdString );
    qDebug( ) << optionString;
  }
  return QDialog::Accepted;
}

}  // NAMESPACE
