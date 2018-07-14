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
      setupFilter();
      }

// setup the search filter
void AdvancedPreferencesWidget::setupFilter()
      {
      QAbstractItemModel* model = ui->treePreferencesWidget->model();
      QSortFilterProxyModel* filter = new QSortFilterProxyModel(ui->treePreferencesWidget);
      filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
      filter->setRecursiveFilteringEnabled(true);
      // enable filtering for all columns
      filter->setFilterKeyColumn(-1);

      filter->setSourceModel(model);
      ui->treePreferencesWidget->QTreeView::setModel(filter);
      connect(ui->searchLineEdit, &QLineEdit::textChanged, filter, &QSortFilterProxyModel::setFilterFixedString);
      }

AdvancedPreferencesWidget::~AdvancedPreferencesWidget()
      {
      delete ui;
      }

} // Ms

