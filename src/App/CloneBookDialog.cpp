// CloneBookDialog.cpp

#include "CloneBookDialog.h"
#include "Translate.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "ui_CloneBookDialog.h"

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
  return ui->location->text() + ui->leaf->text();
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

  QString leaf = ui->leaf->text();
  if (!leaf.isEmpty() && d.exists(ui->leaf->text().mid(1))) {
    QMessageBox::warning(this, "eln",
                         "'" + fn + leaf + "' already exists.",
                         QMessageBox::Cancel);
    return;
  }
} 

void CloneBookDialog::updateLocation(QString aloc) {
  if (aloc.endsWith(".git"))
    aloc = aloc.left(aloc.size()-4);
  int idx = aloc.lastIndexOf("/");
  if (idx>=0)
    aloc = aloc.mid(idx);
  else
    aloc = "/" + aloc;
  ui->leaf->setText(aloc);
}

CloneBookDialog *CloneBookDialog::getInfo() {
  CloneBookDialog *cbd = new CloneBookDialog();
  while (cbd->exec()) {
    // OK pressed
    if (cbd->archiveLocation().isEmpty()) {
      if (QMessageBox::warning(cbd, "eln",
			       "Please specify an archive location",
			       QMessageBox::Cancel | QMessageBox::Ok)
	  == QMessageBox::Cancel)
	break;
      else
	continue;
    }

    if (!cbd->isLocal()) {
      if (cbd->archiveHost().isEmpty()) {
	if (QMessageBox::warning(cbd, "eln",
				 "Please specify an archive host",
				 QMessageBox::Cancel | QMessageBox::Ok)
	    == QMessageBox::Cancel)
	  break;
	else
	  continue;
      }
    }

    QString path = cbd->cloneLocation();
    int idx = path.lastIndexOf("/");
    if (path.isEmpty() || idx<0) {
      if (QMessageBox::warning(cbd, "eln",
			  "Please specify a location for your cloned notebook",
			       QMessageBox::Cancel | QMessageBox::Ok)
	  == QMessageBox::Cancel)
	break;
      else
	continue;
    }
    
    QDir d(path.left(idx));
    QString leaf(path.mid(idx+1));
    if (d.exists(leaf)) {
      if (QMessageBox::warning(cbd, "eln",
			       "A notebook already exists in that location. Please specify another location for your cloned notebook.",
			       QMessageBox::Cancel | QMessageBox::Ok)
	  == QMessageBox::Cancel)
	break;
      else
	continue;
    }
     
    // Basic checks passed
    cbd->close();
    return cbd;
  }
  // Cancel pressed
  delete cbd;
  return 0;
}

