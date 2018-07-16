//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//
//  Copyright (C) 2002-2017 Werner Schweer and others
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

#ifndef __PREFERENCESLISTWIDGET_H__
#define __PREFERENCESLISTWIDGET_H__

#include "awl/colorlabel.h"
#include "preferences.h"

#include <QtWidgets>

#define PREF_VALUE_COLUMN 1

namespace Ms {

//---------------------------------------------------------
//   PreferenceItem
//---------------------------------------------------------

// TODO: Add a Use'type'Preference class (ex: UseStringPreferenceItem),
// which is basically a string that can be enabled or not.
// PREF_UI_CANVAS_BG_USECOLOR and PREF_UI_CANVAS_BG_COLOR would be merged into
// a UseColorPreferenceItem instead of 2 distinct items.
class PreferenceItem : public QTreeWidgetItem, public QObject {

      // the name is actually the Url (or #define value) of the preference
      QString _name;

    protected:
      void save(QVariant value);

    public:
      PreferenceItem();
      PreferenceItem(QString name);

      virtual QWidget* editor() const = 0;
      virtual void save() = 0;
      virtual void update() = 0;
      virtual void setDefaultValue() = 0;
      virtual bool isModified() const = 0;

      void setVisible(bool visible);

      QString name() const { return _name; }
      };

//---------------------------------------------------------
//   BoolPreferenceItem
//---------------------------------------------------------
class BoolPreferenceItem : public PreferenceItem {
   private:
      bool _initialValue;
      QCheckBox* _editor;

   public:
      BoolPreferenceItem(QString name);

      QWidget* editor() const {return _editor;}
      inline void save();
      inline void update();
      inline void setDefaultValue();
      inline bool isModified() const;

      };

//---------------------------------------------------------
//   IntPreferenceItem
//---------------------------------------------------------
class IntPreferenceItem : public PreferenceItem {
      int _initialValue;
      QSpinBox* _editor;

   public:
      IntPreferenceItem(QString name);

      QWidget* editor() const {return _editor;}
      inline void save();
      inline void update();
      inline void setDefaultValue();
      inline bool isModified() const;
      };

//---------------------------------------------------------
//   DoublePreferenceItem
//---------------------------------------------------------
class DoublePreferenceItem : public PreferenceItem {
      double _initialValue;
      QDoubleSpinBox* _editor;

   public:
      DoublePreferenceItem(QString name);

      QWidget* editor() const {return _editor;}
      inline void save();
      inline void update();
      inline void setDefaultValue();
      inline bool isModified() const;
      };

//---------------------------------------------------------
//   StringPreferenceItem
//---------------------------------------------------------
class StringPreferenceItem : public PreferenceItem {
      QString _initialValue;
      QLineEdit* _editor;

   public:
      StringPreferenceItem(QString name);

      QWidget* editor() const {return _editor;}
      inline void save();
      inline void update();
      inline void setDefaultValue();
      inline bool isModified() const;
      };

//---------------------------------------------------------
//   FilePreferenceItem
//---------------------------------------------------------
class FilePreferenceItem : public PreferenceItem {
      QString _initialValue;
      QPushButton* _editor;

   public:
      FilePreferenceItem(QString name);

      QWidget* editor() const { return _editor; }
      inline void save();
      inline void update();
      inline void setDefaultValue();
      inline bool isModified() const;

   private slots:
      void getFile() const;
};

//---------------------------------------------------------
//   DirPreferenceItem
//---------------------------------------------------------
class DirPreferenceItem : public PreferenceItem {
      QString _initialValue;
      QPushButton* _editor;

   public:
      DirPreferenceItem(QString name);

      QWidget* editor() const { return _editor; }
      inline void save();
      inline void update();
      inline void setDefaultValue();
      inline bool isModified() const;

   private slots:
      bool getDirectory() const;
};

//---------------------------------------------------------
//   ColorPreferenceItem
//---------------------------------------------------------
class ColorPreferenceItem : public PreferenceItem {
      QColor _initialValue;
      Awl::ColorLabel* _editor;

   public:
      ColorPreferenceItem(QString name);

      QWidget* editor() const {return _editor;}
      inline void save();
      inline void update();
      inline void setDefaultValue();
      inline bool isModified() const;
      };


//---------------------------------------------------------
//   PreferencesListWidget
//---------------------------------------------------------

class PreferencesListWidget : public QTreeWidget, public PreferenceVisitor {

      QHash<QString, PreferenceItem*> preferenceItems;

      void addPreference(PreferenceItem* item);
      QTreeWidgetItem* findChildByName(const QTreeWidgetItem* parent, const QString& text, const int column) const;
      int findChildDepth(QTreeWidgetItem* parent) const;
      void recursiveChildList(QList<QTreeWidgetItem*>& list, QTreeWidgetItem* item) const;
      QList<QTreeWidgetItem*> recursiveChildList(QTreeWidgetItem* parent) const;
      QList<PreferenceItem*> recursivePreferenceItemList(QTreeWidgetItem* parent) const;

      void hideEmptyItems();

   public:
      explicit PreferencesListWidget(QWidget* parent = nullptr);
      void loadPreferences();
      void updatePreferences();

      std::vector<QString> save();

      void visit(QString key, QTreeWidgetItem* parent, IntPreference*);
      void visit(QString key, QTreeWidgetItem* parent, DoublePreference*);
      void visit(QString key, QTreeWidgetItem* parent, BoolPreference*);
      void visit(QString key, QTreeWidgetItem* parent, StringPreference*);
      void visit(QString key, QTreeWidgetItem* parent, FilePreference*);
      void visit(QString key, QTreeWidgetItem* parent, DirPreference*);
      void visit(QString key, QTreeWidgetItem* parent, ColorPreference*);

   public slots:
      void filter(const QString& query);
      void resetAdvancedPreferenceToDefault();
      void showAll(bool all = true);
      void filterVisiblePreferences(const QString& query, bool all);
};

} // namespace Ms

#endif // __PREFERENCESLISTWIDGET_H__
