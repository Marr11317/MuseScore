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


#include "pluginCreator.h"
#include "ui_aboutPlugins.h"
#include "../preferences.h"
#include "../musescore.h"
#include "../qmlplugin.h"
#include "../icons.h"
#include "../helpBrowser.h"
#include "../libmscore/score.h"

#include <QWidget>
#include <QTime>
#include <QKeySequence>
#include <QSettings>
#include <QFileInfo>
#include <QFile>
#include <QMessageBox>
#include <QtMessageHandler>
#include <QList>

// WARNING: for all the log output, the text is not translated, as the QQmlEngine
//          sends the errors to the log in english anyways.

namespace Ms {

extern bool useFactorySettings;

//---------------------------------------------------------
//   PluginCreator
//---------------------------------------------------------

PluginCreator::PluginCreator(QWidget* parent)
   : QMainWindow(parent)
      {
      m_state       = PCState::INIT;
      m_item        = nullptr;
      m_view        = nullptr;
      m_dock        = nullptr;
      m_manualDock  = nullptr;
      m_helpBrowser = nullptr;
      m_path        = "";
      m_fileName    = "";
      m_aboutPluginCreator = nullptr;
      m_blockTextChanged   = false;

      if (objectName().isEmpty())
            setObjectName("PluginCreator");
      setIconSize(QSize(Ms::preferences.getInt(PREF_UI_THEME_ICONWIDTH) * Ms::guiScaling,
                        Ms::preferences.getInt(PREF_UI_THEME_ICONHEIGHT) * Ms::guiScaling));

      setupUi(this);

      QToolBar *fileTools = addToolBar(tr("File Operations"));
      fileTools->setObjectName("FileOperations");

      // NOTE: For the shortcuts of actionSaveAs, actionQuit and actionClose,
      //       the standard QKeySequence is not defined under Windows.
      //       See QKeySequence's doc at these entries for more details.
      actionNew->setIcon(*icons[int(Icons::fileNew_ICON)]);
      actionNew->setShortcut(QKeySequence(QKeySequence::New));
      connect(actionNew, &QAction::triggered, this, &PluginCreator::newPlugin);
      fileTools->addAction(actionNew);
      actionOpen->setIcon(*icons[int(Icons::fileOpen_ICON)]);
      actionOpen->setShortcut(QKeySequence(QKeySequence::Open));
      connect(actionOpen, &QAction::triggered, this, &PluginCreator::loadPlugin);
      fileTools->addAction(actionOpen);
      actionReload->setIcon(*icons[int(Icons::viewRefresh_ICON)]);
      actionReload->setShortcut(QKeySequence(QKeySequence::Refresh));
      connect(actionReload, &QAction::triggered, this, &PluginCreator::reload);
      fileTools->addAction(actionReload);
      actionSave->setIcon(*icons[int(Icons::fileSave_ICON)]);
      actionSave->setShortcut(QKeySequence(QKeySequence::Save));
      connect(actionSave, &QAction::triggered, this, &PluginCreator::savePlugin);
      fileTools->addAction(actionSave);
      actionSaveAs->setIcon(*icons[int(Icons::fileSaveAs_ICON)]);
      actionSaveAs->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
      actionSaveAs->setShortcutContext(Qt::WindowShortcut);
      connect(actionSaveAs, &QAction::triggered, this, &PluginCreator::savePluginAs);
      fileTools->addAction(actionSaveAs);
      actionManual->setIcon(*icons[int(Icons::helpContents_ICON)]);
      actionManual->setShortcut(QKeySequence(QKeySequence::HelpContents));
      connect(actionManual, &QAction::triggered, this, &PluginCreator::showManual);
      fileTools->addAction(actionManual);
      actionClose->setIcon(*icons[int(Icons::fileClose_ICON)]);
      actionClose->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F4));
      actionClose->setShortcutContext(Qt::WindowShortcut);
      connect(actionClose, &QAction::triggered, this, &PluginCreator::closeCurrentPluginSource);
      actionQuit->setIcon(*icons[int(Icons::close_ICON)]);
      actionQuit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
      actionQuit->setShortcutContext(Qt::WindowShortcut);
      connect(actionQuit, &QAction::triggered, this, &QWidget::close);

      QToolBar* editTools = addToolBar(tr("Edit Operations"));
      editTools->setObjectName("EditOperations");
      actionUndo->setIcon(*icons[int(Icons::undo_ICON)]);
      actionUndo->setShortcut(QKeySequence(QKeySequence::Undo));
      actionRedo->setIcon(*icons[int(Icons::redo_ICON)]);
      actionRedo->setShortcut(QKeySequence(QKeySequence::Redo));
      actionDuplicateLine->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_Down));
      actionDuplicateLine->setShortcutContext(Qt::WindowShortcut);
      connect(actionDuplicateLine, &QAction::triggered, textEdit, &QmlEdit::duplicateLine);
      actionMoveLineUp->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Up));
      actionMoveLineUp->setShortcutContext(Qt::WindowShortcut);
      connect(actionMoveLineUp, &QAction::triggered, textEdit, &QmlEdit::moveLineUp);
      actionMoveLineDown->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Down));
      actionMoveLineDown->setShortcutContext(Qt::WindowShortcut);
      connect(actionMoveLineDown, &QAction::triggered, textEdit, &QmlEdit::moveLineDown);

      if (qApp->layoutDirection() == Qt::LayoutDirection::LeftToRight) {
            editTools->addAction(actionUndo);
            editTools->addAction(actionRedo);
            }
      else {
            editTools->addAction(actionRedo);
            editTools->addAction(actionUndo);
            }
      actionUndo->setEnabled(false);
      actionRedo->setEnabled(false);
      connect(actionUndo, &QAction::triggered, textEdit, &QPlainTextEdit::undo);
      connect(actionRedo, &QAction::triggered, textEdit, &QPlainTextEdit::redo);
      
      QToolBar* pluginTools = addToolBar(tr("Plugin operations"));
      pluginTools->setObjectName("PluginOperations");
      pluginTools->setToolButtonStyle(Qt::ToolButtonTextOnly);
      
      actionRun->setShortcut(QKeySequence(Qt::Key_F9));
      actionRun->setShortcutContext(Qt::WindowShortcut);
      actionRun->setIcon(*icons[int(Icons::play_ICON)]);
      pluginTools->addAction(actionRun);
      connect(actionRun, &QAction::triggered, this, &PluginCreator::runClicked);
      actionStop->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F9));
      actionStop->setShortcutContext(Qt::WindowShortcut);
      actionStop->setIcon(*icons[int(Icons::stop_ICON)]);
      pluginTools->addAction(actionStop);
      connect(actionStop, &QAction::triggered, this, &PluginCreator::stopClicked);

      actionClearLog->setIcon(*icons[int(Icons::reset_ICON)]);
      connect(actionClearLog, &QAction::triggered, log, &QPlainTextEdit::clear);
      actionIncreaseFontSize->setIcon(*icons[int(Icons::plus_ICON)]);
      actionIncreaseFontSize->setShortcut(QKeySequence(QKeySequence::ZoomIn));
      connect(actionIncreaseFontSize, &QAction::triggered, log, &Ms::Console::increaseFontSize);
      connect(actionIncreaseFontSize, &QAction::triggered, textEdit, &Ms::QmlEdit::increaseFontSize);
      actionDecreaseFontSize->setIcon(*icons[int(Icons::minus_ICON)]);
      actionIncreaseFontSize->setShortcut(QKeySequence(QKeySequence::ZoomOut));
      connect(actionDecreaseFontSize, &QAction::triggered, log, &Ms::Console::decreaseFontSize);
      connect(actionDecreaseFontSize, &QAction::triggered, textEdit, &Ms::QmlEdit::decreaseFontSize);
      actionResetFontSize->setIcon(*icons[int(Icons::equal_ICON)]);
      connect(actionResetFontSize, &QAction::triggered, log, &Ms::Console::resetFontSize);
      connect(actionResetFontSize, &QAction::triggered, textEdit, &Ms::QmlEdit::resetFontSize);

      m_showLogButton = new QToolButton;
      m_showLogButton->setToolTip(tr("Toggle log's visibility"));
      m_showLogButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
      statusbar->addPermanentWidget(m_showLogButton);
      connect(logDock, &QDockWidget::visibilityChanged, this, &PluginCreator::setLogVisibilityButtons);
      connect(m_showLogButton, &QPushButton::clicked, this, &PluginCreator::toggleLogVisible);
      connect(actionShowLog, &QAction::triggered, this, &PluginCreator::toggleLogVisible);
      // Initialize the log's visibility buttons.
      setLogVisibilityButtons();

      actionAboutPluginCreator->setIcon(*icons[int(Icons::plugins_ICON)]);
      connect(actionAboutPluginCreator, &QAction::triggered, this, &PluginCreator::showAboutPluginCreator);
      buttonAboutPluginCreator->setIcon(*icons[int(Icons::helpContents_ICON)]);
      connect(buttonAboutPluginCreator, &QAbstractButton::clicked, this, &PluginCreator::showAboutPluginCreator);
      buttonNew->setIcon(*icons[int(Icons::fileNew_ICON)]);
      connect(buttonNew, &QAbstractButton::clicked, this, &PluginCreator::newPlugin);
      buttonLoad->setIcon(*icons[int(Icons::fileOpen_ICON)]);
      connect(buttonLoad, &QAbstractButton::clicked, this, &PluginCreator::loadPlugin);

      connect(textEdit, &QPlainTextEdit::undoAvailable, actionUndo, &QAction::setEnabled);
      connect(textEdit, &QPlainTextEdit::redoAvailable, actionRedo, &QAction::setEnabled);
      connect(textEdit, &QPlainTextEdit::textChanged, this, &PluginCreator::textChanged);
      connect(stackedWidget, &QStackedWidget::currentChanged, this, &PluginCreator::pageChanged);

      readSettings();
      if (m_path.isEmpty()) {
            setState(PCState::EMPTY);
            stackedWidget->setCurrentIndex(1);
            logDock->setVisible(false);
            }
      else {
            QFile f(m_path);
            QFileInfo fi(f);
            if (fi.isFile()) {
                  load();
                  //The state is set directly in the load function.
                  //Therefore, it doesn't need to be set here.
                  QSettings settings;
                  settings.beginGroup(objectName());
                  QTextCursor cursor(textEdit->textCursor());
                  cursor.setPosition(settings.value("textCursorPos").toInt());
                  textEdit->setTextCursor(cursor);
                  settings.endGroup();
                  }
            else {
                  setState(PCState::EMPTY);
                  stackedWidget->setCurrentIndex(1);
                  logDock->setVisible(false);
                  }
            }
      }

