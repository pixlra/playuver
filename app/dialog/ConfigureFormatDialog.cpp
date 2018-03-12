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
 * \file     ConfigureFormatDialog.cpp
 * \brief    Dialog box to set the sequence resolution
 */

#include "ConfigureFormatDialog.h"

#include "lib/CalypFrame.h"

#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QSpinBox>
#include <QString>
#include <QWidget>

#define MAX_SUPPORTED_RESOLUTION 99999

QDataStream& operator<<( QDataStream& out, const CalypStandardResolutionVector& array )
{
  CalypStandardResolution d;
  out << array.size();
  for( int i = 0; i < array.size(); i++ )
  {
    d = array.at( i );
    out << QString::fromStdString( d.shortName ) << d.uiWidth << d.uiHeight;
  }
  return out;
}

QDataStream& operator>>( QDataStream& in, CalypStandardResolutionVector& array )
{
  CalypStandardResolution d;
  QString auxName;
  int array_size;
  in >> array_size;
  for( int i = 0; i < array_size; i++ )
  {
    in >> auxName;
    d.shortName = auxName.toStdString();
    in >> d.uiWidth;
    in >> d.uiHeight;
    array.append( d );
  }
  return in;
}

class AddCustomFormat : public QDialog
{
public:
  AddCustomFormat( QWidget* parent = NULL )
      : QDialog( parent, Qt::Dialog | Qt::WindowTitleHint )
  {
    setWindowModality( Qt::ApplicationModal );
    setWindowTitle( "Add custom resolution" );

    QLabel* nameLabel = new QLabel( "Name" );
    m_lineEdtName = new QLineEdit();

    QLabel* widthLabel = new QLabel( "Width" );
    m_spinWidth = new QSpinBox;
    m_spinWidth->setMinimumSize( 70, 5 );
    m_spinWidth->setMaximumSize( 70, 30 );
    m_spinWidth->setMinimum( 0 );
    m_spinWidth->setMaximum( MAX_SUPPORTED_RESOLUTION );

    QLabel* heightLabel = new QLabel( "Heigth" );
    m_spinHeight = new QSpinBox;
    m_spinHeight->setMinimumSize( 70, 5 );
    m_spinHeight->setMaximumSize( 70, 30 );
    m_spinHeight->setMinimum( 0 );
    m_spinHeight->setMaximum( MAX_SUPPORTED_RESOLUTION );

    QDialogButtonBox* dialogButtonOkCancel = new QDialogButtonBox();
    dialogButtonOkCancel->setObjectName( QString::fromUtf8( "dialogButtonBox" ) );
    dialogButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
    dialogButtonOkCancel->setCenterButtons( false );

    QGridLayout* mainLayout = new QGridLayout;
    mainLayout->addWidget( nameLabel, 0, 0, 1, 1, Qt::AlignCenter );
    mainLayout->addWidget( widthLabel, 0, 1, 1, 1, Qt::AlignCenter );
    mainLayout->addWidget( heightLabel, 0, 2, 1, 1, Qt::AlignCenter );

    mainLayout->addWidget( m_lineEdtName, 1, 0, 1, 1, Qt::AlignCenter );
    mainLayout->addWidget( m_spinWidth, 1, 1, 1, 1, Qt::AlignCenter );
    mainLayout->addWidget( m_spinHeight, 1, 2, 1, 1, Qt::AlignCenter );

    mainLayout->addWidget( dialogButtonOkCancel, 2, 0, 1, 3, Qt::AlignRight );

    setLayout( mainLayout );

    connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );
  }

  CalypStandardResolution runDialog()
  {
    CalypStandardResolution stdResolution;
    stdResolution.shortName = "";
    if( exec() == QDialog::Accepted )
    {
      stdResolution.shortName = m_lineEdtName->text().toStdString();
      stdResolution.uiWidth = m_spinWidth->value();
      stdResolution.uiHeight = m_spinHeight->value();
    }
    return stdResolution;
  }

private:
  QLineEdit* m_lineEdtName;
  QSpinBox* m_spinWidth;
  QSpinBox* m_spinHeight;
};

