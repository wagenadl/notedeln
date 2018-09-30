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

#include "SceneBank.h"
#include "PageView.h"
#include "Notebook.h"
#include "TOC.h"
#include "TOCScene.h"
#include "FrontScene.h"
#include "EntryScene.h"
#include "Assert.h"
#include "Toolbars.h"
#include "PrintDialog.h"

#include <QUrl>
#include <QPrinter>
#include <QProgressDialog>
#include <QPainter>
#include <QDesktopServices>
#include <QDebug>
#include "SearchResultScene.h"

static QSet<int> startPagesOfSearchResults() {
  QSet<int> pp;
  for (QList<SearchResult> const &lst: SearchResultScene::allOpenSearches())
    for (auto const &sr: lst) 
      pp.insert(sr.startPageOfEntry);
  return pp;
}

void PageView::openPrintDialog() {
  QPrinter printer;
  PrintDialog dialog(this);
  dialog.setWindowTitle(tr("Print book"));
  dialog.setMaxTOCPage(bank->tocScene()->sheetCount());
  dialog.setMaxPage(book->toc()->newPageNumber()-1);
  
  if (dialog.exec() != QDialog::Accepted) 
    return;

  hide();

  if (dialog.toFile()) {
    printer.setOutputFileName(dialog.filename());
  } else {
    printer.setOutputFileName(QString());
    printer.setPrinterName(dialog.printername());
    printer.setDuplex(dialog.isDuplex()
                      ? QPrinter::DuplexAuto : QPrinter::DuplexNone);
  }
  
  QProgressDialog progress("Printing...", "Abort", 0, 1000, this);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(1);
  
  QPainter p;
  p.begin(&printer);

  int oldPage = currentPage;
  int oldSection = currentSection;

  int nfront = dialog.printFrontPage() ? 1 : 0;
  int ntoc = 0; // number of PAGES from TOC
  if (dialog.printTOC()) {
    switch (dialog.tocRange()) {
    case PrintDialog::TOCRange::All:
      ntoc = bank->tocScene()->sheetCount();
      break;
    case PrintDialog::TOCRange::CurrentPage:
      ntoc = (oldSection==TOC) ? 1 : 0;
      break;
    case PrintDialog::TOCRange::FromTo:
      ntoc = 1 + dialog.tocTo() - dialog.tocFrom();
      break;
    }
  }
  int nentries = 0; // number of PAGES from entries
  if (dialog.printEntries()) {
    switch (dialog.entriesRange()) {
    case PrintDialog::Range::All:
      nentries = book->toc()->newPageNumber()-1;
      break;
    case PrintDialog::Range::CurrentPage:
      nentries = (oldSection==Entries) ? 1 : 0;
      break;
    case PrintDialog::Range::CurrentEntry:
      nentries = (oldSection==Entries) ? entryScene->sheetCount() : 0;
      break;
    case PrintDialog::Range::FromTo:
      nentries = 1 + dialog.entriesTo() - dialog.entriesFrom();
      break;
    case PrintDialog::Range::SearchResults: {
      QSet<int> pgs = startPagesOfSearchResults();
      nentries = 0;
      for (int p: pgs) 
	nentries += book->toc()->tocEntry(p)->sheetCount();
    } break;
    }
  }

  progress.setMaximum(nfront + ntoc + nentries);
  progress.setValue(0);
  bool any = false;
  try {
    if (nfront) {
      bank->frontScene()->print(&printer, &p);
      progress.setValue(progress.value()+nfront);
      any = true;
    }
    if (progress.wasCanceled())
      throw 0;
    if (ntoc) {
      if (any)
	printer.newPage();
      switch (dialog.tocRange()) {
      case PrintDialog::TOCRange::All:
	bank->tocScene()->print(&printer, &p);
	break;
      case PrintDialog::TOCRange::CurrentPage:
	bank->tocScene()->print(&printer, &p,
				currentSheet, currentSheet);
	break;
      case PrintDialog::TOCRange::FromTo:
	bank->tocScene()->print(&printer, &p,
				dialog.tocFrom()-1, dialog.tocTo()-1);
	break;
      }
      progress.setValue(progress.value()+ntoc);
      any = true;
    }

    if (progress.wasCanceled())
      throw 0;
    
    if (nentries) {
      switch (dialog.entriesRange()) {
      case PrintDialog::Range::All:
	foreach (int startPage, book->toc()->entries().keys()) {
	  progress.setValue(nfront + ntoc + startPage);
	  if (progress.wasCanceled())
	    throw 0;
	  gotoEntryPage(startPage);
	  ASSERT(entryScene);
	  if (any)
	    printer.newPage();
	  entryScene->print(&printer, &p);
	  any = true;
	}
	break;
      case PrintDialog::Range::CurrentPage:
	if (any)
	  printer.newPage();
	entryScene->print(&printer, &p, currentSheet, currentSheet);
	progress.setValue(nfront + ntoc + nentries);
	any = true;
	break;
      case PrintDialog::Range::CurrentEntry:
	if (any)
	  printer.newPage();
	entryScene->print(&printer, &p);
	break;
      case PrintDialog::Range::FromTo: {
	int from = dialog.entriesFrom();
	int to = dialog.entriesTo();
	foreach (int startPage, book->toc()->entries().keys()) {
	  if (progress.wasCanceled())
	    throw 0;
	  if (to>=startPage &&
	      from<startPage+book->toc()->tocEntry(startPage)->sheetCount()) {
	    gotoEntryPage(startPage);
	    ASSERT(entryScene);
	    if (any)
	      printer.newPage();
	    any = entryScene->print(&printer, &p, from-startPage, to-startPage);
	    progress.setValue(progress.value() + entryScene->sheetCount());
	  }
	}
      } break;
      case PrintDialog::Range::SearchResults: {
	qDebug() << "Printing search results NYI";
	break;
      }
      }
      any = true;
      progress.setValue(nfront + ntoc + nentries);
    }
  } catch (int) {
    // abort
  }

  progress.setValue(progress.maximum());

  if (dialog.toFile() && dialog.openFileAfter()) 
    QDesktopServices::openUrl(QUrl(dialog.filename()));

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

  show();
}
      