//---------------------------------------------------------
//   Destructor
//---------------------------------------------------------

PluginCreator::~PluginCreator()
      {
      if (m_manualDock)
            delete m_manualDock;
      }


//---------------------------------------------------------
//   manualPath
//---------------------------------------------------------

QString PluginCreator::manualPath() const
      {
      QString manualpath = mscoreGlobalShare;
      manualpath += "/manual/plugins/plugins.html";
      return manualpath;
      }

//---------------------------------------------------------
//   setState
//   NOTE: The state can be set by different threads.
//         (aka when the textChanged signal is emitted).
//         So be cautious with what you put in it.
//---------------------------------------------------------

QDialog *PluginCreator::aboutPluginCreator() const
{
      return m_aboutPluginCreator;
}

void PluginCreator::setState(PCState newState)
{
      if (m_state == newState)
            return;
      switch(m_state) {
            case PCState::INIT:
                  switch(newState) {
                        case PCState::EMPTY:
                              actionSave->setEnabled(false);
                              actionSaveAs->setEnabled(false);
                              actionRun->setEnabled(false);
                              actionStop->setEnabled(false);
                              actionClose->setEnabled(false);
                              actionRedo->setEnabled(false);
                              actionUndo->setEnabled(false);
                              actionReload->setEnabled(false);
                              break;
                        case PCState::CLEAN:
                              actionSave->setEnabled(false);
                              actionSaveAs->setEnabled(true);
                              actionRun->setEnabled(true);
                              actionStop->setEnabled(false);
                              actionClose->setEnabled(true);
                              actionReload->setEnabled(true);
                              break;
                        case PCState::DIRTY:
                              //This happens because on loading a plugin, the textChanged() signal is emitted,
                              //so the state is set (but not always) to DIRTY instead of CLEAN.
                              //We MUST return; here (and not break;), to prevent the state from being changed.
                              //To prevent this behaviour, set m_blockTextChanged to true before calling textEdit->setPlainText();
                              return;
                        case PCState::INIT:
                              // return because the state doesn't need to be changed. (m_state == newstate)
                              // this should be avoided by the if at the beginning of this function, but in case...
                              return;
                        }
                  break;
            case PCState::EMPTY:
                  switch(newState) {
                        case PCState::CLEAN:
                              actionSave->setEnabled(false);
                              actionSaveAs->setEnabled(true);
                              actionRun->setEnabled(true);
                              actionStop->setEnabled(false);
                              actionClose->setEnabled(true);
                              actionReload->setEnabled(true);
                              break;
                        case PCState::INIT:
                              break;
                        case PCState::DIRTY:
                              //This happens because on loading a plugin, the textChanged() signal is emitted,
                              //so the state is set (but not always) to DIRTY instead of CLEAN.
                              //We MUST return; here (and not break;), to prevent the state from being changed.
                              //To prevent this behaviour, set m_blockTextChanged to true before calling textEdit->setPlainText();
                              return;
                        case PCState::EMPTY:
                              // return because the state doesn't need to be changed. (m_state == newstate)
                              // this should be avoided by the if at the beginning of this function, but in case...
                              return;
                        }
                  break;
            case PCState::CLEAN:
                  switch(newState) {
                        case PCState::DIRTY:
                              actionSave->setEnabled(true);
                              break;
                        case PCState::EMPTY:
                              actionSave->setEnabled(false);
                              actionSaveAs->setEnabled(false);
                              actionRun->setEnabled(false);
                              actionStop->setEnabled(false);
                              actionClose->setEnabled(false);
                              actionRedo->setEnabled(false);
                              actionUndo->setEnabled(false);
                              actionReload->setEnabled(false);
                              break;
                        case PCState::CLEAN:
                              actionSave->setEnabled(false);
                              return;
                        case PCState::INIT:
                              break;
                        }
                  break;
            case PCState::DIRTY:
                  switch(newState) {
                        case PCState::CLEAN:
                              actionSave->setEnabled(false);
                              break;
                        case PCState::EMPTY:
                              actionSave->setEnabled(false);
                              actionSaveAs->setEnabled(false);
                              actionRun->setEnabled(false);
                              actionStop->setEnabled(false);
                              actionClose->setEnabled(false);
                              actionRedo->setEnabled(false);
                              actionUndo->setEnabled(false);
                              actionReload->setEnabled(false);
                              break;
                        case PCState::INIT:
                              break;
                        case PCState::DIRTY:
                              // return because the state doesn't need to be changed. (m_state == newstate)
                              // this should be avoided by the if at the beginning of this function, but in case...
                              return;
                        }
                  break;
            }
      setTitle(m_fileName);
      m_state = newState;
      }

