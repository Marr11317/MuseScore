//=============================================================================
//  MusE Score
//  Linux Music Score Editor
//
//  Copyright (C) 2002-2008 Werner Schweer and others
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

#include "metaedit.h"
#include "libmscore/score.h"
#include "libmscore/undo.h"
#include "musescore.h"
#include "icons.h"

namespace Ms {

//---------------------------------------------------------
//   MetaEditDialog
//---------------------------------------------------------

MetaEditDialog::MetaEditDialog(Score* s, QWidget* parent)
   : QDialog(parent)
      {
      setObjectName("MetaEditDialog");
      setupUi(this);
      setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
      score = s;
      dirty = false;

      level->setValue(score->mscVersion());
      version->setText(score->mscoreVersion());
      int rev = score->mscoreRevision();
      if (rev > 99999)  // MuseScore 1.3 is decimal 5702, 2.0 and later uses a 7-digit hex SHA
            revision->setText(QString::number(rev, 16));
      else
            revision->setText(QString::number(rev, 10));
      filePath->setText(score->importedFilePath());
      filePath->setTextInteractionFlags(Qt::TextSelectableByMouse);

      int idx = 0;
      QMapIterator<QString, QString> i(s->metaTags());
      QGridLayout* grid = static_cast<QGridLayout*>(scrollWidget->layout());
      while (i.hasNext()) {
            i.next();
            QLabel* l = new QLabel;
            l->setText(i.key());
            QLineEdit* text = new QLineEdit(i.value(), 0);
            connect(text, SIGNAL(textChanged(const QString&)), SLOT(setDirty()));
            grid->addWidget(l, idx, 0);
            grid->addWidget(text, idx, 1);
            ++idx;
            }
      revealButton->setIcon(*icons[int(Icons::fileOpenLocation_ICON)]);
#if defined(Q_OS_WIN)
      revealButton->setToolTip("Show in Explorer");
#elif defined(Q_OS_MAC)
      revealButton->setToolTip("Reveal in Finder");
#else
      revealButton->setToolTip("Open file location");
#endif
      connect(revealButton, SIGNAL(clicked()), SLOT(openCurrentFileLocation()));
      connect(newButton, SIGNAL(clicked()), SLOT(newClicked()));
      MuseScore::restoreGeometry(this);
      }

//---------------------------------------------------------
//   newClicked
//---------------------------------------------------------

void MetaEditDialog::newClicked()
      {
      QString s = QInputDialog::getText(this,
         tr("Input Tag Name"),
         tr("New tag name:")
         );
      QGridLayout* grid = static_cast<QGridLayout*>(scrollWidget->layout());
      if (!s.isEmpty()) {
            int idx = grid->rowCount();
            QLabel* l = new QLabel;
            l->setText(s);
            QLineEdit* text = new QLineEdit;
            grid->addWidget(l, idx, 0);
            grid->addWidget(text, idx, 1);
            }
      dirty = true;
      }

//---------------------------------------------------------
//   accept
//---------------------------------------------------------

void MetaEditDialog::accept()
      {
      if (dirty) {
            QGridLayout* grid = static_cast<QGridLayout*>(scrollWidget->layout());
            int idx = grid->rowCount();
            QMap<QString, QString> m;
            for (int i = 0; i < idx; ++i) {
                  QLayoutItem* labelItem = grid->itemAtPosition(i, 0);
                  QLayoutItem* dataItem  = grid->itemAtPosition(i, 1);
                  if (labelItem && dataItem) {
                        QLabel* l = static_cast<QLabel*>(labelItem->widget());
                        QLineEdit* le = static_cast<QLineEdit*>(dataItem->widget());
                        m.insert(l->text(), le->text());
                        }
                  }
            score->undo(new ChangeMetaTags(score, m));
            }
      QDialog::accept();
      }

//---------------------------------------------------------
//   hideEvent
//---------------------------------------------------------

void MetaEditDialog::hideEvent(QHideEvent* event)
      {
      MuseScore::saveGeometry(this);
      QWidget::hideEvent(event);
      }

//---------------------------------------------------------
//   openFileLocation
//---------------------------------------------------------

void MetaEditDialog::openFileLocation(const QString& path, QWidget* parent)
      {
      const QFileInfo fileInfo(path);
      QStringList args;
#if defined(Q_OS_WIN)
      QStringList param;
      if (!fileInfo.isDir())
            param += QLatin1String("/select,");
      param += QDir::toNativeSeparators(fileInfo.canonicalFilePath());
      if(QProcess::startDetached("explorer.exe", param))
            return;
#elif defined(Q_OS_MAC)
      QStringList scriptArgs;
      scriptArgs << QLatin1String("-e")
             << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                                   .arg(fileInfo.canonicalFilePath());
      QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
      scriptArgs.clear();
      scriptArgs << QLatin1String("-e")
             << QLatin1String("tell application \"Finder\" to activate");
      if (!QProcess::execute("/usr/bin/osascript", scriptArgs))
            return;
#endif // not #else so that the following line can be used as a fallback.
      if (!QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.isDir()? path : fileInfo.path())))
            QMessageBox::warning(parent, QString(QT_TR_NOOP("Open File Location Error")),
                                         QString(QT_TR_NOOP("Could not open file location")));
      }

//---------------------------------------------------------
//   openCurrentFileLocation
//---------------------------------------------------------

void MetaEditDialog::openCurrentFileLocation()
      {
      openFileLocation(filePath->text(), this);
      }
} // namespace Ms
