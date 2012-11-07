// PageEditor.C

#include "PageEditor.H"

#include "PageView.H"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QCloseEvent>

PageEditor::PageEditor(Notebook *nb): book(nb) {
  view = new PageView(nb, this);
  setCentralWidget(view);
  view->gotoFront();  
}

PageEditor::~PageEditor() {
}
