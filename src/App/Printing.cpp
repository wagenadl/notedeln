// App/Printing.cpp - This file is part of eln

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

#define NOHIDETOOLBARS 0

#include "PageView.H"
#include "Notebook.H"
#include "TOC.H"
#include "TOCScene.H"
#include "FrontScene.H"
#include "EntryScene.H"
#include "Assert.H"
#include "Toolbars.H"

#include <QPrinter>
#include <QPrintDialog>
#include <QProgressDialog>
#include <QPainter>
#include <QDebug>

void PageView::printDialog() {
  /* Nonstandard interpretation of range options:
     - Current page: only current sheet of current entry
     - Selection: all sheets of current entry
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
      gotoEntryPage(startPage);
      ASSERT(entryScene);
      printer.newPage();
      entryScene->print(&printer, &p);
    }
    progress.setValue(progress.maximum());
    switch (oldSection) {
    case Front:
      gotoFront();
      break;
    case TOC:
      gotoTOC(oldPage);
      break;
    case Entries:
      gotoEntryPage(oldPage);
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
    case Entries:
      entryScene->print(&printer, &p);
      progress.setValue(progress.maximum());
      gotoEntryPage(oldPage);
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
	gotoEntryPage(startPage);
	ASSERT(entryScene);
	if (!first)
	  printer.newPage();
	first = !entryScene->print(&printer, &p, from-startPage, to-startPage);
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
    case Entries:
      gotoEntryPage(oldPage);
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
		      currentSheet, currentSheet);
      break;
    case Entries:
      entryScene->print(&printer, &p,
			currentSheet, currentSheet);
      break;
    }
    progress.setValue(progress.maximum());
    break;
  }

  show();
  
  return;
}
      
