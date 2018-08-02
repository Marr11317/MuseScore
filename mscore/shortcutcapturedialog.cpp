//=============================================================================
//  MusE Score
//  Linux Music Score Editor
//  $Id: shortcutcapturedialog.cpp 5537 2012-04-16 07:55:10Z wschweer $
//
//  Copyright (C) 2002-2007 Werner Schweer and others
//  Copyright (C) 2003 Mathias Lundgren (lunar_shuttle@users.sourceforge.net)
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

#include "shortcutcapturedialog.h"
#include "musescore.h"
#include "shortcut.h"

namespace Ms {

//---------------------------------------------------------
//   ShortcutCaptureDialog
//---------------------------------------------------------

ShortcutCaptureDialog::ShortcutCaptureDialog(Shortcut* shortcut, QMap<QString, Shortcut*> ls, QWidget* parent)
   : QDialog(parent)
      {
      setObjectName("ShortcutCaptureDialog");
      setupUi(this);
      setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
      localShortcuts = ls;
      _shortcut = shortcut;

      addButton->setEnabled(false);
      replaceButton->setEnabled(false);
      oshrtLabel->setText(_shortcut->keysToString());
      oshrtTextLabel->setAccessibleDescription(_shortcut->keysToString());
      oshrtLabel->setEnabled(false);
      connect(clearButton, SIGNAL(clicked()), SLOT(clearClicked()));
      connect(addButton, SIGNAL(clicked()), SLOT(addClicked()));
      connect(replaceButton, SIGNAL(clicked()), SLOT(replaceClicked()));
      clearClicked();

      nshrtLabel->installEventFilter(this);
      MuseScore::restoreGeometry(this);
      }

//---------------------------------------------------------
//   addClicked
//---------------------------------------------------------

void ShortcutCaptureDialog::addClicked()
      {
      done(1);
      }

//---------------------------------------------------------
//   replaceClicked
//---------------------------------------------------------

void ShortcutCaptureDialog::replaceClicked()
      {
      done(2);
      }

//---------------------------------------------------------
//   ShortcutCaptureDialog
//---------------------------------------------------------

ShortcutCaptureDialog::~ShortcutCaptureDialog()
      {
      nshrtLabel->removeEventFilter(this);
      releaseKeyboard();
      delete _shortcut;
      }

//---------------------------------------------------------
//   ShortcutCaptureDialog
//---------------------------------------------------------

bool ShortcutCaptureDialog::checkForConflicts()
      {
      // Check against conflicting shortcuts
      bool conflict = false;
      QString msgString;

      for (Shortcut* ss : localShortcuts) {
            if (_shortcut == ss)
                  continue;
            if (!(_shortcut->state() & ss->state()))    // no conflict if states do not overlap
                  continue;

            QList<QKeySequence> skeys = QKeySequence::keyBindings(ss->standardKey());

            for (const QKeySequence& ks : skeys) {
                  if (ks == _keySequence) {
                        msgString = tr("Shortcut conflicts with %1").arg(ss->descr());
                        conflict = true;
                        break;
                        }
                  }

            for (const QKeySequence& ks : ss->keys()) {
                  if (ks == _keySequence) {
                        msgString = tr("Shortcut conflicts with %1").arg(ss->descr());
                        conflict = true;
                        break;
                        }
                  }
            if (conflict)
                  break;
            }

      messageLabel->setText(msgString);

      if (conflict) {
            if (!nshrtLabel->accessibleName().contains(tr("Shortcut conflicts with")))
                  nshrtLabel->setAccessibleName(msgString);
            }
      else {
            if (!nshrtLabel->accessibleName().contains("New shortcut"))
                  nshrtLabel->setAccessibleName(tr("New shortcut"));
            }
      addButton->setEnabled(!conflict);
      replaceButton->setEnabled(!conflict);
      return conflict;
      }

//---------------------------------------------------------
//   eventFilter
//---------------------------------------------------------

bool ShortcutCaptureDialog::eventFilter(QObject* /*o*/, QEvent* e)
      {
      if (e->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);
            if(!(keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Backtab)){
                  keyPress(keyEvent);
                  return true;
                  }
            }
      return false;
      }


//---------------------------------------------------------
//   keyPressEvent
//---------------------------------------------------------

void ShortcutCaptureDialog::keyPress(QKeyEvent* event)
      {
      // check if there is too much keys pressed.
      if (event->count() > 4)
            return;

      int pressedKey = event->key();
      // add the modifiers
      int pressedKeys = pressedKey + event->modifiers();
      // remove shiftModifier for keys that don't need it: letters and special keys
      if ((pressedKeys & Qt::ShiftModifier) && ((pressedKey < 0x41)
                                                || (pressedKey > 0x5a)
                                                || (pressedKey >= 0x01000000)))
            pressedKeys -= Qt::ShiftModifier;

      // append the pressedKeys to the key sequence.
      switch(_keySequence.count()) {
            case 0: _keySequence = QKeySequence(pressedKeys); break;
            case 1: _keySequence = QKeySequence(_keySequence[0], pressedKeys); break;
            case 2: _keySequence = QKeySequence(_keySequence[0], _keySequence[1], pressedKeys); break;
            case 3: _keySequence = QKeySequence(_keySequence[0], _keySequence[1], _keySequence[2], pressedKeys); break;
            default:
                  qDebug("Internal error: bad key count");
                  break;
            }
      // Qt's QShortcut::toString does not handle the keypad modifier
      nshrtLabel->setText(Shortcut::keySeqToString(_keySequence, QKeySequence::NativeText));

      // if the pressed key is a modifier, don't check if there is conflicts..
      if (pressedKey == 0 || pressedKey == Qt::Key_Shift || pressedKey == Qt::Key_Control ||
         pressedKey == Qt::Key_Meta || pressedKey == Qt::Key_Alt || pressedKey == Qt::Key_AltGr
         || pressedKey == Qt::Key_CapsLock || pressedKey == Qt::Key_NumLock
         || pressedKey == Qt::Key_ScrollLock || pressedKey == Qt::Key_unknown) {
            return;
            }

      checkForConflicts();
      }



//---------------------------------------------------------
//   clearClicked
//---------------------------------------------------------

void ShortcutCaptureDialog::clearClicked()
      {
      if (!nshrtLabel->accessibleName().contains("New shortcut"))
            nshrtLabel->setAccessibleName(tr("New shortcut"));

      nshrtLabel->setAccessibleName(tr("New shortcut"));
      messageLabel->setText("");
      addButton->setEnabled(false);
      replaceButton->setEnabled(false);
      nshrtLabel->setText("");
      _keySequence = 0;
      nshrtLabel->setFocus();
      }

//---------------------------------------------------------
//   hideEvent
//---------------------------------------------------------

void ShortcutCaptureDialog::hideEvent(QHideEvent* event)
      {
      MuseScore::saveGeometry(this);
      QWidget::hideEvent(event);
      }
}

