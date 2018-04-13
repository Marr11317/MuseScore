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
//
//  Syntax highlighter based on example code from Ariya Hidayat
//  (git://gitorious.org/ofi-labs/x2.git BSD licensed).
//=============================================================================

#include "qmledit.h"
#include "../preferences.h"
#include "../musescore.h"
#include "../icons.h"

#include <QPainter>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QtMath>

namespace Ms {

//---------------------------------------------------------
//   JSHighlighter
//---------------------------------------------------------

JSHighlighter::JSHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
    , m_markCaseSensitivity(Qt::CaseInsensitive)
      {
      setObjectName("JSHighlighter");

      // default color scheme.
      // TODO: load it using a style sheet.
      if (preferences.globalStyle() == MuseScoreStyleType::DARK_FUSION) {
            m_colors[QmlEdit::Normal]     = Qt::white;
            m_colors[QmlEdit::Comment]    = QColor("#55ffff");
            m_colors[QmlEdit::Number]     = QColor("#ff55ff");
            m_colors[QmlEdit::String]     = QColor("#55ff55");
            m_colors[QmlEdit::Operator]   = QColor("#808000");
            m_colors[QmlEdit::Identifier] = QColor("#2020b0");
            m_colors[QmlEdit::Keyword]    = QColor("#ff6414");
            m_colors[QmlEdit::BuiltIn]    = QColor("#ff0000");
            m_colors[QmlEdit::Marker]     = QColor("#bbbb00");
            }
      else {
            m_colors[QmlEdit::Normal]     = QColor("#000000");
            m_colors[QmlEdit::Comment]    = QColor("#606060");
            m_colors[QmlEdit::Number]     = QColor("#008000");
            m_colors[QmlEdit::String]     = QColor("#800000");
            m_colors[QmlEdit::Operator]   = QColor("#808000");
            m_colors[QmlEdit::Identifier] = QColor("#000020");
            m_colors[QmlEdit::Keyword]    = QColor("#000080");
            m_colors[QmlEdit::BuiltIn]    = QColor("#008080");
            m_colors[QmlEdit::Marker]     = QColor("#ffff00");
            }

      // https://developer.mozilla.org/en/JavaScript/Reference/Reserved_Words

      static const char* data1[] = { "break", "case", "catch", "continue",
         "default", "delete", "do", "else", "finally", "for", "function",
         "if", "in", "instanceof", "new", "return", "switch", "this", "throw",
         "try", "typeof", "var", "void", "while", "with", "true", "false",
         "null" };

      for (unsigned int i = 0; i < sizeof(data1)/sizeof(*data1); ++i)
            m_keywords.insert(data1[i]);

      // built-in and other popular objects + properties

      static const char* data2[] = { "Object", "prototype", "create",
         "defineProperty", "defineProperties", "getOwnPropertyDescriptor",
         "keys", "getOwnPropertyNames", "constructor", "__parent__", "__proto__",
         "__defineGetter__", "__defineSetter__", "eval", "hasOwnProperty",
         "isPrototypeOf", "__lookupGetter__", "__lookupSetter__", "__noSuchMethod__",
         "propertyIsEnumerable", "toSource", "toLocaleString", "toString",
         "unwatch", "valueOf", "watch", "Function", "arguments", "arity", "caller",
         "constructor", "length", "name", "apply", "bind", "call", "String",
         "fromCharCode", "length", "charAt", "charCodeAt", "concat", "indexOf",
         "lastIndexOf", "localCompare", "match", "quote", "replace", "search",
         "slice", "split", "substr", "substring", "toLocaleLowerCase",
         "toLocaleUpperCase", "toLowerCase", "toUpperCase", "trim", "trimLeft",
         "trimRight", "Array", "isArray", "index", "input", "pop", "push",
         "reverse", "shift", "sort", "splice", "unshift", "concat", "join",
         "filter", "forEach", "every", "map", "some", "reduce", "reduceRight",
         "RegExp", "global", "ignoreCase", "lastIndex", "multiline", "source",
         "exec", "test", "JSON", "parse", "stringify", "decodeURI",
         "decodeURIComponent", "encodeURI", "encodeURIComponent", "eval",
         "isFinite", "isNaN", "parseFloat", "parseInt", "Infinity", "NaN",
         "undefined", "Math", "E", "LN2", "LN10", "LOG2E", "LOG10E", "PI",
         "SQRT1_2", "SQRT2", "abs", "acos", "asin", "atan", "atan2", "ceil",
         "cos", "exp", "floor", "log", "max", "min", "pow", "random", "round",
         "sin", "sqrt", "tan", "document", "window", "navigator", "userAgent"
         };

      for (unsigned int i = 0; i < sizeof(data2)/sizeof(*data2); ++i)
            m_knownIds.insert(data2[i]);
      }

//---------------------------------------------------------
//   setColor
//---------------------------------------------------------

void JSHighlighter::setColor(QmlEdit::ColorComponent component, const QColor& color)
      {
      m_colors[component] = color;
      rehighlight();
      }

//---------------------------------------------------------
//   highlightBlock
//---------------------------------------------------------

void JSHighlighter::highlightBlock(const QString& text)
      {
      // parsing state
      enum class State : char {
            Start = 0,
            Number = 1,
            Identifier = 2,
            String = 3,
            Comment = 4,
            Regex = 5
            };

      QList<int> bracketPositions;
      int blockState = previousBlockState();
      int bracketLevel = blockState >> 4;
      State state = State(blockState & 15);
      if (blockState < 0) {
            bracketLevel = 0;
            state = State::Start;
            }
      int start = 0;
      int i = 0;
      while (i <= text.length()) {
            QChar ch = (i < text.length()) ? text.at(i) : QChar();
            QChar next = (i < text.length() - 1) ? text.at(i + 1) : QChar();
            switch (state) {
                  case State::Start:
                        start = i;
                        if (ch.isSpace()) {
                              ++i;
                              }
                        else if (ch.isDigit()) {
                              ++i;
                              state = State::Number;
                              }
                        else if (ch.isLetter() || ch == '_') {
                              ++i;
                              state = State::Identifier;
                              }
                        else if (ch == '\'' || ch == '\"') {
                              ++i;
                              state = State::String;
                              }
                        else if (ch == '/' && next == '*') {
                              ++i;
                              ++i;
                              state = State::Comment;
                              }
                        else if (ch == '/' && next == '/') {
                              i = text.length();
                              setFormat(start, text.length(), m_colors[QmlEdit::Comment]);
                              }
                        else if (ch == '/' && next != '*') {
                              ++i;
                              state = State::Regex;
                              }
                        else {
                              if (!QString("(){}[]").contains(ch))
                                    setFormat(start, 1, m_colors[QmlEdit::Operator]);
                              if (ch =='{' || ch == '}') {
                                    bracketPositions += i;
                                    if (ch == '{')
                                          bracketLevel++;
                                    else
                                          bracketLevel--;
                                    }
                              ++i;
                              state = State::Start;
                              }
                        break;
                  case State::Number:
                        if (ch.isSpace() || !ch.isDigit()) {
                              setFormat(start, i - start, m_colors[QmlEdit::Number]);
                              state = State::Start;
                              }
                        else {
                              ++i;
                              }
                        break;
                  case State::Identifier:
                        if (ch.isSpace() || !(ch.isDigit() || ch.isLetter() || ch == '_')) {
                              QString token = text.mid(start, i - start).trimmed();
                              if (m_keywords.contains(token))
                                    setFormat(start, i - start, m_colors[QmlEdit::Keyword]);
                              else if (m_knownIds.contains(token))
                                    setFormat(start, i - start, m_colors[QmlEdit::BuiltIn]);
                              state = State::Start;
                              }
                        else {
                              ++i;
                              }
                        break;
                  case State::String:
                        if (ch == text.at(start)) {
                              QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                              if (prev != '\\') {
                                    ++i;
                                    setFormat(start, i - start, m_colors[QmlEdit::String]);
                                    state = State::Start;
                                    }
                              else {
                                    ++i;
                                    }
                              }
                        else {
                              ++i;
                              }
                        break;
                  case State::Comment:
                        if (ch == '*' && next == '/') {
                              ++i;
                              ++i;
                              setFormat(start, i - start, m_colors[QmlEdit::Comment]);
                              state = State::Start;
                              }
                        else {
                              ++i;
                              }
                        break;
                  case State::Regex:
                        if (ch == '/') {
                              QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                              if (prev != '\\') {
                                    ++i;
                                    setFormat(start, i - start, m_colors[QmlEdit::String]);
                                    state = State::Start;
                                    }
                              else {
                                    ++i;
                                    }
                              }
                        else {
                              ++i;
                              }
                        break;
                  default:
                        state = State::Start;
                        break;
                  }
            }

      if (state == State::Comment)
            setFormat(start, text.length(), m_colors[QmlEdit::Comment]);
      else
            state = State::Start;

      if (!m_markString.isEmpty()) {
            int pos = 0;
            int len = m_markString.length();
            QTextCharFormat markerFormat;
            markerFormat.setBackground(m_colors[QmlEdit::Marker]);
            markerFormat.setForeground(m_colors[QmlEdit::Normal]);
            for (;;) {
                  pos = text.indexOf(m_markString, pos, m_markCaseSensitivity);
                  if (pos < 0)
                        break;
                  setFormat(pos, len, markerFormat);
                        ++pos;
                  }
            }
      if (!bracketPositions.isEmpty()) {
            JSBlockData *blockData = reinterpret_cast<JSBlockData*>(currentBlock().userData());
            if (!blockData) {
                  blockData = new JSBlockData;
                  currentBlock().setUserData(blockData);
                  }
            blockData->bracketPositions = bracketPositions;
            }
      blockState = (int(state) & 15) | (bracketLevel << 4);
      setCurrentBlockState(blockState);
      }

//---------------------------------------------------------
//   mark
//---------------------------------------------------------

void JSHighlighter::mark(const QString &str, Qt::CaseSensitivity caseSensitivity)
      {
      m_markString = str;
      m_markCaseSensitivity = caseSensitivity;
      rehighlight();
      }

//---------------------------------------------------------
//   keywords
//---------------------------------------------------------

QStringList JSHighlighter::keywords() const
      {
      return m_keywords.toList();
      }

//---------------------------------------------------------
//   setKeywords
//---------------------------------------------------------

void JSHighlighter::setKeywords(const QStringList &keywords)
      {
      m_keywords = QSet<QString>::fromList(keywords);
      rehighlight();
      }

#ifdef QMLEDIT_BINDINGS // uncomment it at top of qmledit.h
//---------------------------------------------------------
//   Binding
//---------------------------------------------------------
struct Binding {
      const char* name;
      int key1, key2;
      const char* slot;
      };
#endif // QMLEDIT_BINDINGS

//---------------------------------------------------------
//   QmlEdit
//---------------------------------------------------------

QmlEdit::QmlEdit(QWidget* parent)
   : QPlainTextEdit(parent)
      {
      setObjectName("QmlEdit");
      setBackgroundVisible(true);
      setLineWrapMode(QPlainTextEdit::NoWrap);
      QFont font("FreeMono", 16);
      font.setStyleHint(QFont::TypeWriter);
      font.setFixedPitch(true);
      setFont(font);

      QTextCharFormat cf = currentCharFormat();
      cf.setFont(font);
      setCurrentCharFormat(cf);

#ifdef QMLEDIT_BINDINGS // uncomment it at top of qmledit.h
      static const Binding bindings[] = {
            { "start",       Qt::CTRL + Qt::Key_Q, Qt::CTRL + Qt::Key_E, SLOT(start()) },
            { "end",         Qt::CTRL + Qt::Key_Q, Qt::CTRL + Qt::Key_X, SLOT(end()) },
            { "startOfLine", Qt::CTRL + Qt::Key_Q, Qt::CTRL + Qt::Key_S, SLOT(startOfLine()) },
            { "endOfLine",   Qt::CTRL + Qt::Key_Q, Qt::CTRL + Qt::Key_D, SLOT(endOfLine())   },
            { "up",          Qt::CTRL + Qt::Key_E, 0, SLOT(upLine())     },
            { "down",        Qt::CTRL + Qt::Key_X, 0, SLOT(downLine())   },
            { "right",       Qt::CTRL + Qt::Key_D, 0, SLOT(right())      },
            { "left",        Qt::CTRL + Qt::Key_S, 0, SLOT(left())       },
            { "rightWord",   Qt::CTRL + Qt::Key_F, 0, SLOT(rightWord())  },
            { "leftWord",    Qt::CTRL + Qt::Key_A, 0, SLOT(leftWord())   },
            { "pick",        Qt::Key_F8,           0, SLOT(pick())       },
            { "put",         Qt::Key_F9,           0, SLOT(put())        },
            { "delLine",     Qt::CTRL + Qt::Key_Y, 0, SLOT(delLine())    },
            { "delWord",     Qt::CTRL + Qt::Key_T, 0, SLOT(delWord())    }
            };
#endif // QMLEDIT_BINDINGS
      setTabChangesFocus(false);
      setBackgroundVisible(false);
      setCursorWidth(3);

      QPalette p = palette();
      p.setColor(QPalette::Text, (preferences.globalStyle() == MuseScoreStyleType::DARK_FUSION)
                 ? Qt::lightGray : Qt::black);
      p.setColor(QPalette::Base, (preferences.globalStyle() == MuseScoreStyleType::DARK_FUSION)
                 ? Qt::black : Qt::white);
      if (preferences.globalStyle() == MuseScoreStyleType::DARK_FUSION)
            p.setColor(QPalette::Highlight, QColor("#0a246a"));
      setPalette(p);
      hl = new JSHighlighter(document());
      lineNumberArea = new LineNumberArea(this);

#ifdef QMLEDIT_BINDINGS // uncomment at top of qmledit.h
      for (unsigned int i = 0; i < sizeof(bindings)/sizeof(*bindings); ++i) {
            const Binding& b = bindings[i];
            QAction* a = new QAction(b.name, this);
            a->setShortcut(QKeySequence(b.key1, b.key2));
            a->setShortcutContext(Qt::WidgetShortcut);
            a->setPriority(QAction::HighPriority);
            addAction(a);
            connect(a, SIGNAL(triggered()), b.slot);
            }
#endif // QMLEDIT_BINDINGS

      connect(this, &QPlainTextEdit::blockCountChanged,     this, &QmlEdit::updateLineNumberAreaWidth);
      connect(this, &QPlainTextEdit::updateRequest,         this, &QmlEdit::updateLineNumberArea);
      connect(this, &QPlainTextEdit::cursorPositionChanged, this, &QmlEdit::highlightCurrentLine);

      updateLineNumberAreaWidth();
      highlightCurrentLine();

      m_originalFontSize = fontSize();

      contextMenu = new QMenu(this);

      QMenu *menuEdit = new QMenu(tr("Edit"));
      menuEdit->setToolTip(tr("Edit operations"));
      contextMenu->addMenu(menuEdit);

      QAction *duplicateLineAct = new QAction(tr("Duplicate line"));
      duplicateLineAct->setToolTip(tr("Duplicate current line(s)."));
      duplicateLineAct->setIcon(*icons[int(Icons::paste_ICON)]);
      menuEdit->addAction(duplicateLineAct);
      menuEdit->addSeparator();

      QAction *moveLineDownAct = new QAction(tr("Move selection up"));
      moveLineDownAct->setToolTip(tr("Move selection one line down"));
      moveLineDownAct->setIcon(*icons[int(Icons::arrowDown_ICON)]);
      menuEdit->addAction(moveLineDownAct);

      QAction *moveLineUpAct = new QAction(tr("Move selection up"));
      moveLineUpAct->setToolTip(tr("Move selection one line up"));
      moveLineUpAct->setIcon(*icons[int(Icons::arrowUp_ICON)]);
      menuEdit->addAction(moveLineUpAct);

      QMenu *menuFontSize = new QMenu(tr("Font size"));
      menuFontSize->setToolTip(tr("Change the characters' size"));
      contextMenu->addMenu(menuFontSize);

      QAction *increaseFontSizeAct = new QAction(tr("Increase font size"));
      increaseFontSizeAct->setToolTip(tr("Increase font point size\nCurrent: ") + fontSize() + tr("pts", "as in a font size."));
      increaseFontSizeAct->setIcon(*icons[int(Icons::plus_ICON)]);
      menuFontSize->addAction(increaseFontSizeAct);

      QAction *decreaseFontSizeAct = new QAction(tr("Decrease font size"));
      decreaseFontSizeAct->setToolTip(tr("Decrease font point size\nCurrent: ") + fontSize() + tr("pts", "as in a font size."));
      decreaseFontSizeAct->setIcon(*icons[int(Icons::minus_ICON)]);
      menuFontSize->addAction(decreaseFontSizeAct);

      QAction *resetFontSizeAct = new QAction(tr("Reset font size"));
      resetFontSizeAct->setToolTip(tr("Reset font point size to: ") + m_originalFontSize + tr("pts", "as in a font size."));
      resetFontSizeAct->setIcon(*icons[int(Icons::equal_ICON)]);
      menuFontSize->addAction(resetFontSizeAct);

      for (QAction *act: contextMenu->actions())
            act->setIconVisibleInMenu(true);

      connect(duplicateLineAct,    &QAction::triggered, this, &QmlEdit::duplicateLine);
      connect(moveLineUpAct,       &QAction::triggered, this, &QmlEdit::moveLineUp);
      connect(moveLineDownAct,     &QAction::triggered, this, &QmlEdit::moveLineDown);
      connect(increaseFontSizeAct, &QAction::triggered, this, &QmlEdit::increaseFontSize);
      connect(decreaseFontSizeAct, &QAction::triggered, this, &QmlEdit::decreaseFontSize);
      connect(resetFontSizeAct,    &QAction::triggered, this, &QmlEdit::resetFontSize);
      }

/*=========================================================
      //---------------------------------------------------------
      //   focusInEvent
      //---------------------------------------------------------
      void QmlEdit::focusInEvent(QFocusEvent* event)
            {
      //      mscoreState = mscore->state();
      //      mscore->changeState(STATE_DISABLED);
            QPlainTextEdit::focusInEvent(event);
            }

      //---------------------------------------------------------
      //   focusOutEvent
      //---------------------------------------------------------
      void QmlEdit::focusOutEvent(QFocusEvent* event)
            {
      //      mscore->changeState(mscoreState);
            QPlainTextEdit::focusOutEvent(event);
            }
=========================================================*/

//---------------------------------------------------------
//   move
//---------------------------------------------------------

void QmlEdit::move(QTextCursor::MoveOperation op)
      {
      QTextCursor tc(textCursor());
      tc.movePosition(op);
      setTextCursor(tc);
      update();
      }

//---------------------------------------------------------
//   moveToPos
//---------------------------------------------------------

void QmlEdit::moveToPos(int pos)
      {
      QTextCursor tc(textCursor());
      tc.setPosition(pos);
      setTextCursor(tc);
      update();
      }

//---------------------------------------------------------
//   duplicateLine
//     if there is a selection over multiple lines, this function copies
//     all the lines, like in QtCreator.
//---------------------------------------------------------

void QmlEdit::duplicateLine()
      {
      QTextCursor tc(textCursor());
      tc.beginEditBlock();

      //  because it may be negative, not selectedText().length().
      int selectionLength = tc.position() - tc.anchor();

      if (!selectionLength) { // i.e. if there is no selection
            // save cursor position.
            int cursBLockPos = document()->findBlock(tc.position()).position();
            int cursPosInBLock = tc.positionInBlock();

            // select line under cursor.
            tc.setPosition(cursBLockPos);
            tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

            // copy
            QString copy =  "\n" + tc.selectedText();

            tc.insertBlock();
            // unselect
            tc.clearSelection();

            // paste
            tc.insertText(copy);

            // reset position.
            tc.setPosition(tc.block().position() + cursPosInBLock);
            }
      else { // i.e. if there is a selection.
             // this also covers if the selection is over multiple lines.

            // unselect the terminating char.
            if (tc.atEnd()) {
                  tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
                 }
            else if (tc.anchor() >= toPlainText().length()) { // the anchor is at the end. (or further???)
                  tc.movePosition(QTextCursor::End);
                  tc.movePosition(QTextCursor::Left);
                  tc.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
                  }

            // save all positions.
            int anchBlockPos = document()->findBlock(tc.anchor()).position();
            int anchPosInBlock = tc.anchor() - anchBlockPos;
            int cursBlockPos = document()->findBlock(tc.position()).position();
            int anchPosFromCursBlock = tc.anchor() - cursBlockPos;

            // select line(s) under selection
            if (selectionLength >= 0) {
                  tc.setPosition(anchBlockPos); // also unselects
                  tc.setPosition(cursBlockPos, QTextCursor::KeepAnchor);
                  tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                  }
            else {
                  tc.setPosition(cursBlockPos); // also unselects
                  tc.setPosition(anchBlockPos, QTextCursor::KeepAnchor);
                  tc.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                  }

            // copy
            QString copy = tc.selectedText() + "\n";

            // move to place to paste.
            tc.movePosition(QTextCursor::NextBlock);

            // save new block pos.
            int newBlockPos = tc.position();

            // paste
            tc.insertText(copy);

            // reset positions.
            if (selectionLength >= 0) {
                  tc.setPosition(newBlockPos + anchPosInBlock);
                  tc.setPosition(newBlockPos + anchPosInBlock + selectionLength, QTextCursor::KeepAnchor);
                  }
            else {
                  tc.setPosition(newBlockPos + anchPosFromCursBlock + selectionLength);
                  tc.setPosition(newBlockPos + anchPosFromCursBlock, QTextCursor::KeepAnchor);
                  }
            }

      tc.endEditBlock();
      setTextCursor(tc);
      }

//---------------------------------------------------------
//   moveLine
//    if there is a selection over multiple lines, this function moves
//     all the lines, like in QtCreator.
//---------------------------------------------------------

void QmlEdit::moveLine(bool up) //if it is not up, it is down.
      {
      QTextCursor tc(textCursor());
      tc.beginEditBlock();

      int selectionLength = tc.selectedText().length();

      if (!selectionLength) { // i.e. if there is no selection
            // save cursor position.
            int cursBLockPos = document()->findBlock(tc.position()).position();
            int cursPosInBLock = tc.positionInBlock();

            // select line under cursor.
            tc.setPosition(cursBLockPos);
            tc.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);

            // copy
            QString copy = tc.selectedText();

            // remove text

            // move to place to paste.
            if (up)
                  tc.movePosition(QTextCursor::Up);
            else
                  tc.movePosition(QTextCursor::Down);

            // paste
            tc.insertText(copy);

            // reset position.
            tc.setPosition(tc.block().position() + cursPosInBLock);
            }
      else { // i.e. if there is a selection.
            // this also covers if the selection is over multiple lines.

           // unselect the terminating char.
           if (tc.atEnd()) {
                 tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
                }
           else if (tc.anchor() >= toPlainText().length()) { // the anchor is at the end. (or further???)
                 tc.movePosition(QTextCursor::End);
                 tc.movePosition(QTextCursor::Left);
                 tc.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);
                 }

