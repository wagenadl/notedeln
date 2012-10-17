// PageEditor.C

#include "PageEditor.H"

#include "PageView.H"
#include "PageScene.H"
#include "Toolbar.H"
#include "DataFile.H"

#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QCloseEvent>

PageEditor::PageEditor() {
  view_ = new PageView(this);
  setCentralWidget(view_);
  toolbar_ = 0;
  
  /*
  toolbar_ = new Toolbar(this);

  QRect desk = QApplication::desktop()->availableGeometry();
  bool vert = desk.height() > desk.width();

  if (vert) {
    addToolBar(Qt::TopToolBarArea, toolbar_);
  } else {
    toolbar_->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, toolbar_);
  }
  */

  file_ = 0;
  scene_ = 0;
}

PageEditor::~PageEditor() {
}

void PageEditor::load(QString fn) {
  if (scene_)
    delete scene_;
  if (file_ && file_->parent()==this)
    delete file_;

  file_ = PageFile::load(fn);
  Q_ASSERT(file_);
  Q_ASSERT(file_->data());
  file_->setParent(this);
  scene_ = new PageScene(file_->data(), this);  
  view_->setScene(scene_);
  resize(sizeHint());
}

void PageEditor::open(PageFile *f) {
  if (scene_)
    delete scene_;
  if (file_ && file_->parent()==this) {
    delete file_;
  }
  
  Q_ASSERT(f);
  Q_ASSERT(f->data());
  file_ = f;

  scene_ = new PageScene(file_->data(), this);  
  view_->setScene(scene_);
  resize(sizeHint());
}

void PageEditor::create(QString fn) {
  if (scene_)
    delete scene_;
  if (file_ && file_->parent()==this)
    delete file_;

  file_ = PageFile::create(fn, this);
  if (!file_ || !file_->ok()) {
    qDebug() << "PageEditor: failed to load " << fn;
    return;
  }
  scene_ = new PageScene(file_->data(), this);  
  view_->setScene(scene_);
  resize(sizeHint());
}

void PageEditor::closeEvent(QCloseEvent *e) {
  if (file_)
    file_->save(true);
  e->accept();
}
