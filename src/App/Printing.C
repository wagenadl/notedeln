#include "PageView.H"
#include "Notebook.H"
#include "TOC.H"
#include "TOCScene.H"
#include "FrontScene.H"
#include "PageScene.H"
#include "Assert.H"
#include "Toolbars.H"

#include <QPrinter>
#include <QPrintDialog>
#include <QProgressDialog>
#include <QPainter>
#include <QDebug>

void PageView::printDialog() {
  /* Nonstandard interpretation of range options:
     - Current page: only current sheet of current "page"
     - Selection: all sheets of current "page"
     - From-to range: always ignore front matter
     - All: whole book
  */
  QPrinter printer;
  QPrintDialog dialog(&printer, this);
  dialog.setWindowTitle(tr("Print book"));
  dialog.setOption(QAbstractPrintDialog::PrintToFile);
  dialog.setOption(QAbstractPrintDialog::PrintPageRange);
  dialog.setOption(QAbstractPrintDialog::PrintSelection);
  dialog.setOption(QAbstractPrintDialog::PrintShowPageSize);
  dialog.setOption(QAbstractPrintDialog::PrintCurrentPage);

  dialog.setMinMax(1, book->toc()->newPageNumber()-1);
  dialog.setFromTo(1, book->toc()->newPageNumber()-1);
  
  if (dialog.exec() != QDialog::Accepted) 
    return;

  if (toolbars)
    toolbars->hide();
  hide();
  
  QProgressDialog progress("Printing...", "Abort", 0, 1000, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(1);
  
  QPainter p;
  p.begin(&printer);

  int oldPage = currentPage;
  int oldSection = currentSection;
  
  int tocCount = tocScene->sheetCount();
  int pgsCount = book->toc()->newPageNumber()-1;
  
  switch (printer.printRange()) {
  case QAbstractPrintDialog::AllPages:
    progress.setMaximum(1+tocCount+pgsCount);
    frontScene->print(&printer, &p);
    progress.setValue(1);
    if (!progress.wasCanceled()) {
      printer.newPage();
      tocScene->print(&printer, &p);
    }
    foreach (int startPage, book->toc()->entries().keys()) {
      progress.setValue(1+tocCount+startPage);
      if (progress.wasCanceled())
        break;
      gotoPage(startPage);
      ASSERT(pageScene);
      printer.newPage();
      pageScene->print(&printer, &p);
    }
    progress.setValue(progress.maximum());
    switch (oldSection) {
    case Front:
      gotoFront();
      break;
    case TOC:
      gotoTOC(oldPage);
      break;
    case Pages:
      gotoPage(oldPage);
      break;
    }
    break;
  case QAbstractPrintDialog::Selection:
    switch (currentSection) {
    case Front:
      frontScene->print(&printer, &p);
      progress.setValue(progress.maximum());
      break;
    case TOC:
      tocScene->print(&printer, &p);
      progress.setValue(progress.maximum());
      gotoTOC(oldPage);
      break;
    case Pages:
      pageScene->print(&printer, &p);
      progress.setValue(progress.maximum());
      gotoPage(oldPage);
      break;
    }
    break;
  case QAbstractPrintDialog::PageRange: {
    int from = printer.fromPage();
    int to = printer.toPage();
    progress.setMaximum(to);
    progress.setMinimum(from);
    bool first = true;
    foreach (int startPage, book->toc()->entries().keys()) {
      progress.setValue(startPage);
      if (progress.wasCanceled())
        break;
      if (to>=startPage &&
	  from<startPage+book->toc()->entry(startPage)->sheetCount()) {
	gotoPage(startPage);
	ASSERT(pageScene);
	if (!first)
	  printer.newPage();
	first = !pageScene->print(&printer, &p, from-startPage, to-startPage);
      }
    }
    progress.setValue(progress.maximum());
    switch (oldSection) {
    case Front:
      gotoFront();
      break;
    case TOC:
      gotoTOC(oldPage);
      break;
    case Pages:
      gotoPage(oldPage);
      break;
    }
  } break;
  case QAbstractPrintDialog::CurrentPage:
    switch (currentSection) {
    case Front:
      frontScene->print(&printer, &p);
      break;
    case TOC:
      tocScene->print(&printer, &p,
		      tocScene->currentSheet(),
		      tocScene->currentSheet());
      break;
    case Pages:
      pageScene->print(&printer, &p,
		       tocScene->currentSheet(),
		       tocScene->currentSheet());
      break;
    }
    progress.setValue(progress.maximum());
    break;
  }

  if (toolbars)
    toolbars->show();
  show();
  
  return;
}
      