           // save all positions.
           int anchBlockPos = document()->findBlock(tc.anchor()).position();
           int anchPosInBlock = tc.anchor() - anchBlockPos;
           int cursBlockPos = document()->findBlock(tc.position()).position();
           int anchPosFromCursBlock = tc.anchor() - cursBlockPos;

           // select line(s) under selection
           if (selectionLength > 0) {
                 tc.setPosition(anchBlockPos); // also unselects
                 tc.setPosition(cursBlockPos, QTextCursor::KeepAnchor);
                 }
           else {
                 tc.setPosition(cursBlockPos); // also unselects
                 tc.setPosition(anchBlockPos, QTextCursor::KeepAnchor);
                 }
           tc.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);

           // cut
           QString copy = tc.selectedText();
           tc.removeSelectedText();

           // move to place to paste.
           if (up)
                 tc.movePosition(QTextCursor::Up);
           else
                 tc.movePosition(QTextCursor::Down);

           // save new block pos.
           int newBlockPos = tc.position();

           // paste
           tc.insertText(copy);

           // reset positions.
           if (selectionLength > 0) {
                 tc.setPosition(newBlockPos + anchPosInBlock);
                 tc.setPosition(newBlockPos + anchPosInBlock + selectionLength, QTextCursor::KeepAnchor);
                 }
           else {
                 tc.setPosition(newBlockPos + anchPosFromCursBlock + selectionLength);
                 tc.setPosition(newBlockPos + anchPosFromCursBlock, QTextCursor::KeepAnchor);
                 }
            }

      tc.endEditBlock();
      setTextCursor(tc);
      }

