// NewBookDialog.cpp

#include "NewBookDialog.h"
#include "ui_NewBookDialog.h"

NewBookDialog::NewBookDialog(QWidget *parent): QDialog(parent) {
  ui = new Ui_newBookDialog();
  ui->setupUi(this);

  connect(ui->local, SIGNAL(toggled(bool)), SLOT(localToggled(bool)));
  connect(ui->remote, SIGNAL(toggled(bool)), SLOT(remoteToggled(bool)));
  connect(ui->location, SIGNAL(textChanged(QString)),
          SLOT(locationChanged(QString)));
  
  ui->local->setChecked(true);
  ui->archiving->setChecked(false);
}

NewBookDialog::~NewBookDialog() {
}

QString NewBookDialog::location() const {
  return ui->location->text();
}

bool NewBookDialog::hasArchive() const {
  return ui->archiving->isChecked();
}
bool NewBookDialog::isRemote() const {
  return ui->remote->isChecked();
}

QString NewBookDialog::remoteHost() const {
  return ui->host->currentText();
}

QString NewBookDialog::archiveLocation() const {
  return ui->archiveLocation->text();
}

void NewBookDialog::browse() {
}

void NewBookDialog::abrowse() {
}

void NewBookDialog::localToggled(bool) {
}

void NewBookDialog::remoteToggled(bool) {
}

void NewBookDialog::locationChanged(QString) {
}