//---------------------------------------------------------
//   setTitle
//---------------------------------------------------------

void PluginCreator::setTitle(const QString& s)
      {
      QString t(tr("MuseScore Plugin Creator"));
      if (s.isEmpty())
            setWindowTitle(t);
      else {
            setWindowTitle(t + tr(" - ", "Does this need to change in arabic and cyrillic?") + s);
            if (m_state == PCState::DIRTY)
                  setWindowTitle(windowTitle() + tr("*", "Does this need to change in arabic and cyrillic?"));
            }
      }

//---------------------------------------------------------
//   writeSettings
//---------------------------------------------------------

void PluginCreator::writeSettings()
      {
      QSettings settings;
      settings.beginGroup(objectName());
      settings.setValue("windowState", saveState());
      settings.setValue("path", m_path);
      settings.setValue("textCursorPos", (stackedWidget->currentIndex() == 0) ? textEdit->textCursor().position() : 0);
      settings.setValue("logFontSize",  log->fontSize());
      settings.setValue("textFontSize", textEdit->fontSize());
      settings.endGroup();

      MuseScore::saveGeometry(this);
      }

//---------------------------------------------------------
//   readSettings
//---------------------------------------------------------

void PluginCreator::readSettings()
      {
      if (!useFactorySettings) {
            QSettings settings;
            settings.beginGroup(objectName());
            restoreState(settings.value("windowState").toByteArray());
            setPath(settings.value("path", "").toString());
            //settings for textCursorPos are loaded in the constructor.
            log->setFontSize(settings.value("logFontSize").toInt());
            textEdit->setFontSize(settings.value("textFontSize").toInt());
            settings.endGroup();
            }

      MuseScore::restoreGeometry(this);
      }

