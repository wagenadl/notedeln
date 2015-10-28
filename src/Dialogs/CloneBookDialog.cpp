// App/CloneBookDialog.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// CloneBookDialog.cpp

#include "CloneBookDialog.h"
#include "Translate.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "ui_CloneBookDialog.h"
#include "EProcess.h"
#include <QStyle>
#include <QIcon>
#include "DefaultLocation.h"

CloneBookDialog::CloneBookDialog(QWidget *parent): QDialog(parent) {
  ui = new Ui_cloneBookDialog;
  ui->setupUi(this);
  ui->location->setText(defaultLocation());
  ui->leaf->setText("");
  ui->infoText->setPlainText(Translate::_("key-info"));
  QStyle *s = style();
  if (s) {
    QIcon caution = s->standardIcon(QStyle::SP_MessageBoxInformation, 0, this);
    QPixmap pm = caution.pixmap(48); // what _is_ the appropriate size?
    ui->cautionIcon->setPixmap(pm);
  }
  ui->test->hide();
}

CloneBookDialog::~CloneBookDialog() {
}
 
QString CloneBookDialog::archiveLocation() const {
  return ui->alocation->text();
}

QString CloneBookDialog::archiveHost() const {
  QString u = ui->username->text();
  QString h = ui->host->currentText();
  if (h.isEmpty())
    return "";
  else
    return u.isEmpty() ? h : (u + "@" + h);
}

bool CloneBookDialog::isLocal() const {
  return ui->local->isChecked();
}

QString CloneBookDialog::cloneLocation() const {
  QDir d(cloneDestination());
  return d.absoluteFilePath(leaf());
}

QString CloneBookDialog::cloneDestination() const {
  return ui->location->text();
}

QString CloneBookDialog::leaf() const {
  QString s = archiveLocation();
  if (s.endsWith(".git"))
    s = s.left(s.size()-4);
  int idx = s.lastIndexOf("/");
  return idx<0 ? s : s.mid(idx+1);
}

void CloneBookDialog::abrowse() {
  if (!isLocal()) {
    QMessageBox::warning(this, Translate::_("eln"),
                         Translate::_("no-browse-remote"),
                         QMessageBox::Cancel);
    return;
  }

  QString fn = QFileDialog::getExistingDirectory(this,
			    Translate::_("title-clone-archive-location"),
						 ui->alocation->text());
  if (fn.isEmpty())
    return;
  QDir d(fn);
  if (!d.exists()) {
    QMessageBox::warning(this, Translate::_("eln"),
                         Translate::_("fn-does-not-exist").arg(fn),
                         QMessageBox::Cancel);
    return;
  }
  if (!d.exists("branches") || !d.exists("objects")) {
    QMessageBox::warning(this, Translate::_("eln"),
                         Translate::_("fn-is-not-git").arg(fn),
                         QMessageBox::Cancel);
    return;
  }

  ui->alocation->setText(fn);
}

void CloneBookDialog::browse() {
  QString fn = QFileDialog::getExistingDirectory(this,
					 Translate::_("title-clone-dest-path"),
					 ui->location->text());
  if (fn.isEmpty())
    return;
  QDir d(fn);
  if (!d.exists()) {
    QMessageBox::warning(this, Translate::_("eln"),
                         Translate::_("fn-does-not-exist").arg(fn),
                         QMessageBox::Cancel);
    return;
  }

  QString l = leaf();
  if (!l.isEmpty() && d.exists(l)) {
    QMessageBox::warning(this, Translate::_("eln"),
                         Translate::_("fn-exists").arg(d.absoluteFilePath(l)),
                         QMessageBox::Cancel);
    return;
  }
} 

void CloneBookDialog::updateLocation(QString) {
  ui->leaf->setText(leaf());
}

QString CloneBookDialog::getClone() {
  CloneBookDialog cbd;
  while (cbd.exec()) {
    // OK pressed
    if (cbd.archiveLocation().isEmpty()) {
      QMessageBox::warning(&cbd, Translate::_("eln"),
                           Translate::_("no-alocation"));
      continue;
    }

    if (!cbd.isLocal()) {
      if (cbd.archiveHost().isEmpty()) {
	QMessageBox::warning(&cbd, Translate::_("eln"),
                             Translate::_("no-host"));
        continue;
      }
    }

    QString path = cbd.cloneDestination();
    if (path.isEmpty()) {
      QMessageBox::warning(&cbd, Translate::_("eln"),
                           Translate::_("no-clone"));
      continue;
    }
    
    QDir d(path);
    QString l(cbd.leaf());
    if (d.exists(l)) {
      QMessageBox::warning(&cbd, Translate::_("eln"),
                           Translate::_("exists-clone"));
      continue;
    }
     
    // Basic checks passed
    d.mkpath(l);

    // Run git clone
    EProcess proc;
    proc.setWindowCaption(Translate::_("retrieving-clone"));
    proc.setNoStartMessage(Translate::_("no-git"));
    proc.setCommandAndArgs("git",
                           QStringList() << "clone"
                           << (cbd.isLocal() ? cbd.archiveLocation()
                               : (cbd.archiveHost() + ":"
                                  + cbd.archiveLocation()))
                           << cbd.cloneLocation());
    bool ok = proc.exec();
    if (ok)
      return cbd.archiveLocation();
    
    QMessageBox::warning(&cbd, Translate::_("eln"),
                         Translate::_("clone-failed")
                         + ": " + proc.stdErr());
    // we should remove any partial download
    return "";
  }
  // Cancel pressed
  return "";
}

