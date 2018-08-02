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

#include <QKeyEvent>
#include <QPainter>

namespace Awl {

//---------------------------------------------------------
//   ColorLabel
//---------------------------------------------------------

ColorLabel::ColorLabel(QWidget* parent)
   : QPushButton (parent)
      {
      _color  = Qt::blue;
      _pixmap = 0;
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
//   get
//---------------------------------------------------------

void ColorLabel::getColor()
      {
      QColor c = QColorDialog::getColor(_color, this,
         tr("Select Color"),
         QColorDialog::ShowAlphaChannel
         );
      if (c.isValid()) {
            if (_color != c) {
                  _color = c;
                  QPalette palette(_color);
                  setPalette(palette);
                  emit colorChanged(_color);
                  update();
                  }
            }
      }

//---------------------------------------------------------
//   color
//---------------------------------------------------------

const QColor ColorLabel::color() const
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
//   pixmap
//---------------------------------------------------------

QPixmap* ColorLabel::pixmap() const
      {
      return _pixmap;
      }

//---------------------------------------------------------
//   paintEvent
//---------------------------------------------------------

void ColorLabel::paintEvent(QPaintEvent* ev)
      {
//      {
      QPainter p(this);
//      int fw = frameWidth();
//      QRect r = QRect(frameRect().adjusted(fw, fw, -2 * fw, -2 * fw));
//      setBackgroundRole(QPalette::ColorRole);
      if (_pixmap)
            p.drawTiledPixmap(rect(), *_pixmap);
//      else {
//            p.fillRect(r, _color);
//            if (hasFocus()) {
//                  // Get a visible text: white if the text is dark and black if it's light.
//                  // Get the average of R, G and B. If it's greater than or equal to 128,
//                  // then consider that the text is light.
//                  p.setPen(QColor((((_color.red() + _color.green() + _color.blue()) / 3) >= 128) ? Qt::black : Qt::white));
//                  p.drawRect(rect());
//                  }
//            if (!text().isEmpty()) {
//                  // Get a visible text: white if the text is dark and black if it's light.
//                  // Get the average of R, G and B. If it's greater than or equal to 128,
//                  // then consider that the text is light.
//                  p.setPen(QColor((((_color.red() + _color.green() + _color.blue()) / 3) >= 128) ? Qt::black : Qt::white));
//                  p.drawText(rect(), text(), QTextOption(Qt::AlignCenter));
//                  }
//            }
//      }
      QPushButton::paintEvent(ev);
      }

} // namespace Awl