//---------------------------------------------------------
//   closeEvent
//---------------------------------------------------------

void PluginCreator::closeEvent(QCloseEvent* ev)
      {
      if (m_state == PCState::DIRTY) {
            QMessageBox::StandardButton n = QMessageBox::warning(this, tr("MuseScore"),
                  tr("Plugin \"%1\" has unsaved changes.\n"
                  "Save before closing?").arg(m_fileName),
                  QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                  QMessageBox::Save);
            if (n == QMessageBox::Save)
                  savePlugin();
            else if (n == QMessageBox::Cancel) {
                  ev->ignore();
                  return;
                  }
            }
      hide();
      writeSettings();
      // if you uncomment this line, action run needs to be disabled,
      // or it will appear as enabled even if it should not, when reopening the plugin Creator.
      //#closePlugin();
      //#actionRun->setEnabled(false);
      QWidget::closeEvent(ev);
      }

//---------------------------------------------------------
//   qmlMsgHandler
//---------------------------------------------------------

static void qmlMsgHandler(QtMsgType type, const QMessageLogContext &, const QString & msge)
      {
      if (!mscore->pluginCreator())
            return;

      Ms::Console& log = *mscore->pluginCreator()->log;
      QTextCursor c = log.textCursor();
      QTextCursor copy(c);
      copy.beginEditBlock();
      copy.movePosition(QTextCursor::NoMove); // unselects.
      QTextCharFormat f = copy.charFormat();
      log.setTextCursor(copy);

      switch(type) {
            case QtDebugMsg:
                  f.setForeground(QBrush(QColor(Qt::yellow)));
                  copy.setCharFormat(f);
                  log.appendPlainText(QString("Debug: %1\n").arg(msge));
                  break;
            case QtWarningMsg:
                  f.setForeground(QBrush(QColor(Qt::magenta)));
                  copy.setCharFormat(f);
                  log.appendPlainText(QString("Warning: %1\n").arg(msge));
                  break;
            case QtCriticalMsg:
                  f.setForeground(QBrush(QColor(Qt::red)));
                  copy.setCharFormat(f);
                  log.appendPlainText(QString("Critical: %1\n").arg(msge));
                  break;
            case QtFatalMsg:
                  f.setForeground(QBrush(QColor(Qt::darkRed)));
                  copy.setCharFormat(f);
                  log.appendPlainText(QString("Fatal: %1\n").arg(msge));
                  break;
            default:
                  log.appendPlainText(QString("Info: %1\n").arg(msge));
                  return; // the original font doesn't need to be reset, since it hasn't changed.
            }
      copy.endEditBlock();
      log.setTextCursor(c);

      mscore->pluginCreator()->logDock->show();
      }

