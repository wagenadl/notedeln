// PageEditor.C

#include "PageEditor.H"

#include "PageView.H"
#include "PageScene.H"
#include "Notebook.H"
#include "PageFile.H"
#include "DataFile.H"
#include "TOCScene.H"
#include "FrontScene.H"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QCloseEvent>

PageEditor::PageEditor(Notebook *nb): book(nb) {
  view = new PageView(this);
  setCentralWidget(view);
  frontScene = new FrontScene(nb, this);
  tocScene = new TOCScene(nb->toc(), this);
  tocScene->populate();
  pageScene = 0;

  connect(view, SIGNAL(pgUp()), SLOT(previousPage()));
  connect(view, SIGNAL(pgDn()), SLOT(nextPage()));
  connect(view, SIGNAL(home()), SLOT(gotoTOC()));
  connect(view, SIGNAL(end()), SLOT(lastPage()));
}

PageEditor::~PageEditor() {
}

void PageEditor::gotoPage(int n) {
  qDebug() << "PE:page" << n;
  if (n<1)
    n=1;

  if (n>=book->toc()->newPageNumber()) {
    // make a new page?
    TOCEntry *te = book->toc()->find(n-1);
    if (te) {
      // let's look at page before end
      PageFile *file = book->page(te->startPage());
      Q_ASSERT(file);
      if (file->data()->isEmpty()) {
	// previous page is empty -> go there instead
	gotoPage(n-1);
	return;
      }
    }
    qDebug() << " -> creating page" << n;
    book->createPage(n);
  }
  
  TOCEntry *te = book->toc()->find(n);
  if (!te) {
    qDebug() << "PageEditor: gotoPage("<<n<<"): no such page";
    return;
  }

  PageFile *file = book->page(te->startPage());
  Q_ASSERT(file);
  Q_ASSERT(file->data());
  
  if (pageScene)
    delete pageScene;

  currentSection = Pages;
  currentPage = n;

  pageScene = new PageScene(file->data(), this);
  pageScene->populate();
  pageScene->makeWritable(); // this should be more sophisticated
  view->setScene(pageScene);
}

void PageEditor::gotoFront() {
  qDebug() << "PE:front";
  currentSection = Front;
  view->setScene(frontScene);
}

void PageEditor::gotoTOC(int n) {
  qDebug() << "PE:toc" << n;
  currentSection = TOC;
  currentPage = n;
  view->setScene(tocScene);
  tocScene->gotoSheet(currentPage-1);
}

void PageEditor::previousPage() {
  qDebug() << "PE:pp" << currentSection << currentPage;
  switch (currentSection) {
  case Front:
    break;
  case TOC:
    if (currentPage<=1)
      gotoFront();
    else
      gotoTOC(currentPage-1);
    break;
  case Pages:
    if (currentPage<=1)
      gotoTOC(tocScene->sheetCount());
    else
      gotoPage(currentPage-1);
    break;
  }
}

void PageEditor::nextPage() {
  qDebug() << "PE:np" << currentSection << currentPage;
  switch (currentSection) {
  case Front:
    gotoTOC();
    break;
  case TOC:
    if (currentPage>=tocScene->sheetCount())
      gotoPage(1);
    else
      gotoTOC(currentPage+1);
    break;
  case Pages:
    if (currentPage>=book->toc()->newPageNumber()) {
      // go to index?
    } else if (currentPage==book->toc()->newPageNumber()-1) {
      gotoPage(currentPage+1);
      // makes a new page unless current is empty
    } else {
      gotoPage(currentPage+1);
    }
    break;
  }
}

void PageEditor::lastPage() {
  qDebug() << "PE:lp" << currentSection << currentPage;
  gotoPage(book->toc()->newPageNumber()-1);
}
