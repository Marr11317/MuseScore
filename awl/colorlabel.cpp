//=============================================================================
//  Awl
//  Audio Widget Library
//  $Id:$
//
//  Copyright (C) 2002-2006 by Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#include "colorlabel.h"

#include <QPainter>

namespace Awl {

//---------------------------------------------------------
//   ColorLabel
//---------------------------------------------------------

ColorLabel::ColorLabel(QWidget* parent)
   : QFrame (parent)
      {
      _color  = Qt::blue;
      _pixmap = 0;
      _text = "";
      setCursor(Qt::PointingHandCursor);
      }

ColorLabel::~ColorLabel()
      {
      delete _pixmap;
      }

//---------------------------------------------------------
//   setColor
//---------------------------------------------------------

void ColorLabel::setColor(const QColor& c)
      {
      _color = c;
      emit colorChanged(_color);
      update();
      }

//---------------------------------------------------------
//   color
//---------------------------------------------------------

QColor ColorLabel::color() const
      {
      return _color;
      }

//---------------------------------------------------------
//   setPixmap
//---------------------------------------------------------

void ColorLabel::setPixmap(QPixmap* pm)
      {
      delete _pixmap;
      _pixmap = pm;
      emit pixmapChanged(_pixmap);
      update();
      }

//---------------------------------------------------------
//   sizeHint
//---------------------------------------------------------

QSize ColorLabel::sizeHint() const
      {
      return QSize(30, 20);
      }

//---------------------------------------------------------
//   pixmap
//---------------------------------------------------------

QPixmap* ColorLabel::pixmap() const
      {
      return _pixmap;
      }

//---------------------------------------------------------
//   text
//---------------------------------------------------------

QString ColorLabel::text() const
      {
      return _text;
      }

//---------------------------------------------------------
//   setText
//---------------------------------------------------------

void ColorLabel::setText(const QString& text)
      {
      _text = text;
      emit textChanged(text);
      update();
      }

//---------------------------------------------------------
//   paintEvent
//---------------------------------------------------------

void ColorLabel::paintEvent(QPaintEvent* ev)
      {
      {
      QPainter p(this);
      int fw = frameWidth();
      QRect r(frameRect().adjusted(fw, fw, -2 * fw, -2 * fw));
      if (_pixmap)
            p.drawTiledPixmap(r, *_pixmap);
      else {
            p.fillRect(r, _color);
            if (!_text.isEmpty()) {
                  // Get a visible text: some gray that is the opposite of the pixel's lightness.
                  // 1: Get the average value of R, G and B.
                  // 2: Then add it 128 (approximately 255 / 2), to get the opposite darkness.
                  // 3: % 255, so that it's not greater then 255, which would cause unexpected behaviour.
                  //  This gives a gray that is lighter if the original color is dark,
                  //  and darker if the original color is light.
                  int grayLevel = ((((_color.red() + _color.green() + _color.blue()) / 3) + 128) % 255);
                  p.setPen(QColor(grayLevel, grayLevel, grayLevel));
                  p.drawText(r, _text, QTextOption(Qt::AlignCenter));
                  }
            }
      }
      QFrame::paintEvent(ev);
      }

//---------------------------------------------------------
//   mousePressEvent
//---------------------------------------------------------

void ColorLabel::mousePressEvent(QMouseEvent*)
      {
      if (_pixmap)
            return;
      QColor c = QColorDialog::getColor(_color, this,
         tr("Select Color"),
         QColorDialog::ShowAlphaChannel
         );
      if (c.isValid()) {
            if (_color != c) {
                  _color = c;
                  emit colorChanged(_color);
                  update();
                  }
            }
      }

} // namespace Awl