//---------------------------------------------------------
//   runClicked
//---------------------------------------------------------

void PluginCreator::runClicked()
      {
      statusbar->showMessage(tr("Running...\n"), 10000);

      // append vertical line.
      if (!log->toPlainText().isEmpty())
            log->appendPlainText("- - - - - - - - - - - - - - - - - - - - -\n");

      //GREY OUT ALL PREVIOUS OUTPUTS:
      log->grayOutContents();

      log->appendPlainText(QString("Start at: %1\n").arg(QTime::currentTime().toString("hh:mm:ss")));
      QQmlEngine* qml = Ms::MScore::qml();
      connect(qml, SIGNAL(warnings(const QList<QQmlError>&)),
                   SLOT(qmlWarnings(const QList<QQmlError>&)));

      m_item = nullptr;
      QQmlComponent component(qml);
      component.setData(textEdit->toPlainText().toUtf8(), QUrl());
      QObject* obj = component.create();
      if (obj == nullptr) {
            log->appendPlainText(QString("Creating component failed\n"));
            for(QQmlError e: component.errors())
                  log->appendPlainText(QString("   Line %1: %2\n").arg(e.line()).arg(e.description()));
            actionStop->setEnabled(false);
            actionRun->setEnabled(true);
            statusbar->showMessage(tr(" -- Running aborted\n"), 10000);
            return;
            }
      qInstallMessageHandler(qmlMsgHandler);
      actionStop->setEnabled(true);
      actionRun->setEnabled(false);

      m_item = qobject_cast<QmlPlugin*>(obj);
      log->appendPlainText("Plugin Details:\n");
      log->appendPlainText("  Menu Path: " + m_item->menuPath() + "\n");
      log->appendPlainText("  Version: " + m_item->version() + "\n");
      log->appendPlainText("  Description: " + m_item->description() + "\n");
      // if (m_item->requiresScore()) log->appendPlainText("  Requires Score\n");
      if(MuseScoreCore::mscoreCore->currentScore() == nullptr && m_item->requiresScore() == true) {
            QMessageBox::information(nullptr,
                                     tr("MuseScore"),
                                     tr("No score open.\n"
                                        "This plugin requires an open score to run.\n"),
                                     QMessageBox::Ok, QMessageBox::Ok);
            delete obj;
            m_item = nullptr;
            closePlugin();
            statusbar->showMessage(tr(" -- Running aborted\n"), 10000);
            return;
            }
      m_item->setFilePath(m_path.isEmpty() ? QString() : m_path.section('/', 0, -2));

      if (m_item->pluginType() == "dock" || m_item->pluginType() == "dialog") {
            m_view = new QQuickView(qml, 0);
            m_view->setTitle(m_item->menuPath().mid(m_item->menuPath().lastIndexOf(".") + 1));
            m_view->setColor(QApplication::palette().color(QPalette::Window));
            m_view->setResizeMode(QQuickView::SizeRootObjectToView);
            m_view->setWidth(m_item->width());
            m_view->setHeight(m_item->height());
            m_item->setParentItem(m_view->contentItem());

            if (m_item->pluginType() == "dock") {
                  m_dock = new QDockWidget(tr("Plugin"));
                  m_dock->setAttribute(Qt::WA_DeleteOnClose);
                  m_dock->setWidget(QWidget::createWindowContainer(m_view));
                  m_dock->widget()->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
                  Qt::DockWidgetArea area = Qt::RightDockWidgetArea;
                  if (m_item->dockArea() == "left")
                        area = Qt::LeftDockWidgetArea;
                  else if (m_item->dockArea() == "top")
                        area = Qt::TopDockWidgetArea;
                  else if (m_item->dockArea() == "bottom")
                        area = Qt::BottomDockWidgetArea;
                  addDockWidget(area, m_dock);
                  connect(m_dock, SIGNAL(destroyed()), SLOT(closePlugin()));
                  m_dock->widget()->setAttribute(Qt::WA_DeleteOnClose);
                  m_dock->show();
                  }
            m_view->show();
            connect(m_view, SIGNAL(destroyed()), SLOT(closePlugin()));
            }

      connect(qml,  SIGNAL(quit()), SLOT(closePlugin()));

      if (mscore->currentScore() && m_item->pluginType() != "dock")
            mscore->currentScore()->startCmd();
      m_item->runPlugin();
      if (mscore->currentScore() && m_item->pluginType() != "dock")
            mscore->currentScore()->endCmd();
      mscore->endCmd();
      // Main window is on top at this point. Make sure correct view is on top.
      if (m_item->pluginType() == "dock") {
            raise(); // Screen needs to be on top to see docked panel.
            activateWindow();
            }
      else if (m_view) {
            m_view->raise();
            }
      else {
            // Console only, bring to top to see results.
            raise(); // Screen needs to be on top to see docked panel.
            activateWindow();
            }
      }

