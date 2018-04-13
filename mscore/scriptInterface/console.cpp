//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#include "console.h"
#include "../icons.h"
#include "../preferences.h"

#include <QObject>
#include <QWidget>
#include <QPlainTextEdit>
#include <QClipboard>
#include <QApplication>
#include <QString>
#include <QTextCharFormat>
#include <QTextCursor>

namespace Ms {

//---------------------------------------------------------
//   Console
//     default ctor
//---------------------------------------------------------

Console::Console(QWidget* parent)
      : QPlainTextEdit(parent)
      {
      if (objectName().isEmpty())
            setObjectName("Console");

      m_originalFontSize = fontSize();

      setToolTip(tr("This is the console"));
      setWhatsThis(tr("This is the console"));
      setAccessibleName(tr("Console"));
      setAccessibleDescription(tr("This is where your plugin outputs."));
      setDocumentTitle(tr("Console output"));
      setLineWrapMode(QPlainTextEdit::NoWrap);
      setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByKeyboard
                              | Qt::TextInteractionFlag::TextSelectableByMouse
                              | Qt::TextInteractionFlag::LinksAccessibleByKeyboard
                              | Qt::TextInteractionFlag::LinksAccessibleByMouse);
      // since it can be erased with clear(), should this sill be?
      setMaximumBlockCount(1000);
      setPlaceholderText(tr("Console empty..."));

      contextMenu = new QMenu(this);

      QAction *clearAct = new QAction(tr("Clear"));
      clearAct->setToolTip(tr("CLear output pane"));
      clearAct->setIcon(*icons[int(Icons::reset_ICON)]);
      contextMenu->addAction(clearAct);

      QAction *copyAllAct = new QAction(tr("Copy to clipboard"));
      copyAllAct->setToolTip(tr("Copy contents to clipboard"));
      copyAllAct->setIcon(*icons[int(Icons::copy_ICON)]);
      contextMenu->addAction(copyAllAct);
      contextMenu->addSeparator();

      QAction *increaseFontSizeAct = new QAction(tr("Increase font size"));
      increaseFontSizeAct->setToolTip(tr("Increase font point size\nCurrent: ") + fontSize() + tr("pts", "as in a font size."));
      increaseFontSizeAct->setIcon(*icons[int(Icons::plus_ICON)]);
      contextMenu->addAction(increaseFontSizeAct);

      QAction *decreaseFontSizeAct = new QAction(tr("Decrease font size"));
      decreaseFontSizeAct->setToolTip(tr("Decrease font point size\nCurrent: ") + fontSize() + tr("pts", "as in a font size."));
      decreaseFontSizeAct->setIcon(*icons[int(Icons::minus_ICON)]);
      contextMenu->addAction(decreaseFontSizeAct);

      QAction *resetFontSizeAct = new QAction(tr("Reset font size"));
      resetFontSizeAct->setToolTip(tr("Reset font point size to: ") + m_originalFontSize + tr("pts", "as in a font size."));
      resetFontSizeAct->setIcon(*icons[int(Icons::equal_ICON)]);
      contextMenu->addAction(resetFontSizeAct);

      for (QAction* act: contextMenu->actions())
            act->setIconVisibleInMenu(true);

      connect(clearAct,            SIGNAL(triggered(bool)), SLOT(clear()));
      connect(copyAllAct,          SIGNAL(triggered(bool)), SLOT(copyAllToClipboard()));
      connect(increaseFontSizeAct, SIGNAL(triggered(bool)), SLOT(increaseFontSize()));
      connect(decreaseFontSizeAct, SIGNAL(triggered(bool)), SLOT(decreaseFontSize()));
      connect(resetFontSizeAct,    SIGNAL(triggered(bool)), SLOT(resetFontSize()));
      }

//---------------------------------------------------------
//   ~Console
//     dtor
//---------------------------------------------------------

Console::~Console()
{}

//---------------------------------------------------------
//   setFontSize
//---------------------------------------------------------

void Console::setFontSize(int pointSize)
      {
      QFont f = font();
      f.setPointSize(pointSize);
      setFont(f);
      }

//---------------------------------------------------------
//   increaseFontSize
//---------------------------------------------------------

void Console::increaseFontSize()
{
      int ps = fontSize();
      if (ps >= 64)
            return;

      if (ps < 2) {
            setFontSize(2);
            return;
            }

      int i = 0;
      for (i = 0; i <= 100; ++i) {
            if (FontSizes[i] >= ps)
                  break;
            }
      setFontSize(FontSizes[i + 1]);
      }

//---------------------------------------------------------
//   decreaseFontSize
//---------------------------------------------------------

void Console::decreaseFontSize()
      {
      int ps = fontSize();
      if (ps <= 2)
            return;

      if (ps > 64) {
            setFontSize(64);
            return;
            }

      int i = 0;
      for (i = 0; i <= 100; ++i) {
            if (FontSizes[i] >= ps)
                  break;
            }

      setFontSize(FontSizes[i - 1]);
      }

//---------------------------------------------------------
//   copyAllToClipboard
//---------------------------------------------------------

void Console::copyAllToClipboard()
      {
      QClipboard *clipboard = QApplication::clipboard();
      clipboard->setText(toPlainText());
      }

//---------------------------------------------------------
//   grayOutContents
//---------------------------------------------------------

void Console::grayOutContents()
      {
      QTextCursor c = textCursor();
      c.select(QTextCursor::Document);
      QTextCharFormat f = c.charFormat();
      f.setForeground(QBrush(QColor(Qt::gray)));
      setCurrentCharFormat(f);
      c.setCharFormat(f);
      c.clearSelection(); //unselect.
      setTextCursor(c);
      }

//---------------------------------------------------------
//   contextMenuEvent
//---------------------------------------------------------

#ifndef QT_NO_CONTEXTMENU
void Console::contextMenuEvent(QContextMenuEvent* event)
      {
      contextMenu->exec(event->globalPos());
      }
#endif // QT_NO_CONTEXTMENU

} //namespace Ms
