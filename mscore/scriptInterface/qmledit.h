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

#ifndef __QML_EDIT_H__
#define __QML_EDIT_H__

#include "globals.h"

#include <QMenu>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QTextBlock>

// #define QMLEDIT_BINDINGS

namespace Ms {

class JSHighlighter;
class LineNumberArea;

//---------------------------------------------------------
//   QmlEdit
//---------------------------------------------------------

class QmlEdit : public QPlainTextEdit {
      Q_OBJECT

      const unsigned char FontSizes[20] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14,
                                           16, 18, 20, 22, 24, 28, 36, 48, 64};

      LineNumberArea *lineNumberArea;
      JSHighlighter *hl;
      // was disabled in order to prevent MuseScore in background
      // to always disable and enable it's tool buttons.
      ///ScoreState mscoreState;
      QString pickBuffer;

      QMenu *contextMenu;
      int m_originalFontSize;

      //virtual void focusInEvent(QFocusEvent *) override;
      //virtual void focusOutEvent(QFocusEvent *) override;
      void move(QTextCursor::MoveOperation);
      void moveToPos(int pos);
      virtual void keyPressEvent(QKeyEvent *) override;
      void tab();
      void autoIndent();
      void moveLine(bool up); //if it is not up, it down.

   private slots:
      void updateLineNumberAreaWidth();
      void highlightCurrentLine();
      //void highlightMatchingBraces();
      void updateLineNumberArea(const QRect&, int);

#ifdef QMLEDIT_BINDINGS // uncomment it at the top of the document.
      void startOfLine() { move(QTextCursor::StartOfLine); }
      void endOfLine()   { move(QTextCursor::EndOfLine); }
      void upLine()      { move(QTextCursor::Udefine it on top of qmledit.hp); }
      void downLine();
      void right()       { move(QTextCursor::Right); }
      void left()        { move(QTextCursor::Left);  }
      void rightWord()   { move(QTextCursor::NextWord); }
      void start()       { move(QTextCursor::Start); }
      void end()         { move(QTextCursor::End);   }
      void leftWord();
      void pick();
      void put();
      void delLine();
      void delWord();
#endif // QMLEDIT_BINDINGS

   protected:
      virtual void resizeEvent(QResizeEvent *) override;

#ifndef QT_NO_CONTEXTMENU
      virtual void contextMenuEvent(QContextMenuEvent *) override;
#endif // QT_NO_CONTEXTMENU

   public:
      QmlEdit(QWidget* parent = nullptr);
      ~QmlEdit();

      virtual void lineNumberAreaPaintEvent(QPaintEvent *);
      int lineNumberAreaWidth() const;
      enum ColorComponent { Normal, Comment, Number, String, Operator, Identifier,
         Keyword, BuiltIn, Marker };

      int fontSize() const;
      void setFontSize(int);

   public slots:
      void decreaseFontSize();
      void increaseFontSize();
      void resetFontSize();

      void duplicateLine();
      void moveLineUp() { moveLine(true); }
      void moveLineDown() { moveLine(false); }

   signals:
      }; // class QmlEdit

//---------------------------------------------------------
//   LineNumberArea
//---------------------------------------------------------

class LineNumberArea : public QWidget {
      Q_OBJECT
      QmlEdit* editor;

      QSize sizeHint() const {
            return QSize(editor->lineNumberAreaWidth(), 0);
            }
      virtual void paintEvent(QPaintEvent* event) override {
            editor->lineNumberAreaPaintEvent(event);
            }

   public:
      LineNumberArea(QmlEdit* parent) : QWidget(parent) { editor = parent; }
      }; //class LineNumberArea

//---------------------------------------------------------
//   JSBlockData
//---------------------------------------------------------

class JSBlockData : public QTextBlockUserData {
   public:
      QList<int> bracketPositions;
      }; // class JSBlockData

//---------------------------------------------------------
//   JSHighlighter
//---------------------------------------------------------

class JSHighlighter : public QSyntaxHighlighter {
      QSet<QString> m_keywords;
      QSet<QString> m_knownIds;
      QHash<QmlEdit::ColorComponent, QColor> m_colors;
      QString m_markString;
      Qt::CaseSensitivity m_markCaseSensitivity;

   protected:
      void highlightBlock(const QString &text);

   public:
      JSHighlighter(QTextDocument *parent = nullptr);
      void setColor(QmlEdit::ColorComponent component, const QColor &color);
      void mark(const QString &str, Qt::CaseSensitivity caseSensitivity);
      QStringList keywords() const;
      void setKeywords(const QStringList &keywords);
      }; // class JSHighlighter
} // namespace Ms
#endif