//---------------------------------------------------------
//   QmlEdit
//---------------------------------------------------------

QmlEdit::~QmlEdit()
      {
      delete hl;
      }

//---------------------------------------------------------
//   lineNumberAreaWidth
//---------------------------------------------------------

int QmlEdit::lineNumberAreaWidth() const
      {
      int digits = 1;
      int max = qMax(1, blockCount());
      while (max >= 10) {
            max /= 10;
            ++digits;
            }
      int space = 6 + fontMetrics().width(QLatin1Char('9')) * digits;
      return space;
}

//---------------------------------------------------------
//   fontSize
//---------------------------------------------------------

int QmlEdit::fontSize() const
      {
      return fontInfo().pointSize();
      }

//---------------------------------------------------------
//   setFontSize
//---------------------------------------------------------

void QmlEdit::setFontSize(int pointSize)
      {
      QFont font = QPlainTextEdit::font();
      font.setPointSize(pointSize);
      QPlainTextEdit::setFont(font);

      QTextCursor cursor = textCursor();
      QTextCharFormat format = cursor.charFormat();
      format.setFont(font);
      cursor.setCharFormat(format);
      this->currentCharFormat();
      setTextCursor(cursor);

      QPlainTextEdit::document()->setDefaultFont(font);
      }

//---------------------------------------------------------
//   updateLineNumberAreaWidth
//---------------------------------------------------------

