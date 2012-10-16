// Notebook.C

#include "Notebook.H"
#include "TOC.H"
#include "ResourceManager.H"
#include "PageFile.H"
#include "TitleData.H"
#include <QDebug>

Notebook::Notebook(QString path) {
  root = QDir(path);
  tocFile_ = TOCFile::load(root.filePath("toc.json"), this);
  bookFile_ = BookFile::load(root.filePath("book.json"), this);
  Q_ASSERT(tocFile_);
  Q_ASSERT(bookFile_);
}

Notebook::~Notebook() {
  flush();
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

  delete TOCFile::create(d.filePath("toc.json"));
  delete BookFile::create(d.filePath("book.json"));
  
  Notebook *nb = new Notebook(d.absolutePath());
  return nb;
}

ResourceManager *Notebook::resMgr(int pgno) {
  if (resMgrs.contains(pgno))
    return resMgrs[pgno];
  ResourceManager *r
    = new ResourceManager(root.absoluteFilePath(QString("res/%1").arg(pgno)),
			  this);
  resMgrs[pgno] = r;
  return r;
}

TOC *Notebook::toc() const {
  return tocFile_->data();
}

bool Notebook::hasPage(int n) const {
  return toc()->contains(n);
}

PageFile *Notebook::page(int n)  {
  PageFile *f = PageFile::load(root.filePath(QString("pages/%1.json").arg(n)),
			       this);
  if (!f)
    return 0;

  f->data()->setNotebook(this);
  connect(f->data(), SIGNAL(titleMod()), SLOT(titleMod()));
  connect(f->data(), SIGNAL(sheetCountMod()), SLOT(sheetCountMod()));
  return f;
}

PageFile *Notebook::createPage(int n) {
  PageFile *f = PageFile::create(root.filePath(QString("pages/%1.json").arg(n)),
				 this);
  if (!f)
    return 0;

  toc()->addEntry(n, TOCEntry()); // details will be filled out later

  f->data()->setNotebook(this);
  connect(f->data(), SIGNAL(titleMod()), SLOT(titleMod()));
  connect(f->data(), SIGNAL(sheetCountMod()), SLOT(sheetCountMod()));
  bookData()->setEndDate(QDate::currentDate());
  return f;
}

void Notebook::titleMod() {
  qDebug() << "Notebook::titleMod";
  PageData *pg = dynamic_cast<PageData *>(sender());
  Q_ASSERT(pg);
  toc()->setTitle(pg->startPage(), pg->title()->current()->text());
}

void Notebook::sheetCountMod() {
  PageData *pg = dynamic_cast<PageData *>(sender());
  Q_ASSERT(pg);
  toc()->setSheetCount(pg->startPage(), pg->sheetCount());
}

BookData *Notebook::bookData() const {
  return bookFile_->data();
}

void Notebook::flush() {
  tocFile_->save(true);
  bookFile_->save(true);
}
