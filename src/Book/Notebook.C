// Notebook.C

#include "Notebook.H"
#include "TOC.H"
#include "PageFile.H"
#include "TitleData.H"
#include "Style.H"
#include "Assert.H"

#include <QDebug>

Notebook::Notebook(QString path) {
  root = QDir(path);
  tocFile_ = TOCFile::load(root.filePath("toc.json"), this);
  tocFile_->data()->setBook(this);
  bookFile_ = BookFile::load(root.filePath("book.json"), this);
  ASSERT(tocFile_);
  ASSERT(bookFile_);
  style_ = new Style(root.filePath("style.json"));
}

Notebook::~Notebook() {
  flush();
}

Style const &Notebook::style() const {
  return *style_;
}

Notebook *Notebook::load(QString path) {
  QDir d(path);
  if (d.exists())
    return new Notebook(d.absolutePath());
  else
    return 0;
}

QString Notebook::filePath(QString f) const {
  return root.filePath(f);
}

Notebook *Notebook::create(QString path) {
  QDir d(path);
  if (d.exists()) {
    qDebug() << "Notebook: Cannot create new notebook at existing path" << path;
    return 0;
  }

  if (!d.mkpath("pages")) {
    qDebug() << "Notebook: Failed to create 'pages' directory at " << path;
    return 0;
  }
  
  if (!d.mkpath("res")) {
    qDebug() << "Notebook: Failed to create 'res' directory at " << path;
    return 0;
  }

  delete TOCFile::create(d.filePath("toc.json"));
  delete BookFile::create(d.filePath("book.json"));

  QFile styleIn(":/style.json");
  ASSERT(styleIn.open(QFile::ReadOnly));
  QFile styleOut(d.filePath("style.json"));
  if (!styleOut.open(QFile::WriteOnly)) {
    qDebug() << "Notebook: Failed to create 'style.json' at " << path;
    return 0;
  }
  styleOut.write(styleIn.readAll());
  styleIn.close();
  styleOut.close();

  QFile jpegIn(":/front.jpg");
  ASSERT(jpegIn.open(QFile::ReadOnly));
  QFile jpegOut(d.filePath("front.jpg"));
  if (!jpegOut.open(QFile::WriteOnly)) {
    qDebug() << "Notebook: Failed to create 'front.jpg' at " << path;
    return 0;
  }
  jpegOut.write(jpegIn.readAll());
  jpegIn.close();
  jpegOut.close();

  Notebook *nb = new Notebook(d.absolutePath());
  return nb;
}

TOC *Notebook::toc() const {
  return tocFile_->data();
}

bool Notebook::hasPage(int n) const {
  return toc()->contains(n);
}

PageFile *Notebook::page(int n)  {
  if (pgFiles.contains(n))
    return pgFiles[n];
  PageFile *f = loadPage(QDir(root.filePath("pages")), n, this);
  ASSERT(f);
  pgFiles[n] = f;

  f->data()->setBook(this);
  connect(f->data(), SIGNAL(titleMod()), SLOT(titleMod()));
  connect(f->data(), SIGNAL(sheetCountMod()), SLOT(sheetCountMod()));
  return f;
}

PageFile *Notebook::createPage(int n) {
  ASSERT(!pgFiles.contains(n));
  PageFile *f = ::createPage(root.filePath("pages"), n, this);
  if (!f)
    return 0;
  pgFiles[n] = f;
  f->data()->setStartPage(n);
  toc()->addEntry(f->data());

  f->data()->setBook(this);
  connect(f->data(), SIGNAL(titleMod()), SLOT(titleMod()));
  connect(f->data(), SIGNAL(sheetCountMod()), SLOT(sheetCountMod()));
  bookData()->setEndDate(QDate::currentDate());
  return f;
}

bool Notebook::deletePage(int pgno) {
  PageFile *pf = page(pgno);
  if (!pf) {
    qDebug() << "Notebook: cannot delete nonexistent page";
    return false;
  }
  ASSERT(pf->data());
  if (!pf->data()->isEmpty()) {
    qDebug() << "Notebook: refusing to delete non-empty page";
    return false;
  }

  pgFiles[pgno]->cancelSave();
  delete pgFiles[pgno];
  pgFiles.remove(pgno);

  ASSERT(toc()->deleteEntry(toc()->find(pgno)));
  QString fn = QString("pages/%1.json").arg(pgno);
  QString fn0 = fn + "~";
  root.remove(fn0);
  ASSERT(root.rename(fn, fn0));
  return true;
}

void Notebook::titleMod() {
  PageData *pg = dynamic_cast<PageData *>(sender());
  ASSERT(pg);
  TOCEntry *e = toc()->entry(pg->startPage());
  ASSERT(e);
  e->setTitle(pg->title()->current()->text());
}

void Notebook::sheetCountMod() {
  PageData *pg = dynamic_cast<PageData *>(sender());
  ASSERT(pg);
  TOCEntry *e = toc()->entry(pg->startPage());
  ASSERT(e);
  e->setSheetCount(pg->sheetCount());
}

BookData *Notebook::bookData() const {
  return bookFile_->data();
}

void Notebook::flush() {
  bool actv = false;
  bool ok = true;
  if (tocFile_->needToSave()) {
    actv = true;
    ok = ok && tocFile_->saveNow();
  }
  if (bookFile_->needToSave()) {
    actv = true;
    ok = ok && bookFile_->saveNow();
  }
  foreach (PageFile *pf, pgFiles) {
    if (pf->needToSave()) {
      actv = true;
      ok = ok && pf->saveNow();
    }
  }
  if (actv) {
    if (ok)
      qDebug() << "Notebook flushed to disk";
    else
      qDebug() << "Notebook flushed, with errors";
  } else {
    qDebug() << "(No changes to save)";
  }
}
