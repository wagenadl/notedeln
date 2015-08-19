// NewBookDialog.cpp

#include "NewBookDialog.h"
#include "ui_NewBookDialog.h"
#include <QFileInfo>
#include <QFileDialog>
#include "Translate.h"

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
}
