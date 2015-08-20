// NewBookDialog.cpp

#include "NewBookDialog.h"
#include "ui_NewBookDialog.h"
#include <QFileInfo>
#include <QFileDialog>
#include "Translate.h"
#include "Process.h"
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
  QString fn = QFileDialog::getSaveFileName(0, Translate::_("create-path"),
                                            "", "Notebooks (*.nb)");
  if (fn.isEmpty())
    return "";
  if (!fn.endsWith(".nb"))
    fn += ".nb";
  return fn;
}

QString NewBookDialog::getNewArchive() {
  NewBookDialog nbd;
  while (nbd.exec()) {
    QString fn = nbd.location();
    if (fn.isEmpty()) {
      QMessageBox::warning(&nbd, "eln",
                           "Please specify a location for your new notebook");
      continue;
    }

    if (QDir::current().exists(fn)) {
      QMessageBox::warning(&nbd, "eln",
                           "Will not create a new notebook '" + fn
                           + "': file exists.",
                           QMessageBox::Cancel);
      continue;
    }

    bool ok = Notebook::create(fn, nbd.hasArchive() ? "git" : "");
    if (!ok) {
      QMessageBox::critical(&nbd, "eln",
                          "'" + fn + "' could not be created.",
                          QMessageBox::Cancel);
      continue;
    }
    
    if (nbd.hasArchive()) {
      QString dst = nbd.archiveRoot() + "/" + nbd.leaf() + ".git";
      Process proc;
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
        QString msg = "Failed to create archive: " + proc.stderr();
        if (disaster)
          msg += " - And I failed to clean up. You will have to manually remove " + QString::fromUtf8("“") + fn + QString::fromUtf8("”");

        QMessageBox::critical(&nbd, "eln", msg, QMessageBox::Cancel);
        continue;
      }
      
      proc.setNoStartMessage("Could not run git");
      proc.setCommandAndArgs("git", QStringList() 
                             << "push" << "--set-upstream" << dst << "master");
      if (!proc.exec()) {
        bool disaster = !RmDir::recurse(fn);
        QString msg = "Failed to push to archive: " + proc.stderr();
        if (disaster)
          msg += " - And I failed to clean up. You will have to manually remove " + QString::fromUtf8("“") + fn + QString::fromUtf8("”");
        QMessageBox::critical(&nbd, "eln", msg, QMessageBox::Cancel);
        continue;
      }
    }

    return fn; // all good!
  }

  return "";
}
      
