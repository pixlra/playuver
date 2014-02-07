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
#include "viewarea.h"

namespace plaYUVer
{

using namespace SCode;

class SubWindowHandle: public QMdiSubWindow
{
Q_OBJECT

private:

  QScrollArea* m_cScrollArea;
  ViewArea* m_cViewArea;

  InputStream m_currStream;

  QString m_cWindowName;
  QString m_cCurrFileName;

  Double m_dScaleFactor;

  Bool m_bIsPlaying;

public:
  SubWindowHandle( QWidget * parent = 0 );
  ~SubWindowHandle();

  bool loadFile( const QString &fileName );
  bool save();

  Void stopEvent();

  InputStream* getInputStream()
  {
    return &m_currStream;
  }

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

  Void adjustScrollBar( Double factor );

  Double getScaleFactor()
  {
    return m_dScaleFactor;
  }

  QSize sizeHint() const;

  /**
   * Arranges all the listed widgets (normaly QMdiImageInterface) in a cascade
   * pattern.
   * @Note This function implementation is taken from the Qt source. The
   * QMdiArea only have a function that arranges @b all the child windows.
   */
  //static void cascader( QList<QWidget *> &widgets, const QRect &domain );
  QString userFriendlyCurrentFile();
  QString currentFile()
  {
    return m_cCurrFileName;
  }

protected:
  Void closeEvent( QCloseEvent *event );

public Q_SLOTS:
  int playEvent();
};

}  // NAMESPACE

#endif // __IMAGEINTERFACE_H__
