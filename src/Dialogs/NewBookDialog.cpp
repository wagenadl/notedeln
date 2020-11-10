// App/NewBookDialog.cpp - This file is part of eln

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

// NewBookDialog.cpp

#include "NewBookDialog.h"
#include "ui_NewBookDialog.h"
#include <QFileInfo>
#include <QFileDialog>
#include "Translate.h"
#include "EProcess.h"
#include <QMessageBox>
#include "Notebook.h"
#include "RmDir.h"
#include <QDebug>
#include <QStyle>
#include <QIcon>
#include "VersionControl.h"
#include "DefaultLocation.h"

NewBookDialog::NewBookDialog(QWidget *parent): QDialog(parent) {
  ui = new Ui_newBookDialog();
  ui->setupUi(this);

  ui->location->setText(defaultLocation() + "/book.nb");
  connect(ui->location, SIGNAL(textChanged(QString)),
          SLOT(locationChanged()));
  
  ui->local->setChecked(true);
  ui->archive->setChecked(false);
  if (!VersionControl::isGitAvailable())
    ui->archive->hide();
  ui->infoText->setPlainText(Translate::_("key-info"));
  
  QStyle *s = style();
  if (s) {
    QIcon caution = s->standardIcon(QStyle::SP_MessageBoxInformation, 0, this);
    QPixmap pm = caution.pixmap(48); // what _is_ the appropriate size?
    ui->cautionIcon->setPixmap(pm);
  }
  ui->test->hide();
  locationChanged();
}

NewBookDialog::~NewBookDialog() {
}

QString NewBookDialog::location() const {
  QString fn = ui->location->text();
  if (!fn.endsWith(".nb"))
    fn += ".nb";
  return fn;
}

bool NewBookDialog::hasArchive() const {
  return ui->archive->isChecked();
}

bool NewBookDialog::isRemote() const {
  return ui->remote->isChecked();
}

QString NewBookDialog::remoteHost() const {
  QString u = ui->username->text();
  QString h = ui->host->currentText();
  if (h.isEmpty())
    return "";
  else
    return u.isEmpty() ? h : (u + "@" + h);
}

QString NewBookDialog::archiveRoot() const {
  return ui->alocation->text();
}

QString NewBookDialog::leaf() const {
  QFileInfo fi(location());
  return fi.fileName();
}

void NewBookDialog::browse() {
  QString fn = QFileDialog::getSaveFileName(this, Translate::_("create-path"),
                                            ui->location->text(),
					    "Notebooks (*.nb)");
  if (fn.isEmpty())
    return;
  
  if (!fn.endsWith(".nb"))
    fn += ".nb";
  ui->location->setText(fn);
}

void NewBookDialog::abrowse() {
  qDebug() << "abrowse";
  QString loc = defaultLocation();
  qDebug() << "location" << loc;
  QString ttl = Translate::_("create-archive");
  qDebug() << "title" << ttl;
  QString fn = QFileDialog::getExistingDirectory(0, ttl, loc);
  qDebug() << "fn" << fn;
  if (!fn.isEmpty())
    ui->alocation->setText(fn);
}

void NewBookDialog::locationChanged() {
  ui->leaf->setText("/" + leaf() + ".git");
}

QString NewBookDialog::getNew() {
  if (VersionControl::isGitAvailable())
    return getNewArchive();
  else
    return getNewSimple();
}

QString NewBookDialog::getNewSimple() {
  while (true) {
    QString fn = QFileDialog::getSaveFileName(0,
                                              Translate::_("create-path"),
                                              defaultLocation(),
                                              Translate::_("Notebooks")
                                              + " (*.nb)");
    if (fn.isEmpty())
      return "";
    if (!fn.endsWith(".nb"))
      fn += ".nb";
    bool ok = Notebook::create(fn, "");
    if (ok)
      return fn;
    QMessageBox::critical(0, Translate::_("eln"),
                Translate::_("could-not-create-notebook").arg(fn)
			  + "\n" + Notebook::errorMessage(),
                          QMessageBox::Cancel);
 }
 return ""; // not executed
}

QString NewBookDialog::getNewArchive() {
  NewBookDialog nbd;
  qDebug() << "getnewarchive";
  while (nbd.exec()) {
    qDebug() << "nbd.exec";
    QString fn = nbd.location();
    if (fn.isEmpty()) {
      QMessageBox::warning(&nbd, Translate::_("eln"),
                           Translate::_("specify-location"));
      continue;
    }

    if (QDir::current().exists(fn)) {
      QMessageBox::warning(&nbd, Translate::_("eln"),
			   Translate::_("could-not-create-notebook-exists").
			   arg(fn),
                           QMessageBox::Cancel);
      continue;
    }

    bool ok = Notebook::create(fn, nbd.hasArchive() ? "git" : "");
    if (!ok) {
      QMessageBox::critical(&nbd, Translate::_("eln"),
			    Translate::_("could-not-create-notebook").arg(fn)
			    + "\n" + Notebook::errorMessage(),
                          QMessageBox::Cancel);
      continue;
    }
    
    if (nbd.hasArchive()) {
      QString dst = nbd.archiveRoot() + "/" + nbd.leaf() + ".git";
      EProcess proc;
      proc.setWorkingDirectory(fn);
      proc.setWindowCaption("Creating archive");
      if (nbd.isRemote()) {
        QString host = nbd.remoteHost();
        proc.setNoStartMessage("Could not run ssh");
        proc.setCommandAndArgs("ssh", QStringList()
                               << nbd.remoteHost()
                               << "git" << "init" << "--bare" << dst);
        dst = host + ":" + dst;
      } else {
        proc.setNoStartMessage("Could not run git");
        proc.setCommandAndArgs("git", QStringList()
                               << "init" << "--bare" << dst);
      }
      if (!proc.exec()) {
        bool disaster = !RmDir::recurse(fn);
        QString msg = Translate::_("failed-to-create-archive")
      .arg(proc.stdErr());
        if (disaster)
          msg += Translate::_("failed-to-clean-up").arg(fn);

        QMessageBox::critical(&nbd, Translate::_("eln"),
                              msg, QMessageBox::Cancel);
        continue;
      }
      
      proc.setNoStartMessage(Translate::_("no-git"));
      proc.setCommandAndArgs("git", QStringList() 
                             << "push" << "--set-upstream" << dst << "master");
      if (!proc.exec()) {
        bool disaster = !RmDir::recurse(fn);
        QString msg = Translate::_("failed-to-push-archive").arg(proc.stdErr());
        if (disaster)
          msg += Translate::_("failed-to-clean-up").arg(fn);
        QMessageBox::critical(&nbd, Translate::_("eln"),
                              msg, QMessageBox::Cancel);
        continue;
      }
    }

    return fn; // all good!
  }

  return "";
}
      
