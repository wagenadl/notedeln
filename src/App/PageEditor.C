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
  pageScene = 0;
}

PageEditor::~PageEditor() {
}

void PageEditor::gotoPage(int n) {
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

  pageScene = new PageScene(file->data(), this);
  pageScene->populate();
  pageScene->makeWritable(); // this should be more sophisticated
  view->setScene(pageScene);
}

void PageEditor::gotoFront() {
  view->setScene(frontScene);
}
