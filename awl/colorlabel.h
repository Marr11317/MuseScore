//=============================================================================
//  Awl
//  Audio Widget Library
//  $Id:$
//
//  Copyright (C) 2002-2007 by Werner Schweer and others
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

#ifndef __AWLCOLORLABEL_H__
#define __AWLCOLORLABEL_H__

#include <QtWidgets>

namespace Awl {

//---------------------------------------------------------
//   ColorLabel
//---------------------------------------------------------

class ColorLabel : public QPushButton {
      Q_OBJECT
      Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
      Q_PROPERTY(QPixmap* pixmap READ pixmap WRITE setPixmap NOTIFY pixmapChanged)

      QColor _color;
      QPixmap* _pixmap;

      virtual void paintEvent(QPaintEvent*) override;

   signals:
      void colorChanged(const QColor&);
      void pixmapChanged(const QPixmap*);

   public:
      ColorLabel(QWidget* parent = nullptr);
      ~ColorLabel();

      const QColor color() const;
      QPixmap* pixmap() const;

   public slots:
      void setPixmap(QPixmap*);
      void setColor(const QColor&);

      // starts the colordialog.
      void getColor();

};

}  // namespace Awl
#endif