ConfigureFormatDialog::ConfigureFormatDialog( QWidget* parent )
    : QDialog( parent )
{
  QString Name;
  unsigned int uiWidth, uiHeight;

  readSettings();
  for( int i = 0; i < aRCustomFileFormats.size(); i++ )
  {
    Name = QString::fromStdString( aRCustomFileFormats[i].shortName );
    uiWidth = aRCustomFileFormats[i].uiWidth;
    uiHeight = aRCustomFileFormats[i].uiHeight;
    standardResolutionNames.append( QString( "%1 (%2x%3)" ).arg( Name ).arg( uiWidth ).arg( uiHeight ) );
    standardResolutionSizes.append( QSize( uiWidth, uiHeight ) );
  }

  std::vector<CalypStandardResolution> listCalypStdResolution = CalypStream::stdResolutionSizes();
  for( unsigned int i = 0; i < listCalypStdResolution.size(); i++ )
  {
    Name = QString::fromStdString( listCalypStdResolution[i].shortName );
    uiWidth = listCalypStdResolution[i].uiWidth;
    uiHeight = listCalypStdResolution[i].uiHeight;
    standardResolutionNames.append( QString( "%1 (%2x%3)" ).arg( Name ).arg( uiWidth ).arg( uiHeight ) );
    standardResolutionSizes.append( QSize( uiWidth, uiHeight ) );
  }

  standardResolutionNames.append( QStringLiteral( "Custom..." ) );
  standardResolutionSizes.append( QSize( 0, 0 ) );

  setObjectName( "ConfigureFormat" );

  setWindowTitle( "Configure Resolution" );
  setWindowIcon( QIcon( ":/images/configureformat.png" ) );

  QVBoxLayout* MainLayout = new QVBoxLayout( this );

  QFont titleFont, menusFont, normalFont;

  titleFont.setPointSize( 12 );
  titleFont.setBold( true );
  titleFont.setWeight( 75 );

  menusFont.setBold( true );
  titleFont.setWeight( 75 );

  normalFont.setBold( false );
  // menusFont.setPointSize( 12 );

  //  // headLayout
  //  QHBoxLayout* headLayout = new QHBoxLayout();
  //  QLabel* dialogTitleLabel = new QLabel();
  //  dialogTitleLabel->setFont( titleFont );
  //  dialogTitleLabel->setText( "Configure Resolution" );
  //  headLayout->addWidget( dialogTitleLabel );
  //  headLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding,
  //  QSizePolicy::Minimum ) );
  //
  //  MainLayout->addLayout( headLayout );
  //  MainLayout->addItem( new QSpacerItem( 10, 20, QSizePolicy::Minimum ) );

  /*
   *  Filename layout
   */
  QHBoxLayout* filenameLayout = new QHBoxLayout();
  QLabel* filenameLabel = new QLabel();
  filenameLabel->setText( "Name" );
  filenameLabel->setFont( menusFont );
  m_labelFilename = new QLabel();
  m_labelFilename->setFont( titleFont );
  filenameLayout->addWidget( filenameLabel );
  filenameLayout->addItem( new QSpacerItem( 20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  filenameLayout->addWidget( m_labelFilename );
  MainLayout->addLayout( filenameLayout );

  QSizePolicy sizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
  sizePolicy.setHorizontalStretch( 0 );
  sizePolicy.setVerticalStretch( 0 );

  /*
   *  Resolution
   */
  QGroupBox* resolutionGroup = new QGroupBox();
  resolutionGroup->setTitle( QStringLiteral( "Resolution" ) );
  resolutionGroup->setFont( menusFont );

  QGridLayout* resolutionGrid = new QGridLayout();
  resolutionGroup->setLayout( resolutionGrid );

  m_comboBoxStandardResolution = new QComboBox();

  m_comboBoxStandardResolution->setFont( normalFont );
  m_comboBoxStandardResolution->setSizePolicy( sizePolicy );
  m_comboBoxStandardResolution->setAcceptDrops( true );
  m_comboBoxStandardResolution->clear();
  m_comboBoxStandardResolution->insertItems( 0, standardResolutionNames );
  m_comboBoxStandardResolution->setCurrentIndex( -1 );

  QLabel* widthLabel = new QLabel( "Width" );
  widthLabel->setFont( normalFont );
  m_spinBoxWidth = new QSpinBox();
  m_spinBoxWidth->setRange( 0, MAX_SUPPORTED_RESOLUTION );
  m_spinBoxWidth->setValue( 0 );
  m_spinBoxWidth->setFont( normalFont );

  QLabel* heightLabel = new QLabel( "Height" );
  heightLabel->setFont( normalFont );
  m_spinBoxheight = new QSpinBox();
  m_spinBoxheight->setRange( 0, MAX_SUPPORTED_RESOLUTION );
  m_spinBoxheight->setValue( 0 );
  m_spinBoxheight->setFont( normalFont );

  QLabel* pixelsLabel = new QLabel();
  pixelsLabel->setText( "Pixels" );
  pixelsLabel->setFont( normalFont );
  resolutionGrid->addWidget( m_comboBoxStandardResolution, 1, 0, Qt::AlignCenter );
  resolutionGrid->addWidget( widthLabel, 0, 1, Qt::AlignCenter );
  resolutionGrid->addWidget( m_spinBoxWidth, 1, 1, Qt::AlignCenter );
  resolutionGrid->addWidget( heightLabel, 0, 2, Qt::AlignCenter );
  resolutionGrid->addWidget( m_spinBoxheight, 1, 2, Qt::AlignCenter );
  resolutionGrid->addWidget( pixelsLabel, 1, 3, Qt::AlignCenter );

  /*
   *  Pixel format
   */
  QGroupBox* pixelGroup = new QGroupBox();
  pixelGroup->setTitle( QStringLiteral( "Pixel Format" ) );
  pixelGroup->setFont( menusFont );
  QGridLayout* pixelGridLayout = new QGridLayout();
  pixelGroup->setLayout( pixelGridLayout );

  QLabel* colorSpaceLabel = new QLabel();
  colorSpaceLabel->setText( "Color Space" );
  colorSpaceLabel->setFont( normalFont );
  m_comboBoxColorSpace = new QComboBox();
  m_comboBoxColorSpace->setSizePolicy( sizePolicy );
  m_comboBoxColorSpace->setFont( normalFont );
  m_comboBoxColorSpace->clear();

  QLabel* pixelFormatLabel = new QLabel();
  pixelFormatLabel->setText( "Sampling" );
  pixelFormatLabel->setFont( normalFont );
  m_comboBoxPixelFormat = new QComboBox();
  m_comboBoxPixelFormat->setSizePolicy( sizePolicy );
  m_comboBoxPixelFormat->setFont( normalFont );
  m_comboBoxPixelFormat->clear();

  for( unsigned int i = 0; i < CalypFrame::supportedColorSpacesListNames().size(); i++ )
  {
    m_comboBoxColorSpace->insertItem( i, CalypFrame::supportedColorSpacesListNames()[i].c_str() );
  }
  m_comboBoxColorSpace->setCurrentIndex( 1 );

  pixelGridLayout->addWidget( colorSpaceLabel, 0, 0 );
  pixelGridLayout->addItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 1 );
  pixelGridLayout->addWidget( m_comboBoxColorSpace, 0, 2 );

  pixelGridLayout->addWidget( pixelFormatLabel, 1, 0 );
  pixelGridLayout->addItem( new QSpacerItem( 10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ), 1, 1 );
  pixelGridLayout->addWidget( m_comboBoxPixelFormat, 1, 2 );

  QLabel* bitsPerPelLabel = new QLabel();
  bitsPerPelLabel->setFont( normalFont );
  bitsPerPelLabel->setText( "Pixel bit depth" );
  m_spinBoxBits = new QSpinBox();
  m_spinBoxBits->setFont( normalFont );
  m_spinBoxBits->setMinimumWidth( 30 );
  m_spinBoxBits->setRange( 0, 16 );
  m_spinBoxBits->setValue( 8 );
  pixelGridLayout->addWidget( bitsPerPelLabel, 2, 0 );
  pixelGridLayout->addItem( new QSpacerItem( 20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ), 2, 1 );
  pixelGridLayout->addWidget( m_spinBoxBits, 2, 2 );

  /*
   *  Endianess
   */
  QHBoxLayout* endiannessLayout = new QHBoxLayout();
  QLabel* endiannessLabel = new QLabel();
  endiannessLabel->setFont( menusFont );
  endiannessLabel->setText( "Endianness" );
  m_comboBoxEndianness = new QComboBox();
  m_comboBoxEndianness->setFont( normalFont );
  m_comboBoxEndianness->setSizePolicy( sizePolicy );
  m_comboBoxEndianness->clear();
  m_comboBoxEndianness->addItem( QStringLiteral( "Big Endian" ) );
  m_comboBoxEndianness->addItem( QStringLiteral( "Little Endian" ) );
  endiannessLayout->addWidget( endiannessLabel );
  endiannessLayout->addItem( new QSpacerItem( 20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  endiannessLayout->addWidget( m_comboBoxEndianness );
  m_widgetEndianness = new QWidget();
  m_widgetEndianness->setContentsMargins( 0, 0, 0, 0 );
  m_widgetEndianness->setLayout( endiannessLayout );
  m_widgetEndianness->setVisible( true );

  /*
   *  Frame rate format
   */
  QHBoxLayout* framerateFormatLayout = new QHBoxLayout();
  QLabel* framerateFormatLabel = new QLabel();
  framerateFormatLabel->setFont( menusFont );
  framerateFormatLabel->setText( "Frame Rate" );
  m_spinBoxFrameRate = new QSpinBox();
  m_spinBoxFrameRate->setSuffix( QStringLiteral( " Hz" ) );
  m_spinBoxFrameRate->setMinimumWidth( 30 );
  m_spinBoxFrameRate->setRange( 0, 1000 );
  m_spinBoxFrameRate->setValue( 30 );
  framerateFormatLayout->addWidget( framerateFormatLabel );
  framerateFormatLayout->addItem( new QSpacerItem( 20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  framerateFormatLayout->addWidget( m_spinBoxFrameRate );
  QWidget* framerateWidget = new QWidget();
  framerateWidget->setContentsMargins( 0, 0, 0, 0 );
  framerateWidget->setLayout( framerateFormatLayout );

  /*
   *  Confirmation buttons
   */
  QDialogButtonBox* dialogButtonOkCancel = new QDialogButtonBox();
  dialogButtonOkCancel->setObjectName( QString::fromUtf8( "dialogButtonBox" ) );
  dialogButtonOkCancel->setStandardButtons( QDialogButtonBox::Cancel | QDialogButtonBox::Ok );
  dialogButtonOkCancel->setCenterButtons( false );

  MainLayout->addItem( new QSpacerItem( 10, 5, QSizePolicy::Minimum ) );
  MainLayout->addWidget( resolutionGroup );
  MainLayout->addItem( new QSpacerItem( 10, 5, QSizePolicy::Minimum ) );
  MainLayout->addWidget( pixelGroup );
  MainLayout->addItem( new QSpacerItem( 10, 5, QSizePolicy::Minimum ) );
  MainLayout->addWidget( m_widgetEndianness );
  MainLayout->addWidget( framerateWidget );
  MainLayout->addItem( new QSpacerItem( 10, 5, QSizePolicy::Minimum, QSizePolicy::Expanding ) );
  MainLayout->addWidget( dialogButtonOkCancel );

  connect( m_comboBoxStandardResolution, SIGNAL( currentIndexChanged( int ) ), this,
           SLOT( slotStandardResolutionSelected( int ) ) );
  connect( m_spinBoxWidth, SIGNAL( valueChanged( int ) ), this, SLOT( slotResolutionChange() ) );
  connect( m_spinBoxheight, SIGNAL( valueChanged( int ) ), this, SLOT( slotResolutionChange() ) );
  connect( m_comboBoxColorSpace, SIGNAL( currentIndexChanged( int ) ), this, SLOT( slotColorSpaceChange( int ) ) );
  connect( m_spinBoxBits, SIGNAL( valueChanged( int ) ), this, SLOT( slotBitsChange( int ) ) );

  connect( dialogButtonOkCancel, SIGNAL( accepted() ), this, SLOT( accept() ) );
  connect( dialogButtonOkCancel, SIGNAL( rejected() ), this, SLOT( reject() ) );

  m_comboBoxColorSpace->setCurrentIndex( 0 );

  setFixedSize( MainLayout->sizeHint() );
}

int ConfigureFormatDialog::runConfigureFormatDialog( const QString& Filename, unsigned int& rWidth, unsigned int& rHeight,
                                                     int& rInputFormat, unsigned int& rBits, int& rEndianess, unsigned int& rFrameRate )
{
  // Set default values
  // setWindowTitle( "Configure resolution for " + Filename );
  m_labelFilename->setText( QFileInfo( Filename ).fileName() );
  m_spinBoxWidth->setValue( rWidth );
  m_spinBoxheight->setValue( rHeight );

  int colorSpace = 0;
  int sampling = 0;
  if( rInputFormat >= 0 )
  {
    ClpString pelFmtName = CalypFrame::supportedPixelFormatListNames()[rInputFormat];
    for( unsigned int j = 0; j < CalypFrame::supportedColorSpacesListNames().size(); j++ )
    {
      for( unsigned int i = 0; i < CalypFrame::supportedPixelFormatListNames( j ).size(); i++ )
      {
        if( pelFmtName == CalypFrame::supportedPixelFormatListNames( j )[i] )
        {
          colorSpace = j;
          sampling = i;
          break;
        }
      }
    }
  }
  m_comboBoxColorSpace->setCurrentIndex( colorSpace );
  m_comboBoxPixelFormat->setCurrentIndex( sampling );
  m_spinBoxBits->setValue( rBits );
  m_comboBoxEndianness->setCurrentIndex( rEndianess );
  if( rBits > 8 )
  {
    m_widgetEndianness->setVisible( true );
  }
  else
  {
    m_widgetEndianness->setVisible( false );
  }
  m_spinBoxFrameRate->setValue( rFrameRate );

  for( int i = 0; i < standardResolutionSizes.size() - 1; i++ )
  {
    if( standardResolutionSizes.at( i ) == QSize( rWidth, rHeight ) )
    {
      m_comboBoxStandardResolution->setCurrentIndex( i );
      break;
    }
  }

  if( exec() == QDialog::Rejected )
  {
    writeSettings();
    return QDialog::Rejected;
  }

  rWidth = m_spinBoxWidth->value();
  rHeight = m_spinBoxheight->value();
  colorSpace = m_comboBoxColorSpace->currentIndex();
  sampling = m_comboBoxPixelFormat->currentIndex();
  ClpString pelFmtName = CalypFrame::supportedPixelFormatListNames( colorSpace )[sampling];
  for( unsigned int i = 0; i < CalypFrame::supportedPixelFormatListNames().size(); i++ )
  {
    if( pelFmtName == CalypFrame::supportedPixelFormatListNames()[i] )
    {
      rInputFormat = i;
      break;
    }
  }
  rBits = m_spinBoxBits->value();
  rEndianess = m_comboBoxEndianness->currentIndex();
  rFrameRate = m_spinBoxFrameRate->value();
  writeSettings();
  return QDialog::Accepted;
}

void ConfigureFormatDialog::slotStandardResolutionSelected( int idx )
{
  if( idx == standardResolutionSizes.size() - 1 )
  {
    AddCustomFormat* pcCustomFmtDialog = new AddCustomFormat;
    CalypStandardResolution customResolution = pcCustomFmtDialog->runDialog();
    aRCustomFileFormats.append( customResolution );
    if( customResolution.shortName != "" )
    {
      QString name = QString::fromStdString( customResolution.shortName );
      name.append( QString( " (%2x%3)" ).arg( customResolution.uiWidth ).arg( customResolution.uiHeight ) );
      standardResolutionNames.insert( 0, name );
      standardResolutionSizes.insert( 0, QSize( customResolution.uiWidth, customResolution.uiHeight ) );

      m_comboBoxStandardResolution->blockSignals( true );
      m_comboBoxStandardResolution->insertItem( 0, name );
      m_comboBoxStandardResolution->blockSignals( false );
      m_comboBoxStandardResolution->setCurrentIndex( 0 );
    }
  }
  if( idx >= 0 )
  {
    QSize currSize = standardResolutionSizes.at( idx );
    m_spinBoxWidth->blockSignals( true );
    m_spinBoxWidth->setValue( currSize.width() );
    m_spinBoxWidth->blockSignals( false );
    m_spinBoxheight->blockSignals( true );
    m_spinBoxheight->setValue( currSize.height() );
    m_spinBoxheight->blockSignals( false );
  }
}

void ConfigureFormatDialog::slotResolutionChange()
{
  m_comboBoxStandardResolution->blockSignals( true );
  int newIdx = -1;
  for( int i = 0; i < standardResolutionSizes.size(); i++ )
  {
    if( standardResolutionSizes.at( i ) == QSize( m_spinBoxWidth->value(), m_spinBoxheight->value() ) )
    {
      newIdx = i;
      break;
    }
  }
  m_comboBoxStandardResolution->setCurrentIndex( newIdx );
  m_comboBoxStandardResolution->blockSignals( false );
}

void ConfigureFormatDialog::slotColorSpaceChange( int idx )
{
  if( idx == -1 )
    return;

  m_comboBoxPixelFormat->clear();
  for( unsigned int i = 0; i < CalypFrame::supportedPixelFormatListNames( idx ).size(); i++ )
  {
    m_comboBoxPixelFormat->insertItem( i, CalypFrame::supportedPixelFormatListNames( idx )[i].c_str() );
  }
  m_comboBoxPixelFormat->setCurrentIndex( 0 );
}

void ConfigureFormatDialog::slotBitsChange( int idx )
{
  if( idx > 8 )
  {
    m_widgetEndianness->setVisible( true );
  }
  else
  {
    m_widgetEndianness->setVisible( false );
  }
}

void ConfigureFormatDialog::readSettings()
{
  QSettings appSettings;
  QVariant value = appSettings.value( "ConfigureFormatDialog/CustomFormats" );
  aRCustomFileFormats = value.value<CalypStandardResolutionVector>();
}

void ConfigureFormatDialog::writeSettings()
{
  QSettings appSettings;
  QVariant var;
  var.setValue<CalypStandardResolutionVector>( aRCustomFileFormats );
  appSettings.setValue( "ConfigureFormatDialog/CustomFormats", var );
}