void QmlEdit::updateLineNumberAreaWidth()
      {
      setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
      }

//---------------------------------------------------------
//   updateLineNumberArea
//---------------------------------------------------------

void QmlEdit::updateLineNumberArea(const QRect& rect, int dy)
      {
      if (dy)
            lineNumberArea->scroll(0, dy);
      else
            lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

      if (rect.contains(viewport()->rect()))
            updateLineNumberAreaWidth();
      }

//---------------------------------------------------------
//   resizeEvent
//---------------------------------------------------------

void QmlEdit::resizeEvent(QResizeEvent *e)
      {
      QPlainTextEdit::resizeEvent(e);

      QRect cr = contentsRect();
      lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
      }

//---------------------------------------------------------
//   contextMenuEvent
//---------------------------------------------------------

#ifndef QT_NO_CONTEXTMENU
void QmlEdit::contextMenuEvent(QContextMenuEvent* event)
      {
      contextMenu->exec(event->globalPos());
      event->accept();
      }
#endif // QT_NO_CONTEXTMENU

//---------------------------------------------------------
//   highlightCurrentLine
//---------------------------------------------------------

void QmlEdit::highlightCurrentLine()
      {
      QList<QTextEdit::ExtraSelection> extraSelections;

      if (!isReadOnly()) {
            QTextEdit::ExtraSelection selection;
            selection.format.setBackground(preferences.globalStyle() == MuseScoreStyleType::DARK_FUSION
                                           ? QColor("#202020") : Qt::white);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            extraSelections.append(selection);
            }
      setExtraSelections(extraSelections);
}

