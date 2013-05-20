// PageEditor.C

#include "PageEditor.H"
#include "PageView.H"

PageEditor::PageEditor(Notebook *nb): book(nb) {
  view = new PageView(nb, this);
  setCentralWidget(view);
  view->gotoFront();  
}

PageEditor::~PageEditor() {
}
