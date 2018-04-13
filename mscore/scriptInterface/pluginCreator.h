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

#ifndef __PLUGIN_CREATOR_H__
#define __PLUGIN_CREATOR_H__

//#define _PLUGIN_CREATOR_NO_SIGNALS_

#include "ui_pluginCreator.h"

#include <QMainWindow>
#include <QObject>
#include <QQuickView>
#include <QDockWidget>
#include <QString>
#include <QCloseEvent>
#include <QQmlError>
#include <QToolButton>

namespace Ms {

class QmlPlugin;
class HelpBrowser;

//---------------------------------------------------------
//   PluginCreator
//---------------------------------------------------------

class PluginCreator : public QMainWindow, public Ui::PluginCreatorBase {
      Q_OBJECT

      enum PCState : quint8 { INIT, EMPTY, CLEAN, DIRTY };

      PCState m_state;
      bool m_created;

      QString m_path;
      QString m_fileName;
      QmlPlugin *m_item;
      HelpBrowser *m_helpBrowser;
      QDockWidget *m_manualDock;
      QToolButton *m_showLogButton;
      QPointer<QQuickView> m_view;
      QPointer<QDockWidget> m_dock;

      QDialog *m_aboutPluginCreator;

      void setState(PCState);
      PCState state() const;
      void setCreated(bool created = true);
      bool created() const;

      QString path() const;
      void setPath(QString);
      void onPathChanged();
      QString fileName() const;
      void onFileNameChanged();

      virtual void closeEvent(QCloseEvent*);
      void readSettings();
      void writeSettings();
      void setTitle(const QString&);
      QString manualPath() const;
      void doSavePlugin(bool);

      //This blocks once the textChanged() function to work.
      //It is like a one time signal blocker.
      //See the implementation of textChanged() for more details.
      //The other way would have been to disconnect and then to
      //reconnect right after, but that would have been way more expensive.
      bool m_blockTextChanged;

   private slots:
      void runClicked();
      void stopClicked();

      void loadPlugin();
      void reload();
      void load();
      void newPlugin();

      void savePlugin();
      void savePluginAs();

      void textChanged();

      void closePlugin();
      void closeCurrentPluginSource();

      void showManual();
      void qmlWarnings(const QList<QQmlError>&);

      void setLogVisibilityButtons();
      void toggleLogVisible() { logDock->setVisible(!logDock->isVisible()); }

      void pageChanged(int);

      void showAboutPluginCreator();

   public:
      PluginCreator(QWidget* parent = nullptr);
      ~PluginCreator();

      QDialog *aboutPluginCreator() const;

#ifdef _PLUGIN_CREATOR_NO_SIGNALS_
   signals:
      void pathChanged();
      void fileNameChanged();
#endif //_PLUGIN_CREATOR_NO_SIGNALS_
};


} // namespace Ms
#endif
