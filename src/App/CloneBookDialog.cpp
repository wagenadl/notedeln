// CloneBookDialog.cpp

#include "CloneBookDialog.h"
#include "Translate.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "ui_CloneBookDialog.h"
#include "Process.h"

CloneBookDialog::CloneBookDialog(QWidget *parent): QDialog(parent) {
  ui = new Ui_cloneBookDialog;
  ui->setupUi(this);
  ui->leaf->setText("");
}

CloneBookDialog::~CloneBookDialog() {
}
 
QString CloneBookDialog::archiveLocation() const {
  return ui->alocation->text();
}

QString CloneBookDialog::archiveHost() const {
  return ui->host->currentText();
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
    QMessageBox::warning(this, "eln",
                         "Browsing is only supported for local archives.",
                         QMessageBox::Cancel);
    return;
  }
  
  QFileDialog qfd(this);
  qfd.setWindowTitle(Translate::_("title-clone-archive-location"));
  qfd.setFileMode(QFileDialog::Directory);
  qfd.setOptions(QFileDialog::ShowDirsOnly);
  if (!qfd.exec()) 
    return;

  QStringList fns = qfd.selectedFiles();
  if (fns.isEmpty())
    return;
  if (fns.size()>1)
    qDebug() << "Multiple files selected; using only the first.";

  QString fn = fns[0];
  QDir d(fn);
  if (!d.exists()) {
    QMessageBox::warning(this, "eln",
                         "'" + fn + "' does not exist.",
                         QMessageBox::Cancel);
    return;
  }
  if (!d.exists("branches") || !d.exists("objects")) {
    QMessageBox::warning(this, "eln",
                         "'" + fn + "' is not a git archive.",
                         QMessageBox::Cancel);
    return;
  }

  ui->alocation->setText(fn);
}

void CloneBookDialog::browse() {
  QFileDialog qfd(this);
  qfd.setWindowTitle(Translate::_("title-clone-archive-location"));
  qfd.setFileMode(QFileDialog::Directory);
  qfd.setOptions(QFileDialog::ShowDirsOnly);
  if (!qfd.exec()) 
    return;

  QStringList fns = qfd.selectedFiles();
  if (fns.isEmpty())
    return;
  if (fns.size()>1)
    qDebug() << "Multiple files selected; using only the first.";

  QString fn = fns[0];
  QDir d(fn);
  if (!d.exists()) {
    QMessageBox::warning(this, "eln",
                         "'" + fn + "' does not exist.",
                         QMessageBox::Cancel);
    return;
  }

  QString l = leaf();
  if (!l.isEmpty() && d.exists(l)) {
    QMessageBox::warning(this, "eln",
                         "'" + d.absoluteFilePath(l) + "' already exists.",
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
      QMessageBox::warning(&cbd, "eln", Translate::_("no-alocation"));
      continue;
    }

    if (!cbd.isLocal()) {
      if (cbd.archiveHost().isEmpty()) {
	QMessageBox::warning(&cbd, "eln", Translate::_("no-host"));
        continue;
      }
    }

    QString path = cbd.cloneDestination();
    if (path.isEmpty()) {
      QMessageBox::warning(&cbd, "eln", Translate::_("no-clone"));
      continue;
    }
    
    QDir d(path);
    QString l(cbd.leaf());
    if (d.exists(l)) {
      QMessageBox::warning(&cbd, "eln", Translate::_("exists-clone"));
      continue;
    }
     
    // Basic checks passed
    d.mkpath(l);

    // Run git clone
    Process proc;
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
    
    QMessageBox::warning(&cbd, "eln", Translate::_("clone-failed")
                         + ": " + proc.stderr());
    // we should remove any partial download
    return "";
  }
  // Cancel pressed
  return "";
}

