// App/PrintDialog.cpp - This file is part of eln

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

// PrintDialog.cpp

#include "PrintDialog.h"
#include "ui_PrintDialog.h"
#include <QDebug>
#include <QPrinterInfo>
#include <QDir>
#include <QFileDialog>
#include "DefaultLocation.h"

PrintDialog::PrintDialog(QWidget *parent): QDialog(parent) {
  ui = new Ui_printDialog();
  ui->setupUi(this);
  resize(sizeHint());

  connect(ui->pPrinter, SIGNAL(toggled(bool)), SLOT(setDestination()));
  connect(ui->pPrinterName, SIGNAL(currentIndexChanged(int)),
          SLOT(setDestination()));
  connect(ui->pFile, SIGNAL(toggled(bool)), SLOT(setDestination()));
  connect(ui->rtRange, SIGNAL(toggled(bool)), SLOT(toggleTocRange(bool)));
  connect(ui->reRange, SIGNAL(toggled(bool)), SLOT(toggleEntriesRange(bool)));
  connect(ui->pBrowse, SIGNAL(clicked(bool)), SLOT(browse()));
  
  QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
  if (printers.isEmpty()) {
    ui->pPrinter->setEnabled(false);
    ui->pPrinterName->setEnabled(false);
    ui->pFile->setChecked(true);
  } else {
    foreach (QPrinterInfo const &p, printers)
      ui->pPrinterName->addItem(p.printerName());
  }

  ui->pFileName->setText(defaultLocation() + "/output.pdf");
}

PrintDialog::~PrintDialog() {
}

void PrintDialog::setDestination() {
  bool isPrinter = ui->pPrinter->isChecked();
  ui->oDuplex->setEnabled(isPrinter);
  ui->pPrinterName->setEnabled(isPrinter);
  ui->pFileName->setEnabled(!isPrinter);
  ui->pBrowse->setEnabled(!isPrinter);
}

void PrintDialog::toggleTocRange(bool t) {
  ui->rtrFrom->setEnabled(t);
  ui->rtrTo->setEnabled(t);
}

void PrintDialog::toggleEntriesRange(bool t) {
  ui->rerFrom->setEnabled(t);
  ui->rerTo->setEnabled(t);
}

void PrintDialog::browse() {
  QString oldfn = ui->pFileName->text();
  QString dir;
  if (oldfn.contains("/"))
    dir = oldfn.left(oldfn.lastIndexOf("/"));

  QString fn = QFileDialog::getSaveFileName(NULL, "Output filename",
                                            dir, "*.pdf");
  if (fn.isEmpty())
    return;
  
  if (!fn.endsWith(".pdf"))
    fn += ".pdf";
  ui->pFileName->setText(fn);
}

bool PrintDialog::toFile() const {
  return ui->pFile->isChecked();
}

bool PrintDialog::openFileAfter() const {
  return ui->pOpenPDF->isChecked();
}

QString PrintDialog::filename() const {
  return ui->pFileName->text();
}

QString PrintDialog::printername() const {
  return ui->pPrinterName->currentText();
}

bool PrintDialog::isDuplex() const {
  return ui->oDuplex->isChecked();
}

bool PrintDialog::printFrontPage() const {
  return ui->rFrontPage->isChecked();
}

bool PrintDialog::printTOC() const {
  return ui->rTOC->isChecked();
}

bool PrintDialog::printEntries() const {
  return ui->rEntries->isChecked();
}

PrintDialog::TOCRange PrintDialog::tocRange() const {
  if (ui->rtAll->isChecked())
    return TOCRange::All;
  else if (ui->rtCurrent->isChecked())
    return TOCRange::CurrentPage;
  else
    return TOCRange::FromTo;
}
int PrintDialog::tocFrom() const {
  return ui->rtrFrom->value();
}

int PrintDialog::tocTo() const {
  return ui->rtrTo->value();
}

PrintDialog::Range PrintDialog::entriesRange() const {
  if (ui->reAll->isChecked())
    return Range::All;
  else if (ui->reCurrentPage->isChecked())
    return Range::CurrentPage;
  else if (ui->reCurrentEntry->isChecked())
    return Range::CurrentEntry;
  else if (ui->reRange->isChecked())
    return Range::FromTo;
  else // ui->reSearchResults->isChecked
    return Range::SearchResults;
}

int PrintDialog::entriesFrom() const {
  return ui->rerFrom->value();
}

int PrintDialog::entriesTo() const {
  return ui->rerTo->value();
}

void PrintDialog::setMaxPage(int n) {
  ui->rerFrom->setMaximum(n);
  ui->rerTo->setMaximum(n);
}

void PrintDialog::setMaxTOCPage(int n) {
  ui->rtrFrom->setMaximum(n);
  ui->rtrTo->setMaximum(n);
}