//---------------------------------------------------------
//   highlightMatchingBraces
//     This was abandonned because it would be too complicated to handle braces in comments,
//     as well as in strings.
//---------------------------------------------------------

//void QmlEdit::highlightMatchingBraces()
//{
//      if (isReadOnly())
//            return;

//      int pos = textCursor().position();
//      QString text = toPlainText();
//      QChar cAtPosLeft = text.at(pos);
//      QChar cAtPosRight = text.at(pos + 1);
//      bool left = ((cAtPosLeft == '{') || (cAtPosLeft == '[')
//                    || (cAtPosLeft == '}') || (cAtPosLeft == ']'));
//      bool right = (((cAtPosRight + 1) == '{') || ((cAtPosRight + 1) == '[')
//                   || ((cAtPosRight + 1) == '}') || ((cAtPosRight + 1) == ']'));

//      // if not neither the right of the cursor and the left are on something that is
//      // the beginning of a block.
//      if (!(right || left))
//            return;

//      QList<QTextEdit::ExtraSelection> extraSelections;
//      QTextEdit::ExtraSelection selection;
//      selection.format.setBackground(QColor("#11AAFF"));
//      selection.cursor = textCursor();
//      selection.cursor.clearSelection();

//      if (right) {
//            selection.cursor.movePosition(QTextCursor::Right);
//            selection.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
//            extraSelections.append(selection);
//            selection.cursor = textCursor();
//            }

