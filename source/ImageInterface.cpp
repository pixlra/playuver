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

#include <QtWidgets>
#include <QWidget>
#include <QPixmap>
#include <QBitmap>
#include <QColor>

#include "ConfigureResolutionDialog.h"
#include "ImageInterface.h"
#include "viewarea.h"

namespace plaYUVer
{


ImageInterface::ImageInterface( QWidget * parent ) :
    QScrollArea( parent )
{
  setParent( parent );

  setAttribute( Qt::WA_DeleteOnClose );
  isUntitled = true;

  m_cViewArea = new ViewArea( this );

//  m_cViewArea = new QLabel( this );
//  m_cViewArea->setScaledContents( true );
//  m_cViewArea->setBackgroundRole( QPalette::Base );
//  //m_cViewArea->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
//  m_cViewArea->setScaledContents( false );

  setWidgetResizable( true );
  setAttribute( Qt::WA_DeleteOnClose );
  setMinimumSize( 200, 200 );
  setBackgroundRole( QPalette::Dark );
  setWidget( m_cViewArea );

  fileExists = false;
  m_firsTime = true;
  m_dScaleFactor = 1;

}

ImageInterface::~ImageInterface()
{
  delete m_cViewArea;
}

bool ImageInterface::loadFile( const QString &fileName )
{
  ConfigureResolutionDialog resolutionDialog;

  if( resolutionDialog.exec() == QDialog::Rejected )
  {
    return false;
  }
  
  QApplication::setOverrideCursor( Qt::WaitCursor );

  m_currStream.init( fileName, resolutionDialog.getResolution().width(), resolutionDialog.getResolution().height() );

  m_currStream.readFrame();
  if( m_currStream.checkErrors( READING ) )
  {
    QMessageBox::warning( this, tr( "plaYUVer" ), tr( "Cannot read %1." ).arg( fileName ) );
    return false;
  }

  QApplication::restoreOverrideCursor();

  //QImage image( fileName );
  //m_cViewArea->setImage( QPixmap::fromImage( image ) );

  m_cViewArea->setImage( QPixmap::fromImage( m_currStream.getFrameQImage() ) );

  normalSize();

  m_cCurrFileName = fileName;

  return true;
}

bool ImageInterface::save()
{
  if( isUntitled )
  {
    return saveAs();
  }
  else
  {
    return saveFile( m_cCurrFileName );
  }
}

bool ImageInterface::saveAs()
{
  QString fileName = QFileDialog::getSaveFileName( this, tr( "Save As" ), m_cCurrFileName );
  if( fileName.isEmpty() )
    return false;

  return saveFile( fileName );
}

bool ImageInterface::saveFile( const QString &fileName )
{

  return true;
}

Void ImageInterface::normalSize()
{
  m_dScaleFactor = 1.0;
  //m_cViewArea->resize( m_dScaleFactor * m_cViewArea->pixmap()->size() );
  m_cViewArea->setZoomFactor( m_dScaleFactor );
}

Void ImageInterface::zoomToFit()
{
  // Scale to a smaller size that the real to a nicer look
  QSize niceFit( viewport()->size().width() - 10, viewport()->size().height() - 10 );

  if( m_currStream.getWidth() <= niceFit.width() && m_currStream.getHeight() <= niceFit.height() )
  {
    normalSize();
    return;
  }
  scaleView( niceFit );
}

Void ImageInterface::scaleView( Double factor )
{
  //Q_ASSERT( m_cViewArea->pixmap() );
  m_dScaleFactor *= factor;
  m_cViewArea->setZoomFactor( m_dScaleFactor );
  //m_cViewArea->resize( m_dScaleFactor * m_cViewArea->pixmap()->size() );
  adjustScrollBar( m_dScaleFactor );
}

Void ImageInterface::scaleView( Int width, Int height )
{
  scaleView( QSize( width, height ) );
}

Void ImageInterface::scaleView( const QSize & size )
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

Void ImageInterface::adjustScrollBar( Double factor )
{
  QScrollBar *scrollBar = horizontalScrollBar();
  scrollBar->setValue( int( factor * scrollBar->value() + ( ( factor - 1 ) * scrollBar->pageStep() / 2 ) ) );
  scrollBar = verticalScrollBar();
  scrollBar->setValue( int( factor * scrollBar->value() + ( ( factor - 1 ) * scrollBar->pageStep() / 2 ) ) );
}

/*
 Note: Code imported from Qt4.4
 */

static bool sanityCheck( const QList<QWidget *> &widgets, const int index, const char *where )
{
  if( index < 0 || index >= widgets.size() )
  {
    const char error[] = "index out of range";
    Q_ASSERT_X( false, where, error );
    qWarning( "%s:%s", where, error );
    return false;
  }
  if( !widgets.at( index ) )
  {
    const char error[] = "null pointer";
    Q_ASSERT_X( false, where, error );
    qWarning( "%s:%s", where, error );
    return false;
  }
  return true;
}

QSize ImageInterface::sizeHint() const
{
  QSize maxSize; // The size of the parent (viewport widget
                 // of the QMdiArea).

  QWidget *p = parentWidget();
  if( p )
  {
    maxSize = p->size();
  }

  QSize isize = QSize( m_currStream.getWidth() + 500, m_currStream.getHeight() + 500 );

  // If the ImageInterface needs more space that the avaiable, we'll give
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

void ImageInterface::cascader( QList<QWidget *> &widgets, const QRect &domain )
{
  if( widgets.isEmpty() )
    return;

  // Tunables:
  const int topOffset = 0;
  const int bottomOffset = 50;
  const int leftOffset = 0;
  const int rightOffset = 100;
  const int dx = 10;

  QStyleOptionTitleBar options;
  options.initFrom( widgets.at( 0 ) );
  int titleBarHeight = widgets.at( 0 )->style()->pixelMetric( QStyle::PM_TitleBarHeight, &options, widgets.at( 0 ) );
#if defined(Q_WS_MAC) && !defined(QT_NO_STYLE_MAC)
  // ### Remove this after the mac style has been fixed
  if ( qobject_cast<QMacStyle *>( widgets.at( 0 )->style() ) )
  titleBarHeight -= 4;
#endif
  const QFontMetrics fontMetrics = QFontMetrics( QApplication::font( "QWorkspaceTitleBar" ) );
  const int dy = qMax( titleBarHeight - ( titleBarHeight - fontMetrics.height() ) / 2, 1 );

  const int n = widgets.size();
  const int nrows = qMax( ( domain.height() - ( topOffset + bottomOffset ) ) / dy, 1 );
  const int ncols = qMax( n / nrows + ( ( n % nrows ) ? 1 : 0 ), 1 );
  const int dcol = ( domain.width() - ( leftOffset + rightOffset ) ) / ncols;

  int i = 0;
  for( int row = 0; row < nrows; ++row )
  {
    for( int col = 0; col < ncols; ++col )
    {
      const int x = leftOffset + row * dx + col * dcol;
      const int y = topOffset + row * dy;
      if( !sanityCheck( widgets, i, "SimpleCascader" ) )
        continue;
      QWidget *widget = widgets.at( i++ );
      QRect newGeometry = QRect( QPoint( x, y ), widget->sizeHint() );
      widget->setGeometry( QStyle::visualRect( widget->layoutDirection(), domain, newGeometry ) );
      if( i == n )
        return;
    }
  }
}

Void ImageInterface::closeEvent( QCloseEvent *event )
{
  event->accept();
}

QString ImageInterface::userFriendlyCurrentFile()
{
  return m_cCurrFileName;
  //return strippedName( m_cCurrFileName );
}

QString ImageInterface::strippedName( const QString &fullFileName )
{
  return QFileInfo( fullFileName ).fileName();
}

} // NAMESPACE
