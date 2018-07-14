//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//
//  Copyright (C) 2002-2011 Werner Schweer and others
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

#include "advancedpreferenceswidget.h"

namespace Ms {

AdvancedPreferencesWidget::AdvancedPreferencesWidget(QWidget* parent) :
      QWidget(parent),
      ui(new Ui::AdvancedPreferencesWidget)
      {
      ui->setupUi(this);
      connect(ui->searchLineEdit, &QLineEdit::textChanged, ui->treePreferencesWidget, &PreferencesListWidget::filter);
      connect(ui->resetToDefaultButton, &QPushButton::clicked, ui->treePreferencesWidget, &PreferencesListWidget::resetAdvancedPreferenceToDefault);
      connect(ui->treePreferencesWidget, &QTreeWidget::itemSelectionChanged, this, &AdvancedPreferencesWidget::enableResetPreferenceToDefault);
      }

AdvancedPreferencesWidget::~AdvancedPreferencesWidget()
      {
      delete ui;
      }

void AdvancedPreferencesWidget::enableResetPreferenceToDefault()
      {
      for (QTreeWidgetItem* item: ui->treePreferencesWidget->selectedItems()) {
            if (item->childCount()) { // the item has children and so can't be a PreferenceItem.
                  ui->resetToDefaultButton->setEnabled(false);
                  return;
                  }
            }
      // if all selected items are PreferenceItems, then enable resetToDefaultButton.
      ui->resetToDefaultButton->setEnabled(ui->treePreferencesWidget->selectedItems().count());
      }

} // Ms