//---------------------------------------------------------
//   closePlugin
//---------------------------------------------------------

void PluginCreator::closePlugin()
      {
      statusbar->showMessage(tr("Closing..."), 10000);
      actionStop->setEnabled(false);
      actionRun->setEnabled(true);
      if (m_view)
            m_view->close();
      if (m_dock)
            m_dock->close();
      qInstallMessageHandler(nullptr);
      statusbar->showMessage(tr(" -- Closed."), 10000);
      raise();
      activateWindow();
      }

//---------------------------------------------------------
//   stopClicked
//---------------------------------------------------------

void PluginCreator::stopClicked()
      {
      statusbar->showMessage(tr("Stopping..."), 10000);
      closePlugin();
      statusbar->showMessage(tr(" -- Stopped."), 10000);
      }

//---------------------------------------------------------
//   loadPlugin
//---------------------------------------------------------

void PluginCreator::loadPlugin()
      {
      statusbar->showMessage(tr("loading..."), 10000);
      if (m_state == PCState::DIRTY && !m_path.isEmpty()) {
            QMessageBox::StandardButton n = QMessageBox::warning(this, tr("MuseScore"),
               tr("Plugin \"%1\" has unsaved changes.\n"
               "Save before closing?").arg(m_fileName),
               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
               QMessageBox::Save);
            if (n == QMessageBox::Save)
                  savePlugin();
            else if (n == QMessageBox::Cancel) {
                  statusbar->showMessage(tr(" -- Operation canceled."), 10000);
                  return;
                  }
            }
      setPath(mscore->getPluginFilename(true));
      // Write the message before, so it's overwritten by load() if an error occurs.
      statusbar->showMessage(tr(" -- Loaded."), 10000);
      load();

      raise();
      activateWindow();
      }

//---------------------------------------------------------
//   load
//---------------------------------------------------------

void PluginCreator::load()
      {
      if (m_path.isEmpty())
            return;
      QFile f(m_path);
      if (f.open(QIODevice::ReadOnly)) {
            m_blockTextChanged = true;
            textEdit->setPlainText(f.readAll());
            f.close();
            }
      else {
            QMessageBox::warning(this, tr("MuseScore"),
               tr("The file \"%1\" couldn't be opened.\n"
                  "It might be locked, or you might not have permission to open it.")
                                 .arg(m_fileName),
               QMessageBox::Ok,
               QMessageBox::Ok);
            statusbar->showMessage(tr(" -- Failed to load file."), 10000);
            return;
            }
      m_created = false;
      setState(PCState::CLEAN);
      if(stackedWidget->currentIndex() == 1) {
            stackedWidget->setCurrentIndex(0);
            logDock->setVisible(true);
            }
      }

//---------------------------------------------------------
//   reload
//---------------------------------------------------------

void PluginCreator::reload()
      {
      statusbar->showMessage(tr("Reloading..."), 10000);
      if (m_created) {
            QMessageBox::StandardButton button1 = QMessageBox::warning(this, tr("MuseScore"),
               tr("Plugin \"%1\" has never been saved, so it connot be reloaded.\n"
                  "Save?").arg(m_fileName),
               QMessageBox::Save | QMessageBox::Cancel,
               QMessageBox::Save);
            if (button1 == QMessageBox::Save) {
                  doSavePlugin(true);
                  return;
                  }
            else if (button1 == QMessageBox::Cancel) {
                  statusbar->showMessage(tr(" -- The file was not reloaded."), 10000);
                  return;
                  }
            }
      if (m_path.isEmpty()) {
            QMessageBox::StandardButton b2 = QMessageBox::warning(this, tr("MuseScore"),
               tr("Plugin \"%1\" needs to be saved before reloading.\n"
                  "Save?").arg(m_fileName),
               QMessageBox::Save | QMessageBox::Cancel,
               QMessageBox::Save);
            if (b2 == QMessageBox::Save){
                  doSavePlugin(true);
                  return;
                  }
            else if (b2 == QMessageBox::Cancel) {
                  statusbar->showMessage(tr(" -- The file was not reloaded."), 10000);
                  return;
                  }
            }
      // Save the current textCursor position.
      QTextCursor cursor(textEdit->textCursor());
      int pos = cursor.position();
      int anch = cursor.anchor();
      statusbar->showMessage(tr(" -- Reloaded."), 10000);
      // Write the message before, so it's overwritten by load() if an error occurs.
      load();
      cursor.setPosition(anch);
      cursor.setPosition(pos, QTextCursor::KeepAnchor);
      textEdit->setTextCursor(cursor);
      }

//---------------------------------------------------------
//   savePlugin
//---------------------------------------------------------