//      if (left) {
//            selection.cursor.movePosition(QTextCursor::Left);
//            selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
//            extraSelections.append(selection);
//            selection.cursor = textCursor();
//            }

//      if((cAtPosRight == '{') || (cAtPosRight == '[')) {
//            int count = 1; // if count gets to zero, this is the matching brace.
//            int i = 0;
//            for (i = 0; (i + pos < text.length()) || !count; i++) {
//                  QChar c = text.at(i + pos);
//                  if (c == '{')
//                        count++;
//                  else if (c == '}')
//                        count--;
//                  }
//            if (!count) { // found the end of that block
//                  selection.cursor.setPosition(pos + i);
//                  selection.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
//                  extraSelections.append(selection);
//                  selection.cursor = textCursor();
//                  }
//            }
//      extraSelections.append(selection);

//      setExtraSelections(extraSelections);
//}

//---------------------------------------------------------
//   lineNumberAreaPaintEvent
//---------------------------------------------------------

void QmlEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
      {
      QPainter painter((QWidget *)lineNumberArea);
      painter.fillRect(event->rect(), (preferences.globalStyle() == MuseScoreStyleType::DARK_FUSION)
                       ? QColor("#303030") : Qt::lightGray);

      QFont font("FreeMono", fontSize());
      font.setStyleHint(QFont::TypeWriter);
      font.setFixedPitch(true);
      painter.setFont(font);
      painter.setPen((preferences.globalStyle() == MuseScoreStyleType::DARK_FUSION)
                     ? Qt::lightGray : Qt::black);

      QTextBlock block = firstVisibleBlock();
      int blockNumber = block.blockNumber();
      int tcBlockNumber = textCursor().blockNumber();
      int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
      int bottom = top + (int) blockBoundingRect(block).height();

      int width = lineNumberArea->width();
      int height = fontMetrics().height();
      while (block.isValid() && top <= event->rect().bottom()) {
            ++blockNumber;
            if (block.isVisible() && bottom >= event->rect().top()) {
                  if (blockNumber == tcBlockNumber) {
                        font.setBold(true);
                        painter.drawText(0, top, width - 3, height,
                                         Qt::AlignRight, QString::number(blockNumber));
                        font.setBold(false);
                        }
                  else {
                        painter.drawText(0, top, width - 3, height,
                                         Qt::AlignRight, QString::number(blockNumber));
                        }
                  }
            block = block.next();
            top = bottom;
            bottom = top + (int) blockBoundingRect(block).height();
            }
      event->accept();
      } // fn lineNumberAreaPaintEvent()

