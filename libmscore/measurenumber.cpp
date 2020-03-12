//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2011-2014 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#include "score.h"
#include "measurenumber.h"
// #include "xml.h"
#include "measure.h"
#include "staff.h"

#include <QPointF>
#include <QVariant>
#include <QRectF>

namespace Ms {

//---------------------------------------------------------
//   measureNumberStyle
//---------------------------------------------------------

static const ElementStyle measureNumberStyle {
      { Sid::measureNumberVPlacement, Pid::PLACEMENT },
      };

//---------------------------------------------------------
//   MeasureNumber
//---------------------------------------------------------

MeasureNumber::MeasureNumber(Score* s) : TextBase(s, Tid::MEASURE_NUMBER)
      {
      setFlag(ElementFlag::ON_STAFF, true);
      initElementStyle(&measureNumberStyle);
      }

//---------------------------------------------------------
//   propertyDefault
//---------------------------------------------------------

QVariant MeasureNumber::propertyDefault(Pid id) const
      {
      switch(id) {
            case Pid::SUB_STYLE:
                  return int(Tid::MEASURE_NUMBER);
            case Pid::PLACEMENT:
                  return int(Placement::ABOVE);
            default:
                  return TextBase::propertyDefault(id);
            }
      }

//---------------------------------------------------------
//   layout
//---------------------------------------------------------

void MeasureNumber::layout()
      {
      setPos(QPointF());
      if (!parent())
            setOffset(0.0, 0.0);

      // if there is no measure, we cannot return as the element needs to get layout
      // even if there's no score attached to it. This happens for example in the palettes.
      if (measure()) {
            const Staff* stf = staff();
            const StaffType* st = stf->constStaffType(measure()->tick());
            // if there is only one line, the barlines span outside the ledger line, so the default position is not correct
            if (st->lines() == 1) {
                  if (placeBelow())
                        rypos() = -2.0 * score()->styleV(Sid::measureNumberOffset).value<QPointF>().ry() * spatium() + bbox().height();
                  }
            else {
                  if (placeBelow())
                        rypos() = stf->height() - 2.0 * score()->styleV(Sid::measureNumberOffset).value<QPointF>().ry() * spatium() + bbox().height();
                  }
            }
      else
            rypos() = 0.0;
      TextBase::layout1();
      }

} // namespace MS

