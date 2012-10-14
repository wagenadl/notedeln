// Notebook.C

#include "Notebook.H"
#include "TOC.H"
#include "ResourceManager.H"
#include "PageFile.H"
#include "TitleData.H"

Notebook::Notebook(QString path) {
  root = QDir(path);
  toc_ = new TOC(root.filePath("toc.json"), this);
  resMgr_ = new ResourceManager(root.filePath("res"), this);
  bookFile_ = BookFile::load(root.filePath("book.json"), this);
  Q_ASSERT(bookFile_);
}

Notebook *Notebook::load(QString path) {
  QDir d(path);
  if (d.exists())
    return new Notebook(d.absolutePath());
  else
    return 0;
}

Notebook *Notebook::create(QString path) {
  QDir d(path);
  if (d.exists()) 
    return 0;

  if (!d.mkpath("pages")) 
    return 0;
  
  if (!d.mkpath("res"))
    return 0;

  delete TOC::create(d.filePath("toc.json"));
  delete BookFile::create(d.filePath("book.json"));
  
  Notebook *nb = new Notebook(d.absolutePath());
  return nb;
}

ResourceManager *Notebook::resMgr() const {
  return resMgr_;
}

TOC *Notebook::toc() const {
  return toc_;
}

bool Notebook::hasPage(int n) const {
  return toc_->contains(n);
}

PageFile *Notebook::page(int n)  {
  return PageFile::load(root.filePath(QString("pages/%1.json").arg(n)),
			this);
}

PageFile *Notebook::createPage(int n) {
  PageFile *f = PageFile::create(root.filePath(QString("pages/%1.json").arg(n)),
				 this);
  if (!f)
    return 0;
  toc_->addEntry(n, TOCEntry()); // details will be filled out later
  connect(f->data(), SIGNAL(titleMod()), SLOT(titleMod()));
  connect(f->data(), SIGNAL(sheetcountMod()), SLOT(sheetCountMod()));
  bookData()->setEndDate(QDate::currentDate());
  return f;
}

void Notebook::titleMod() {
  PageData *pg = dynamic_cast<PageData *>(sender());
  Q_ASSERT(pg);
  toc_->setTitle(pg->startPage(), pg->title()->current()->text());
}

void Notebook::sheetCountMod() {
  PageData *pg = dynamic_cast<PageData *>(sender());
  Q_ASSERT(pg);
  toc_->setSheetCount(pg->startPage(), pg->sheetCount());
}

BookData *Notebook::bookData() const {
  return bookFile_->data();
}