#ifdef QMLEDIT_BINDINGS // uncomment at top of qmledit.h.
//---------------------------------------------------------
//   pick
//---------------------------------------------------------
void QmlEdit::pick()
      {
      pickBuffer = textCursor().block().text();
      }

//---------------------------------------------------------
//   put
//---------------------------------------------------------

void QmlEdit::put()
      {
      QTextCursor c = textCursor();
      int column = c.columnNumber();
      c.movePosition(QTextCursor::StartOfBlock);
      c.insertText(pickBuffer + "\n");
      c.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
      c.movePosition(QTextCursor::Up);
      setTextCursor(c);
      }

//---------------------------------------------------------
//   delLine
//---------------------------------------------------------

void QmlEdit::delLine()
      {
      QTextCursor c = textCursor();
      c.select(QTextCursor::BlockUnderCursor);
      pickBuffer = c.selectedText().mid(1);
      c.removeSelectedText();
      c.movePosition(QTextCursor::Down);
      setTextCursor(c);
      }

//---------------------------------------------------------
//   delWord
//---------------------------------------------------------

void QmlEdit::delWord()
      {
      QTextCursor c = textCursor();
      int i = c.position();
      if (document()->characterAt(i) == QChar(' ')) {
            while(document()->characterAt(i) == QChar(' '))
                  c.deleteChar();
            }
      else {
            for (;;) {
                  QChar ch = document()->characterAt(i);
                  if (ch == QChar(' ') || ch == QChar('\n'))
                        break;
                  c.deleteChar();
                  }
            while(document()->characterAt(i) == QChar(' '))
                  c.deleteChar();
            }
      }

