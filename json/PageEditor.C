// PageEditor.C

#include "PageEditor.H"

#include "PageView.H"
#include "PageScene.H"
#include "Toolbar.H"

#include <QApplication>
#include <QDesktopWidget>

PageEditor::PageEditor() {
  view_ = new PageView(this);
  setCentralWidget(view_);

  toolbar_ = new Toolbar(this);

  QRect desk = QApplication::desktop()->availableGeometry();
  bool vert = desk.height() > desk.width();

  if (vert) {
    addToolBar(Qt::TopToolBarArea, toolbar_);
  } else {
    toolbar_->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, toolbar_);
  }
      
  scene_ = new PageScene(0, this);
  view_->setScene(scene_);

  resize(sizeHint());
}

PageEditor::~PageEditor() {
}

void PageEditor::load(QString) {
}

void PageEditor::create(QString) {
}
