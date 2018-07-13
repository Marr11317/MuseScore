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

#define PREF_VALUE_COLUMN 1

namespace Ms {

//---------------------------------------------------------
//   PreferenceItem
//---------------------------------------------------------

// TODO: Add a FilePreferenceItem class.

// TODO: Add a Use'type'Preference class (ex: UseStringPreferenceItem),
// which is basically a string that can be enabled or not.
// PREF_UI_CANVAS_BG_USECOLOR and PREF_UI_CANVAS_BG_COLOR would be merged into
// a UseColorPreferenceItem instead of 2 distinct items.
class PreferenceItem : public QTreeWidgetItem, public QObject {

      QString _name;

    protected:
      void save(QVariant value);

    public:
      PreferenceItem();
      PreferenceItem(QString name);

      virtual void save() = 0;
      virtual void update() = 0;
      virtual void setDefaultValue() = 0;
      virtual QWidget* editor() const = 0;
      virtual bool isModified() const = 0;

      QString name() const {return _name;}

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

      void save();
      void update();
      void setDefaultValue();
      QWidget* editor() const {return _editor;}
      bool isModified() const;

      };


//---------------------------------------------------------
//   IntPreferenceItem
//---------------------------------------------------------
class IntPreferenceItem : public PreferenceItem {
      int _initialValue;
      QSpinBox* _editor;

   public:
      IntPreferenceItem(QString name);

      void save();
      void update();
      void setDefaultValue();
      QWidget* editor() const {return _editor;}
      bool isModified() const;
      };

//---------------------------------------------------------
//   DoublePreferenceItem
//---------------------------------------------------------
class DoublePreferenceItem : public PreferenceItem {
      double _initialValue;
      QDoubleSpinBox* _editor;

   public:
      DoublePreferenceItem(QString name);

      void save();
      void update();
      void setDefaultValue();
      QWidget* editor() const {return _editor;}
      bool isModified() const;
      };

//---------------------------------------------------------
//   StringPreferenceItem
//---------------------------------------------------------
class StringPreferenceItem : public PreferenceItem {
      QString _initialValue;
      QLineEdit* _editor;

   public:
      StringPreferenceItem(QString name);

      void save();
      void update();
      void setDefaultValue();
      QWidget* editor() const {return _editor;}
      bool isModified() const;
      };

//---------------------------------------------------------
//   ColorPreferenceItem
//---------------------------------------------------------
class ColorPreferenceItem : public PreferenceItem {
      QColor _initialValue;
      Awl::ColorLabel* _editor;

   public:
      ColorPreferenceItem(QString name);

      void save();
      void update();
      void setDefaultValue();
      QWidget* editor() const {return _editor;}
      bool isModified() const;
      };


//---------------------------------------------------------
//   PreferencesListWidget
//---------------------------------------------------------

class PreferencesListWidget : public QTreeWidget, public PreferenceVisitor {

      QHash<QString, PreferenceItem*> preferenceItems;

      void addPreference(PreferenceItem* item);
      QTreeWidgetItem* findChildByName(QTreeWidgetItem* parent, QString name, int column);

   public:
      explicit PreferencesListWidget(QWidget* parent = 0);
      void loadPreferences();
      void updatePreferences();

      std::vector<QString> save();

      void visit(QString key, QTreeWidgetItem* parent, IntPreference*);
      void visit(QString key, QTreeWidgetItem* parent, DoublePreference*);
      void visit(QString key, QTreeWidgetItem* parent, BoolPreference*);
      void visit(QString key, QTreeWidgetItem* parent, StringPreference*);
      void visit(QString key, QTreeWidgetItem* parent, ColorPreference*);

};

} // namespace Ms

#endif // __PREFERENCESLISTWIDGET_H__
