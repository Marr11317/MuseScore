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

#include "preferenceslistwidget.h"

namespace Ms {

PreferencesListWidget::PreferencesListWidget(QWidget* parent)
      : QTreeWidget(parent)
      {
      header()->setSectionResizeMode(0, QHeaderView::Interactive);
      header()->resizeSections(QHeaderView::ResizeToContents);
      sortByColumn(0, Qt::AscendingOrder);
      loadPreferences();
      resizeColumnToContents(1);
      }

QTreeWidgetItem* PreferencesListWidget::findChildByName(QTreeWidgetItem* parent, QString name, int column)
      {
      for (int childNum = 0; childNum < parent->childCount(); ++childNum) {
            QTreeWidgetItem* child = parent->child(childNum);
            if (child->text(column) == name)
                  return child;
            }
      return nullptr;
      }

void PreferencesListWidget::loadPreferences()
      {
      QTreeWidgetItem* currentParent = invisibleRootItem();
      // iterate over all the preferences.
      for (QString path : preferences.allPreferences().keys()) {
            Preference* pref = preferences.allPreferences().value(path);
            if(!pref->showInAdvancedList())
                  continue;

            // iterate over the directories of the preferences.
            QStringList dirs = path.split("/");
            for (int dirNumber = 0; dirNumber < dirs.count(); ++dirNumber) {
                  QString currentDir = dirs.at(dirNumber);
                  // check if child already exists.
                  QTreeWidgetItem* child = findChildByName(currentParent, currentDir, 0);
                  // if doesn't exist, appendChild. if exist, current parent becomes child.
                  if (!child) {
                        // if it's not a "directory" but it's a file, then just change it to the corresponding preferenceItem.
                        if (dirNumber == dirs.count() - 1)
                              pref->accept(path, currentParent, *this);
                        else
                              currentParent->addChild(new QTreeWidgetItem(currentParent, QStringList() << currentDir));
                        currentParent = currentParent->child(currentParent->childCount() - 1);
                        }
                  else
                        currentParent = child;
                  }
            // once the preference is put, get back to the root item
            // to put the next preference.
            currentParent = invisibleRootItem();
            }
      }

void PreferencesListWidget::updatePreferences()
      {
      for (PreferenceItem* item : preferenceItems.values())
            item->update();
      }

void PreferencesListWidget::addPreference(PreferenceItem* item)
      {
      setItemWidget(item, PREF_VALUE_COLUMN, item->editor());
      preferenceItems[item->name()] = item;
      }

void PreferencesListWidget::visit(QString key, QTreeWidgetItem* parent, IntPreference*)
      {
      IntPreferenceItem* item = new IntPreferenceItem(key);
      parent->addChild(item);
      addPreference(item);
      }

void PreferencesListWidget::visit(QString key, QTreeWidgetItem* parent, DoublePreference*)
      {
      DoublePreferenceItem* item = new DoublePreferenceItem(key);
      parent->addChild(item);
      addPreference(item);
      }

void PreferencesListWidget::visit(QString key, QTreeWidgetItem* parent, BoolPreference*)
      {
      BoolPreferenceItem* item = new BoolPreferenceItem(key);
      parent->addChild(item);
      addPreference(item);
      }

void PreferencesListWidget::visit(QString key, QTreeWidgetItem* parent, StringPreference*)
      {
      StringPreferenceItem* item = new StringPreferenceItem(key);
      parent->addChild(item);
      addPreference(item);
      }

void PreferencesListWidget::visit(QString key, QTreeWidgetItem* parent, ColorPreference*)
      {
      ColorPreferenceItem* item = new ColorPreferenceItem(key);
      parent->addChild(item);
      addPreference(item);
      }

void PreferencesListWidget::filter(const QString& query)
      {
      QString s = query.toLower();
      for (PreferenceItem* item : preferenceItems.values())
            item->setHidden(!item->name().toLower().contains(s));


      }

void PreferencesListWidget::resetAdvancedPreferenceToDefault()
      {
      preferences.setReturnDefaultValues(true);
      for (QTreeWidgetItem* item : selectedItems()) {
            if (item->childCount()) // The item is not a PreferenceItem, since it has children.
                  continue;
            PreferenceItem* pref = static_cast<PreferenceItem*>(item);
            pref->setDefaultValue();
            }
      preferences.setReturnDefaultValues(false);
      }

void PreferenceItem::setVisible(bool visible)
      {
      if (!visible == isHidden())
            return;

      if (visible) {
            QTreeWidgetItem* par = QTreeWidgetItem::parent();
            while(par) {
                  par->setHidden(false);
                  par = par->parent();
                  }
            }
      else {

            }
      }


std::vector<QString> PreferencesListWidget::save()
      {
      std::vector<QString> changedPreferences;
      for (PreferenceItem* item : preferenceItems.values()) {
            if (item->isModified()) {
                  item->save();
                  changedPreferences.push_back(item->name());
                  }
            }

      return changedPreferences;
      }

//---------------------------------------------------------
//   PreferenceItem
//---------------------------------------------------------

PreferenceItem::PreferenceItem()
{
}

PreferenceItem::PreferenceItem(QString name)
      : _name(name)
      {
      setText(0, name.split("/").last());
      setSizeHint(1, QSize(100, 40));
      }

void PreferenceItem::save(QVariant value)
      {
      preferences.setPreference(name(), value);
      }

//---------------------------------------------------------
//   ColorPreferenceItem
//---------------------------------------------------------

ColorPreferenceItem::ColorPreferenceItem(QString name)
      : PreferenceItem(name),
        _initialValue(preferences.getColor(name)),
        _editor(new Awl::ColorLabel)
      {
      _editor->setColor(_initialValue);
      _editor->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      _editor->setToolTip(tr("Click to modify the color"));
      }

void ColorPreferenceItem::save()
      {
      QColor newValue = _editor->color();
      _initialValue = newValue;
      PreferenceItem::save(newValue);
      }

void ColorPreferenceItem::update()
      {
      QColor newValue = preferences.getColor(name());
      _editor->setColor(newValue);
      }

void ColorPreferenceItem::setDefaultValue()
      {
      _editor->setColor(preferences.defaultValue(name()).value<QColor>());
      }

bool ColorPreferenceItem::isModified() const
      {
      return _initialValue != _editor->color();
      }


//---------------------------------------------------------
//   IntPreferenceItem
//---------------------------------------------------------

IntPreferenceItem::IntPreferenceItem(QString name)
      : PreferenceItem(name),
        _initialValue(preferences.getInt(name))
{
      _editor = new QSpinBox;
      _editor->setMaximum(INT_MAX);
      _editor->setMinimum(INT_MIN);
      _editor->setValue(_initialValue);
}

void IntPreferenceItem::save()
      {
      int newValue = _editor->value();
      _initialValue = newValue;
      PreferenceItem::save(newValue);
      }

void IntPreferenceItem::update()
      {
      int newValue = preferences.getInt(name());
      _editor->setValue(newValue);
      }

void IntPreferenceItem::setDefaultValue()
      {
      _editor->setValue(preferences.defaultValue(name()).toInt());
      }


bool IntPreferenceItem::isModified() const
      {
      return _initialValue != _editor->value();
      }

//---------------------------------------------------------
//   DoublePreferenceItem
//---------------------------------------------------------

DoublePreferenceItem::DoublePreferenceItem(QString name)
      : PreferenceItem(name),
        _initialValue(preferences.getDouble(name)),
        _editor(new QDoubleSpinBox)
      {
      _editor->setMaximum(DBL_MAX);
      _editor->setMinimum(DBL_MIN);
      _editor->setValue(_initialValue);
      }

void DoublePreferenceItem::save()
      {
      double newValue = _editor->value();
      _initialValue = newValue;
      PreferenceItem::save(newValue);
      }

void DoublePreferenceItem::update()
      {
      double newValue = preferences.getDouble(name());
      _editor->setValue(newValue);
      }

void DoublePreferenceItem::setDefaultValue()
      {
      _editor->setValue(preferences.defaultValue(name()).toDouble());
      }

bool DoublePreferenceItem::isModified() const
      {
      return _initialValue != _editor->value();
      }


//---------------------------------------------------------
//   BoolPreferenceItem
//---------------------------------------------------------

BoolPreferenceItem::BoolPreferenceItem(QString name)
      : PreferenceItem(name),
        _initialValue(preferences.getBool(name)),
        _editor(new QCheckBox)
      {
      _editor->setChecked(_initialValue);
      auto setCheckboxTextAndToolTip = [&](bool checked)
             {
             _editor->setText(checked ? tr("true") : tr("false"));
             _editor->setToolTip(checked ? tr("true") : tr("false"));
             };
      setCheckboxTextAndToolTip(_initialValue);
      connect(_editor, &QCheckBox::toggled, this, setCheckboxTextAndToolTip);
      }

void BoolPreferenceItem::save()
      {
      bool newValue = _editor->isChecked();
      _initialValue = newValue;
      PreferenceItem::save(newValue);
      }

void BoolPreferenceItem::update()
      {
      bool newValue = preferences.getBool(name());
      _editor->setChecked(newValue);
      }

void BoolPreferenceItem::setDefaultValue()
      {
      _editor->setChecked(preferences.defaultValue(name()).toBool());
      }

bool BoolPreferenceItem::isModified() const
      {
      return _initialValue != _editor->isChecked();
      }

//---------------------------------------------------------
//   StringPreferenceItem
//---------------------------------------------------------

StringPreferenceItem::StringPreferenceItem(QString name)
      : PreferenceItem(name),
        _initialValue(preferences.getString(name)),
        _editor(new QLineEdit)
      {
      _editor->setText(_initialValue);
      }

void StringPreferenceItem::save()
      {
      QString newValue = _editor->text();
      _initialValue = newValue;
      PreferenceItem::save(newValue);
      }

void StringPreferenceItem::update()
      {
      QString newValue = preferences.getString(name());
      _editor->setText(newValue);
      }

void StringPreferenceItem::setDefaultValue()
      {
      _editor->setText(preferences.defaultValue(name()).toString());
      }

bool StringPreferenceItem::isModified() const
      {
      return _initialValue != _editor->text();
      }



} // namespace Ms
