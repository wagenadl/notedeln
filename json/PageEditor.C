// PageEditor.C

#include "PageEditor.H"

#include "PageView.H"
#include "PageScene.H"
#include "Toolbar.H"
#include "PageFile.H"

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

  file_ = 0;
  scene_ = 0;
}

PageEditor::~PageEditor() {
}

void PageEditor::load(QString fn) {
  if (scene_)
    delete scene_;
  if (file_)
    delete file_;

  file_ = new PageFile(fn, this);
  scene_ = new PageScene(file_->data(), this);  
  view_->setScene(scene_);
  resize(sizeHint());
}

void PageEditor::create(QString fn) {
  if (scene_)
    delete scene_;
  if (file_)
    delete file_;

  file_ = PageFile::create(fn);
  scene_ = new PageScene(file_->data(), this);  
  view_->setScene(scene_);
  resize(sizeHint());
}
