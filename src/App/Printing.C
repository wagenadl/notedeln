#include "PageView.H"
#include "Notebook.H"
#include "TOC.H"
#include "TOCScene.H"
#include "FrontScene.H"
#include "PageScene.H"

#include <QPrinter>
#include <QPrintDialog>
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
  
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  QPainter p;
  p.begin(&printer);

  int oldPage = currentPage;
  int oldSection = currentSection;
  
  switch (printer.printRange()) {
  case QAbstractPrintDialog::AllPages:
    frontScene->print(&printer, &p);
    printer.newPage();
    tocScene->print(&printer, &p);
    foreach (int startPage, book->toc()->entries().keys()) {
      gotoPage(startPage);
      Q_ASSERT(pageScene);
      printer.newPage();
      pageScene->print(&printer, &p);
    }
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
      break;
    case TOC:
      tocScene->print(&printer, &p);
      gotoTOC(oldPage);
      break;
    case Pages:
      pageScene->print(&printer, &p);
      gotoPage(oldPage);
      break;
    }
    break;
  case QAbstractPrintDialog::PageRange: {
    int from = printer.fromPage();
    int to = printer.toPage();
    bool first = true;
    foreach (int startPage, book->toc()->entries().keys()) {
      if (to>=startPage &&
	  from<startPage+book->toc()->entry(startPage)->sheetCount()) {
	gotoPage(startPage);
	Q_ASSERT(pageScene);
	if (!first)
	  printer.newPage();
	first = !pageScene->print(&printer, &p, from-startPage, to-startPage);
      }
    }
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
    break;
  }

  return;
}
      