void PluginCreator::doSavePlugin(bool saveas) 
      {
      if (saveas) {
            setPath(mscore->getPluginFilename(false));
            if (m_path.isEmpty()){
                  statusbar->showMessage(tr(" -- The file was not saved.\n"), 10000);;
                  raise();
                  activateWindow();
                  return;
                  }
            }
      QFile f(m_path);
      if(m_path.section(".", -1, -1) != "qml" ) {
            QMessageBox::critical(mscore, tr("Save Plugin"), tr("Cannot determine file type"));
            statusbar->showMessage(tr(" !-- Not saved.\n"), 10000);
            raise();
            activateWindow();
            return;
            }

      if (f.open(QIODevice::WriteOnly)) {
            f.write(textEdit->toPlainText().toUtf8());
            f.close();
            textEdit->document()->setModified(false);
            m_created = false;
            setState(PCState::CLEAN);
            stackedWidget->setCurrentIndex(0);
            logDock->setVisible(true);
            }
      else {
            QMessageBox::critical(this, tr("MuseScore"),
               tr("The file couldn't be opened for writing: it couldn't be saved."),
               QMessageBox::Ok, QMessageBox::NoButton);
            statusbar->showMessage(tr(" !-- Not saved.\n"), 10000);
            raise();
            activateWindow();
            return;
            }
      statusbar->showMessage(tr(" -- Saved.\n"), 10000);
      raise();
      activateWindow();
      }

void PluginCreator::savePlugin()
      {
      statusbar->showMessage(tr("Saving to: \"") + m_path + tr("\"..."), 10000);
      doSavePlugin(m_created);
      }

void PluginCreator::savePluginAs()
      {
      statusbar->showMessage(tr("Saving..."), 10000);
      doSavePlugin(true);
      statusbar->showMessage(tr("Saved to: \"") + m_fileName + tr("\"."), 10000);
      }

//---------------------------------------------------------
//   newPlugin
//    NOTE: when calling the newPlugin() function, there is no need
//          to call closeCurrentPluginSource() before.
//---------------------------------------------------------

void PluginCreator::newPlugin()
      {
      if (m_state == PCState::DIRTY && !m_path.isEmpty()) {
            QMessageBox::StandardButton n = QMessageBox::warning(this, tr("MuseScore"),
               tr("Plugin \"%1\" has unsaved changes.\n"
               "Save before closing?").arg(m_fileName),
               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
               QMessageBox::Save);
            if (n == QMessageBox::Save)
                  savePlugin();
            else if (n == QMessageBox::Cancel)
                  return;
            }
      setPath(tr("Untitled"));
      m_created = true;
      QString s(
         "import QtQuick 2.0\n"
         "import MuseScore 3.0\n"
         "\n"
         "MuseScore {\n"
         "      menuPath: \"Plugins.pluginName\"\n"
         "      description: \"Description goes here\"\n"
         "      version: \"1.0\"\n"
         "\n"
         "      onRun: {\n"
         "            console.log(\"Hello world!\")\n"
         "            Qt.quit()\n"
         "            }\n"
         "      }\n");
      setState(PCState::CLEAN);
      stackedWidget->setCurrentIndex(0);
      logDock->setVisible(true);
      m_blockTextChanged = true;
      textEdit->setPlainText(s);
      raise();
      activateWindow();
      statusbar->showMessage(tr("Hello, world!"), 10000);
      }

//---------------------------------------------------------
//   textChanged
//---------------------------------------------------------

void PluginCreator::textChanged()
      {
      if (m_blockTextChanged){
            m_blockTextChanged = false;
            return;
            }
      if (textEdit->document()->isModified()){
            setState(PCState::DIRTY);
            // when coming from INIT to DIRTY, the state will not be set to DIRTY, so this won't be called.
            statusbar->showMessage(tr("File has unsaved changes."), 10000);
      }
      else if (m_state == PCState::DIRTY) { //revert state to CLEAN after undo('s).
            setState(PCState::CLEAN);
            if (statusbar->currentMessage() == tr("File has unsaved changes."))
                  statusbar->clearMessage();
            }
      }

//---------------------------------------------------------
//   qmlWarnings
//---------------------------------------------------------

void PluginCreator::qmlWarnings(const QList<QQmlError>& el)
      {
      for(const QQmlError& e: el)
            log->appendPlainText(QString("%1:%2: %3\n").arg(e.line()).arg(e.column()).arg(e.description()));

      actionRun->setEnabled(true);
      actionStop->setEnabled(false);
      }

//---------------------------------------------------------
//   modifyShowLog
//---------------------------------------------------------

