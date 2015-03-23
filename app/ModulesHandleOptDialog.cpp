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
  OpionConfiguration( QWidget *parent, const QString& optName ) :
          QWidget( parent ),
          m_cName( optName )
  {
    m_pcCheckBox = new QCheckBox( this );
    m_pcCheckBox->setText( optName );
    m_pcSpinBox = new QSpinBox( this );
    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->addWidget( m_pcCheckBox );
    layout->addWidget( m_pcSpinBox );
    m_pcSpinBox->setMaximum( 999 );
  }

  Bool getChecked()
  {
    return m_pcCheckBox->checkState() == Qt::Checked;
  }
  Int getValue()
  {
    return m_pcSpinBox->value();
  }
  QString& getName()
  {
    return m_cName;
  }
private:
  QCheckBox* m_pcCheckBox;
  QSpinBox* m_pcSpinBox;
  QString m_cName;
};

ModulesHandleOptDialog::ModulesHandleOptDialog( QWidget *parent, PlaYUVerAppModuleIf *pcCurrModuleIf ) :
        QDialog( parent ),
        m_pcCurrModuleIf( pcCurrModuleIf )
{
  resize( 300, 10 );
  setWindowTitle( "Configure Resolution" );
  setWindowIcon( QIcon( ":/images/configureformat.png" ) );

  const Options::OptionsList& moduleOptions = m_pcCurrModuleIf->m_pcModule->m_cModuleOptions.getOptionList();

  QVBoxLayout* mainLayout = new QVBoxLayout( this );

  OpionConfiguration* pcOption;
  for( Options::OptionsList::const_iterator it = moduleOptions.begin(); it != moduleOptions.end(); ++it )
  {
    pcOption = new OpionConfiguration( this, QString::fromStdString( ( *it )->opt->opt_string ) );
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
  Int argsCount = 0;
  for( Int i = 0; i < m_apcOptionList.size(); i++ )
  {
    if( m_apcOptionList.at( i )->getChecked() )
    {
      optionString.append( QString( "--%1=%2" ).arg( m_apcOptionList.at( i )->getName() ).arg( m_apcOptionList.at( i )->getValue() ) );
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