//---------------------------------------------------------
//   downLine
//---------------------------------------------------------

void QmlEdit::downLine()
      {
      qDebug("Down line");
      move(QTextCursor::Down);
      }

//---------------------------------------------------------
//   leftWord
//---------------------------------------------------------

void QmlEdit::leftWord()
      {
      QTextCursor c = textCursor();

      if (c.positionInBlock() == 0)
            return;
      c.movePosition(QTextCursor::Left);

      bool inSpace = true;
      for (;c.positionInBlock();) {
            int i = c.position();
            if (document()->characterAt(i) == QChar(' ')) {
                  if (!inSpace) {
                        c.movePosition(QTextCursor::Right);
                        break;
                        }
                  }
            else {
                  if (inSpace)
                        inSpace = false;
                  }
            c.movePosition(QTextCursor::Left);
            }
      setTextCursor(c);
      }
#endif //QMLEDIT_BINDINGS

//---------------------------------------------------------
//   keyPressEvent
//---------------------------------------------------------

void QmlEdit::keyPressEvent(QKeyEvent* event)
      {
      if (event->modifiers() != Qt::ControlModifier && event->key() == Qt::Key_Tab) {
            tab();
            event->accept();
            return;
            }
      if (event->modifiers() != Qt::ControlModifier &&
          (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)) {
            autoIndent();
            event->accept();
            return;
            }
      QPlainTextEdit::keyPressEvent(event);
      }

//---------------------------------------------------------
//   autoindent - line new line up with start of previous.
//---------------------------------------------------------

void QmlEdit::autoIndent()
      {
      QTextCursor c = textCursor();
      QTextBlock b = c.block();
      QString line = "";
      while (line.trimmed() == "" && b.isValid()) // Find last non-blank line to line up on.
            {
            line = b.text();
            b = b.previous();
            }
      int indent = 0;
      c.insertText("\n");
      while (line[indent] == " ") {
            indent += 1;
            c.insertText(" ");
      }
}

//---------------------------------------------------------
//   increaseFontSize
//---------------------------------------------------------

void QmlEdit::increaseFontSize()
      {
      int ps = fontSize();
      if (ps >= 64)
            return;

      if (ps < 2) {
            setFontSize(2);
            return;
            }

      int i = 0;
      for (i = 0; i <= 100; i++) {
            if (FontSizes[i] >= ps)
                  break;
            }
      setFontSize(FontSizes[i + 1]);
}

//---------------------------------------------------------
//   decreaseFontSize
//---------------------------------------------------------

void QmlEdit::resetFontSize()
      {
      setFontSize(m_originalFontSize);
      }

//---------------------------------------------------------
//   decreaseFontSize
//---------------------------------------------------------

void QmlEdit::decreaseFontSize()
      {
      int ps = fontSize();
      if (ps <= 2)
            return;

      if (ps > 64) {
            setFontSize(64);
            return;
            }

      int i = 0;
      for (i = 0; i <= 100; i++) {
            if (FontSizes[i] >= ps)
                  break;
            }

      setFontSize(FontSizes[i - 1]);
      }

//---------------------------------------------------------
//   tab
//---------------------------------------------------------

void QmlEdit::tab()
      {
      QTextCursor c = textCursor();
      c.insertText(" ");
      while (c.positionInBlock() % 6)
            c.insertText(" ");
      setTextCursor(c);
      }
}

