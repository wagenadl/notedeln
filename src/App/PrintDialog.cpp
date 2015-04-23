// PrintDialog.cpp

#include "PrintDialog.h"
#include "ui_PrintDialog.h"
#include <QDebug>
#include <QPrinterInfo>
#include <QDir>
#include <QFileDialog>

PrintDialog::PrintDialog(QWidget *parent): QDialog(parent) {
  ui = new Ui_printDialog();
  ui->setupUi(this);

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
    ui->pPrinter->setChecked(true);
  }

  ui->pFileName->setText(QDir::home().absoluteFilePath("output.pdf"));
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

PrintDialog::Range PrintDialog::tocRange() const {
  if (ui->rtAll->isChecked())
    return All;
  else if (ui->rtCurrent->isChecked())
    return CurrentPage;
  else
    return FromTo;
}
int PrintDialog::tocFrom() const {
  return ui->rtrFrom->value();
}

int PrintDialog::tocTo() const {
  return ui->rtrTo->value();
}

PrintDialog::Range PrintDialog::entriesRange() const {
  if (ui->reAll->isChecked())
    return All;
  else if (ui->reCurrentPage->isChecked())
    return CurrentPage;
  else if (ui->reCurrentEntry->isChecked())
    return CurrentEntry;
  else
    return FromTo;
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

