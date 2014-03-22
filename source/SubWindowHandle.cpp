/*    This file is a part of plaYUVer project
 *    Copyright (C) 2014  by plaYUVer developers
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
 * \file     imageinterface.cpp
 * \brief    Sub windows handling
 */

#include "ConfigureFormatDialog.h"
#include "SubWindowHandle.h"
#include "ViewArea.h"

namespace plaYUVer
{

SubWindowHandle::SubWindowHandle( QWidget * parent ) :
    QMdiSubWindow( parent )
{
  setParent( parent );

  setAttribute( Qt::WA_DeleteOnClose );
  setBackgroundRole( QPalette::Light );

  // Create a new scroll area inside the sub-window
  m_cScrollArea = new QScrollArea( this );
  setWidget( m_cScrollArea );

  // Create a new interface to show images
  m_cViewArea = new ViewArea( m_cScrollArea );

  // Define the cViewArea as the widget inside the scroll area
  m_cScrollArea->setWidget( m_cViewArea );

  m_pcCurrentModule = NULL;

  m_cWindowName = QString( " " );
  m_bIsPlaying = true;
  m_dScaleFactor = 1;

}

SubWindowHandle::~SubWindowHandle()
{
  delete m_cViewArea;
  if( !m_pcCurrentModule )
    return;
  m_pcCurrentModule->m_pcAction->setChecked( false );
  m_pcCurrentModule->destroy();
  m_pcCurrentModule = NULL;
}

bool SubWindowHandle::loadFile( const QString &fileName )
{
  ConfigureFormatDialog formatDialog( this );

  m_pCurrStream = new InputStream;

  if( m_pCurrStream->needFormatDialog( fileName ) )
  {
    if( formatDialog.exec() == QDialog::Rejected )
    {
      return false;
    }
    m_pCurrStream->init( fileName, formatDialog.getResolution().width(), formatDialog.getResolution().height(), formatDialog.getPixelFormat(),
        formatDialog.getFrameRate() );
  }
  else
  {
    m_pCurrStream->init( fileName, 0, 0, 0, 0 );
  }
  QApplication::setOverrideCursor( Qt::WaitCursor );
  QApplication::restoreOverrideCursor();
  if( m_pCurrStream->getStatus() == 0 )
  {
    return false;
  }

  m_pCurrFrameQImage = new QImage( m_pCurrStream->getWidth(), m_pCurrStream->getHeight(), QImage::Format_RGB888 );

  m_pCurrStream->readFrame();
  if( m_pCurrStream->checkErrors( READING ) )
  {
    QMessageBox::warning( this, tr( "plaYUVer" ), tr( "Cannot read %1." ).arg( fileName ) );
    return false;
  }
  m_cViewArea->setInputStream(m_pCurrStream);

  QApplication::restoreOverrideCursor();

  refreshFrame();
  normalSize();

  m_cWindowName = QString( " " );
  m_cWindowName = m_pCurrStream->getStreamInformationString();

  m_cCurrFileName = fileName;

  setWindowTitle( m_cWindowName );

  return true;
}

/**
 * Functions to enable a module in the
 * current SubWindow
 */
Void SubWindowHandle::enableModule( PlaYUVerModuleIf* select_module )
{
  if( m_pcCurrentModule == select_module )
  {
    disableModule();
    return;
  }
  m_pcCurrentModule = select_module;
  m_pcCurrentModule->create( m_pCurrStream->getFrame() );
  refreshFrame();
}

Void SubWindowHandle::disableModule()
{
  if( !m_pcCurrentModule )
    return;

  m_pcCurrentModule->destroy();
  m_pcCurrentModule = NULL;
  refreshFrame();
}

QImage* SubWindowHandle::FrameToQImage( PlaYUVerFrame* curr_frame )
{
  Pel*** bufferRGB = curr_frame->getPelBufferRGB();

  curr_frame->YUV420toRGB();

  for( UInt y = 0; y < curr_frame->getHeight(); y++ )
  {
    for( UInt x = 0; x < curr_frame->getWidth(); x++ )
    {
      m_pCurrFrameQImage->setPixel( x, y, qRgb( bufferRGB[0][y][x], bufferRGB[1][y][x], bufferRGB[2][y][x] ) );
    }
  }
  return m_pCurrFrameQImage;
}

Void SubWindowHandle::refreshFrame()
{
  if( m_pcCurrentModule )
  {
    FrameToQImage( m_pcCurrentModule->process( m_pCurrStream->getFrame() ) );
  }
  else
  {
    FrameToQImage( m_pCurrStream->getFrame( (PlaYUVerFrame*) NULL ) );
  }
  m_cViewArea->setImage( QPixmap::fromImage( *m_pCurrFrameQImage ) );
}

bool SubWindowHandle::save()
{
  QString supported = tr( "Supported Files" );
  QString formats = InputStream::supportedWriteFormats();
  formats.prepend( " (" );
  formats.append( ")" );
  supported.append( formats );  // supported=="Supported Files (*.pbm *.jpg...)"

  QStringList filter;
  filter << supported << InputStream::supportedWriteFormatsList() << tr( "All Files (*)" );

  QString fileName = QFileDialog::getSaveFileName( this, tr( "Save As" ), m_cCurrFileName, filter.join( ";;" ) );

  if( fileName.isEmpty() )
    return false;

  QApplication::setOverrideCursor( Qt::WaitCursor );

  if( !m_pCurrStream->writeFrame( fileName ) )
  {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning( this, tr( "SCode" ), tr( "Cannot save file %1" ).arg( fileName ) );
    return false;
  }

  QApplication::restoreOverrideCursor();

  return true;
}

bool SubWindowHandle::playEvent()
{
  bool iRet = true;
  if( m_bIsPlaying )
  {
    m_pCurrStream->readFrame();
    if( m_pCurrStream->checkErrors( READING ) )
    {
      QMessageBox::warning( this, tr( "plaYUVer" ), tr( "Cannot read %1." ).arg( m_cCurrFileName ) );
      return false;
    }
    else if( m_pCurrStream->checkErrors( END_OF_SEQ ) )
    {
      iRet = false;
    }
    refreshFrame();
    return iRet;
  }
  return false;
}

Void SubWindowHandle::seekEvent( UInt new_frame_num )
{
  m_pCurrStream->seekInput( new_frame_num );
  m_pCurrStream->readFrame();
  refreshFrame();
}

Void SubWindowHandle::stopEvent()
{
  m_pCurrStream->seekInput( 0 );
  m_pCurrStream->readFrame();
  refreshFrame();
  return;
}

Void SubWindowHandle::normalSize()
{
  m_dScaleFactor = 1.0;
  m_cViewArea->setZoomFactor( m_dScaleFactor );
}

Void SubWindowHandle::zoomToFit()
{
  // Scale to a smaller size that the real to a nicer look
  QSize niceFit( m_cScrollArea->viewport()->size().width() - 10, m_cScrollArea->viewport()->size().height() - 10 );
  if( (Int)m_pCurrStream->getWidth() <= niceFit.width() && (Int)m_pCurrStream->getHeight() <= niceFit.height() )
  {
    normalSize();
    return;
  }
  scaleView( niceFit );
}

Void SubWindowHandle::scaleView( Double factor )
{
  Q_ASSERT( m_cViewArea->image() );
  m_dScaleFactor *= factor;
  m_cViewArea->setZoomFactor( m_dScaleFactor );
  adjustScrollBar( m_dScaleFactor );
}

Void SubWindowHandle::scaleView( Int width, Int height )
{
  scaleView( QSize( width, height ) );
}

Void SubWindowHandle::scaleView( const QSize & size )
{
  QSize imgViewSize( m_pCurrStream->getWidth(), m_pCurrStream->getHeight() );
  QSize newSize = imgViewSize;
  newSize.scale( size, Qt::KeepAspectRatio );

  // Calc the zoom factor
  Double wfactor = 1;
  Double hfactor = 1;
  m_dScaleFactor = 1;

  wfactor = ( Double )newSize.width() / imgViewSize.width();
  hfactor = ( Double )newSize.height() / imgViewSize.height();

  if( wfactor < hfactor )
    scaleView( wfactor );
  else
    scaleView( hfactor );
}

Void SubWindowHandle::adjustScrollBar( Double factor )
{
  QScrollBar *scrollBar = m_cScrollArea->horizontalScrollBar();
  scrollBar->setValue( int( factor * scrollBar->value() + ( ( factor - 1 ) * scrollBar->pageStep() / 2 ) ) );
  scrollBar = m_cScrollArea->verticalScrollBar();
  scrollBar->setValue( int( factor * scrollBar->value() + ( ( factor - 1 ) * scrollBar->pageStep() / 2 ) ) );
}

QSize SubWindowHandle::sizeHint() const
{
  QSize maxSize;  // The size of the parent (viewport widget
                  // of the QMdiArea).

  QWidget *p = parentWidget();
  if( p )
  {
    maxSize = p->size();
  }

  QSize isize = QSize( m_pCurrStream->getWidth() + 50, m_pCurrStream->getHeight() + 50 );

  // If the SubWindowHandle needs more space that the avaiable, we'll give
  // to the subwindow a reasonable size preserving the image aspect ratio.
  if( isize.width() < maxSize.width() && isize.height() < maxSize.height() )
  {
    return isize;
  }
  else
  {
    isize.scale( maxSize, Qt::KeepAspectRatio );
    maxSize = isize;
  }

  return maxSize;
}

Void SubWindowHandle::closeEvent( QCloseEvent *event )
{
  event->accept();
}

QString SubWindowHandle::userFriendlyCurrentFile()
{
  return m_cWindowName;
}

}  // NAMESPACE
