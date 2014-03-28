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
 * \file     imageinterface.h
 * \brief    Sub windows handling
 */

#ifndef __IMAGEINTERFACE_H__
#define __IMAGEINTERFACE_H__

#include "config.h"

#if( QT_VERSION_PLAYUVER == 5 )
#include <QtWidgets>
#elif( QT_VERSION_PLAYUVER == 4 )
#include <QtGui>
#endif

#include "TypeDef.h"
#include "InputStream.h"
#include "ViewArea.h"
#include "PlaYUVerModuleIf.h"

namespace plaYUVer
{

class SubWindowHandle: public QMdiSubWindow
{
Q_OBJECT

private:

  QScrollArea* m_cScrollArea;
  ViewArea* m_cViewArea;

  PlaYUVerModuleIf* m_pcCurrentModule;

  InputStream* m_pCurrStream;
  QImage* m_pCurrFrameQImage;

  QString m_cWindowName;
  QString m_cCurrFileName;

  Bool m_bIsPlaying;

public:
  SubWindowHandle( QWidget * parent = 0 );
  ~SubWindowHandle();

  Bool mayClose();
  Bool loadFile( const QString &fileName );
  Bool save();

  Void stopEvent();
  Void seekAbsoluteEvent( UInt new_frame_num );
  Void seekRelativeEvent( Bool bIsFoward );

  QImage* FrameToQImage( PlaYUVerFrame* curr_frame );

  Void refreshFrame();

  InputStream* getInputStream()
  {
    return m_pCurrStream;
  }

  ViewArea* getViewArea()
  {
    return m_cViewArea;
  }

  /**
   * Functions to enable a module in the
   * current SubWindow
   */
  Void enableModule( PlaYUVerModuleIf* select_module );
  Void disableModule();

  /**
   * Show the image at its original size
   */
  Void normalSize();
  /**
   * Scale the image (zoomed in or out) to fit on the window.
   */
  Void zoomToFit();
  /**
   * Scale the image by a given factor
   * @param factor factor of scale. Ex: 1.2 scale the image up by 20% and
   *        0.8 scale the image down by 25%
   */
  Void scaleView( Double factor );

  /**
   * The current image size is scaled to a rectangle as large as possible
   * inside (@p width, @p height ) preserving the aspect ratio.
   */
  Void scaleView( Int width, Int height );

  /**
   * This is an overloaded member function, provided for convenience.
   * Scales the image to a rectangle with the given size, preserving the
   * aspect ratio.
   */
  Void scaleView( const QSize & size );

  Double getScaleFactor()
  {
    return m_cViewArea->getZoomFactor();
  }

  QSize sizeHint() const;

  QString userFriendlyCurrentFile()
  {
    return m_cWindowName;
  }

  QString currentFile()
  {
    return m_cCurrFileName;
  }

protected:
  void closeEvent( QCloseEvent *event );

public Q_SLOTS:
  bool playEvent();
  void adjustScrollBar( double factor );
};

}  // NAMESPACE

#endif // __IMAGEINTERFACE_H__
