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

NewBookDialog::NewBookDialog(QWidget *parent): QDialog(parent) {
  ui = new Ui_newBookDialog();
  ui->setupUi(this);

  connect(ui->location, SIGNAL(textChanged(QString)),
          SLOT(locationChanged(QString)));
  
  ui->local->setChecked(true);
  ui->archive->setChecked(false);
  if (!VersionControl::isGitAvailable())
    ui->archive->hide();

  QStyle *s = style();
  if (s) {
    QIcon caution = s->standardIcon(QStyle::SP_MessageBoxInformation, 0, this);
    QPixmap pm = caution.pixmap(48); // what _is_ the appropriate size?
    ui->cautionIcon->setPixmap(pm);
  }
  ui->test->hide();
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
  return ui->host->currentText();
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
                                            "", "Notebooks (*.nb)");
  if (fn.isEmpty())
    return;
  
  if (!fn.endsWith(".nb"))
    fn += ".nb";
  ui->location->setText(fn);
}

void NewBookDialog::abrowse() {
  QString fn = QFileDialog::getExistingDirectory(this,
                                              Translate::_("create-archive"));
  if (!fn.isEmpty())
    ui->alocation->setText(fn);
}

void NewBookDialog::locationChanged(QString) {
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
                                              "",
                                              Translate::_("Notebooks")
                                              + " (*.nb)");
    if (fn.isEmpty())
      return "";
    if (!fn.endsWith(".nb"))
      fn += ".nb";
    bool ok = Notebook::create(fn, "");
    if (ok)
      return fn;
    QMessageBox::critical(0, "eln",
                Translate::_("could-not-create-notebook").arg(fn)
			  + "\n" + Notebook::errorMessage(),
                          QMessageBox::Cancel);
 }
 return ""; // not executed
}

QString NewBookDialog::getNewArchive() {
  NewBookDialog nbd;
  while (nbd.exec()) {
    QString fn = nbd.location();
    if (fn.isEmpty()) {
      QMessageBox::warning(&nbd, "eln", Translate::_("specify-location"));
      continue;
    }

    if (QDir::current().exists(fn)) {
      QMessageBox::warning(&nbd, "eln",
			   Translate::_("could-not-create-notebook-exists").
			   arg(fn),
                           QMessageBox::Cancel);
      continue;
    }

    bool ok = Notebook::create(fn, nbd.hasArchive() ? "git" : "");
    if (!ok) {
      QMessageBox::critical(&nbd, "eln",
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

        QMessageBox::critical(&nbd, "eln", msg, QMessageBox::Cancel);
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
        QMessageBox::critical(&nbd, "eln", msg, QMessageBox::Cancel);
        continue;
      }
    }

    return fn; // all good!
  }

  return "";
}
      
