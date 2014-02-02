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
#include "viewarea.h"

namespace plaYUVer
{

SubWindowHandle::SubWindowHandle( QWidget * parent ) :
    QMdiSubWindow( parent )
{
  setParent( parent );

  setAttribute( Qt::WA_DeleteOnClose );
  setMinimumSize( 500, 500 );
  setBackgroundRole( QPalette::Light );

  // Create a new scroll area inside the sub-window
  m_cScrollArea = new QScrollArea( this );
  setWidget( m_cScrollArea );

  // Create a new interface to show images
  m_cViewArea = new ViewArea( m_cScrollArea );

  // Define the cViewArea as the widget inside the scroll area
  m_cScrollArea->setWidget( m_cViewArea );

  m_cCurrFileName = QString( "" );
  m_dScaleFactor = 1;

}

SubWindowHandle::~SubWindowHandle()
{
  delete m_cViewArea;
}

bool SubWindowHandle::loadFile( const QString &fileName )
{
  ConfigureFormatDialog formatDialog( this );

  if( m_currStream.needFormatDialog( fileName ) )
  {
    if( formatDialog.exec() == QDialog::Rejected )
    {
      return false;
    }
    m_currStream.init( fileName, formatDialog.getResolution().width(), formatDialog.getResolution().height(), formatDialog.getPixelFormat(), formatDialog.getFrameRate() );
  }
  else
  {
    m_currStream.init( fileName, 0, 0, 0, 0 );
  }
  QApplication::setOverrideCursor( Qt::WaitCursor );
  QApplication::restoreOverrideCursor();
  if( m_currStream.getStatus() == 0 )
  {
    return false;
  }

  m_currStream.readFrame();
  if( m_currStream.checkErrors( READING ) )
  {
    QMessageBox::warning( this, tr( "plaYUVer" ), tr( "Cannot read %1." ).arg( fileName ) );
    return false;
  }

  QApplication::restoreOverrideCursor();

  m_cViewArea->setImage( QPixmap::fromImage( m_currStream.getFrameQImage() ) );

  normalSize();

  m_cCurrFileName = fileName;
  setWindowTitle( userFriendlyCurrentFile() + "[*]" );

  return true;
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

  if( !m_currStream.writeFrame( fileName ) )
  {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning( this, tr( "SCode" ), tr( "Cannot save file %1" ).arg( fileName ) );
    return false;
  }

  QApplication::restoreOverrideCursor();

  return true;
}

bool SubWindowHandle::nextVideoFrame()
{
  m_currStream.readFrame();
  if( m_currStream.checkErrors( READING ) )
  {
    QMessageBox::warning( this, tr( "plaYUVer" ), tr( "Cannot read %1." ).arg( m_cCurrFileName ) );
    return false;
  }
  m_cViewArea->setImage( QPixmap::fromImage( m_currStream.getFrameQImage() ) );
  return true;
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
  if( m_currStream.getWidth() <= niceFit.width() && m_currStream.getHeight() <= niceFit.height() )
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
  QSize imgViewSize( m_currStream.getWidth(), m_currStream.getHeight() );
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

  QSize isize = QSize( m_currStream.getWidth() + 500, m_currStream.getHeight() + 500 );

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
  return strippedName( m_cCurrFileName );
}

QString SubWindowHandle::strippedName( const QString &fullFileName )
{
  return QFileInfo( fullFileName ).fileName();
}

}  // NAMESPACE
