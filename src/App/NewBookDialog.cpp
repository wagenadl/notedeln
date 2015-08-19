// NewBookDialog.cpp

#include "NewBookDialog.h"
#include "ui_NewBookDialog.h"
#include <QFileInfo>
#include <QFileDialog>
#include "Translate.h"
#include "Process.h"
#include <QMessageBox>
#include "Notebook.h"

NewBookDialog::NewBookDialog(QWidget *parent): QDialog(parent) {
  ui = new Ui_newBookDialog();
  ui->setupUi(this);

  connect(ui->location, SIGNAL(textChanged(QString)),
          SLOT(locationChanged(QString)));
  
  ui->local->setChecked(true);
  ui->archive->setChecked(false);
}

NewBookDialog::~NewBookDialog() {
}

QString NewBookDialog::location() const {
  return ui->location->text();
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
  QFileInfo fi(ui->location->text());
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
  NewBookDialog nbd;
  while (nbd.exec()) {
    QString fn = nbd.location();
    if (fn.isEmpty()) {
      QMessageBox::warning(&nbd, "eln",
                           "Please specify a location for your new notebook");
      continue;
    }

    if (!fn.endsWith(".nb"))
      fn += ".nb";

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
      if (nbd.isRemote())
        dst = nbd.remoteHost() + ":" + dst;
      Process proc;
      proc.setWorkingDirectory(fn);
      proc.setWindowCaption("Creating archive");
      proc.setNoStartMessage("Could not run git");
      proc.setCommandAndArgs("git", QStringList()
                             << "init" << "--bare" << dst);
      if (!proc.exec()) {
        QMessageBox::critical(&nbd, "eln",
                              "Failed to create archive: " + proc.stderr()
                              + " - Notebook created without archive",
                              QMessageBox::Cancel);
        // I should, instead, remove the just created notebook
        return fn;
      }
      
      proc.setCommandAndArgs("git", QStringList() 
                             << "push" << "--set-upstream" << dst << "master");
      if (!proc.exec()) {
        QMessageBox::critical(&nbd, "eln",
                              "Failed to push to archive: " + proc.stderr()
                              + " - Notebook created without archive",
                              QMessageBox::Cancel);
        // I should, instead, remove the just created notebook
        return fn;
      }
    }

    return fn; // all good!
  }

  return "";
}
      