void PluginCreator::setLogVisibilityButtons()
      {
      if (logDock->isVisible()) {
            m_showLogButton->setText(tr("Hide log"));
            m_showLogButton->setIcon(*icons[(int) Icons::arrowDown_ICON]);
            m_showLogButton->update();
            actionShowLog->setText(tr("Hide log"));
            actionShowLog->setIcon(*icons[(int) Icons::arrowDown_ICON]);
            }
      else {
            m_showLogButton->setText(tr("Show log"));
            m_showLogButton->setIcon(*icons[(int) Icons::arrowUp_ICON]);
            m_showLogButton->update();
            actionShowLog->setText(tr("Show log"));
            actionShowLog->setIcon(*icons[(int) Icons::arrowUp_ICON]);
            }
      }

//---------------------------------------------------------
//   pageChanged
//    NOTE: hiding the pixmaps makes the logDock more resizable,
//          since it reduces the stackedWidget's height.
//---------------------------------------------------------

void PluginCreator::pageChanged(int index)
      {
      if(index == 0) {
            logDock->setVisible(true);
            pixmapMS->hide();
            pixmapPlugins->hide();

            actionDuplicateLine->setEnabled(true);
            actionMoveLineUp->setEnabled(true);
            actionMoveLineDown->setEnabled(true);

            textEdit->update();
            }
      else {
            logDock->setVisible(false);
            pixmapMS->show();
            pixmapPlugins->show();

            actionDuplicateLine->setEnabled(false);
            actionMoveLineUp->setEnabled(false);
            actionMoveLineDown->setEnabled(false);

            textEdit->update();
            }
      }

//---------------------------------------------------------
//   showManual
//---------------------------------------------------------

void PluginCreator::showManual()
      {
      if (m_helpBrowser == nullptr) {
            m_helpBrowser = new HelpBrowser;
            m_manualDock = new QDockWidget(tr("Manual"));
            m_manualDock->setObjectName("Manual");

            m_manualDock->setWidget(m_helpBrowser);
            addDockWidget(Qt::RightDockWidgetArea, m_manualDock);
            m_helpBrowser->setContent(manualPath());
            }
      m_manualDock->setVisible(!m_manualDock->isVisible());
      }

//---------------------------------------------------------
//   setPath
//---------------------------------------------------------

void PluginCreator::setPath(QString newPath)
      {
      if (m_path == newPath)
            return;
      m_path = newPath;
#ifdef _PLUGIN_CREATOR_NO_SIGNALS_
      emit pathChanged();
#endif //_PLUGIN_CREATOR_NO_SIGNALS_
      onPathChanged();
      }

//---------------------------------------------------------
//   path
//---------------------------------------------------------

QString PluginCreator::path() const
      {
      return m_path;
      }

//---------------------------------------------------------
//   closeCurrentPluginSource
//---------------------------------------------------------

void PluginCreator::closeCurrentPluginSource()
      {
      statusbar->showMessage(tr("Closing..."), 10000);
      if (m_state == PCState::DIRTY && !m_path.isEmpty()) {
            QMessageBox::StandardButton n = QMessageBox::warning(this, tr("MuseScore"),
               tr("Plugin \"%1\" has unsaved changes.\n"
               "Save before closing?").arg(m_fileName),
               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
               QMessageBox::Save);
            if (n == QMessageBox::Save)
                  savePlugin();
            else if (n == QMessageBox::Cancel)
                  return;
            }
      setPath(QString());
      setState(PCState::EMPTY);
      stackedWidget->setCurrentIndex(1);
      logDock->setVisible(false);
      m_blockTextChanged = true;
      textEdit->setPlainText("");
      statusbar->showMessage(tr(" -- Closed."), 10000);
      }

//---------------------------------------------------------
//   showAboutPluginCreator
//---------------------------------------------------------

void PluginCreator::showAboutPluginCreator()
      {
      if (!m_aboutPluginCreator){
            m_aboutPluginCreator = new QDialog(this);
            Ui::AboutPlugins ui;
            ui.setupUi(m_aboutPluginCreator);
            }
      m_aboutPluginCreator->show();
      }


//---------------------------------------------------------
//   setCreated
//---------------------------------------------------------

void PluginCreator::setCreated(bool created)
      {
      m_created = created;
      }

//---------------------------------------------------------
//   created
//---------------------------------------------------------

bool PluginCreator::created() const
      {
      return m_created;
      }

//---------------------------------------------------------
//   pathChanged
//---------------------------------------------------------

void PluginCreator::onPathChanged(){
      QFile f(m_path);
      QFileInfo fi(f);
      m_fileName = fi.completeBaseName();
      onFileNameChanged();
#ifdef _PLUGIN_CREATOR_NO_SIGNALS_
      emit fileNameChanged();
#endif //_PLUGIN_CREATOR_NO_SIGNALS_
}

//---------------------------------------------------------
//   pathChanged
//---------------------------------------------------------

QString PluginCreator::fileName() const
{
      return m_fileName;
}

//---------------------------------------------------------
//   onFileNameChanged
//---------------------------------------------------------

void PluginCreator::onFileNameChanged(){
      setTitle(m_fileName);
      }

} //namespace Ms

