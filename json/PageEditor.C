// PageEditor.C

#include "PageEditor.H"

#include <QGraphicsView>
#include "PageScene.H"

PageEditor::PageEditor() {
  view_ = new QGraphicsView(this);
  setCentralWidget(view_);

  scene_ = new PageScene(0, this);

  view_->setScene(scene_);
}

PageEditor::~PageEditor() {
}

void PageEditor::load(QString) {
}

void PageEditor::create(QString) {
}
