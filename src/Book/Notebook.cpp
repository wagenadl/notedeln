// Notebook.C

#include "Notebook.H"
#include "TOC.H"
#include "EntryFile.H"
#include "TitleData.H"
#include "Style.H"
#include "Assert.H"
#include "Mode.H"
#include "RecentBooks.H"
#include "VersionControl.H"

#include <QDebug>

#define COMMIT_IVAL_S 600 // if vc, commit every once in a while

Notebook::Notebook(QString path) {
  uncommitted = false;
  lastCommit = QDateTime::currentDateTime();
  root = QDir(path);
  Style s0(root.filePath("style.json"));
  if (s0.contains("vc"))
    VersionControl::update(root.path(), s0.string("vc"));
  tocFile_ = TOCFile::load(root.filePath("toc.json"), this);
  tocFile_->data()->setBook(this);
  bookFile_ = BookFile::load(root.filePath("book.json"), this);
  ASSERT(tocFile_);
  ASSERT(bookFile_);
  style_ = new Style(root.filePath("style.json"));
  mode_ = new Mode(this);

  RecentBooks::instance()->addBook(this);
}

Notebook::~Notebook() {
  flush(true);
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

QString Notebook::dirPath() const {
  return root.path();
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

  /*
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
  */

  Notebook *nb = new Notebook(d.absolutePath());
  return nb;
}

TOC *Notebook::toc() const {
  return tocFile_->data();
}

bool Notebook::hasPage(int n) const {
  return toc()->contains(n);
}

EntryFile *Notebook::page(int n)  {
  if (pgFiles.contains(n))
    return pgFiles[n];
  EntryFile *f = loadPage(QDir(root.filePath("pages")), n, this);
  ASSERT(f);
  pgFiles[n] = f;

  f->data()->setBook(this);
  connect(f->data(), SIGNAL(titleMod()), SLOT(titleMod()));
  connect(f->data(), SIGNAL(sheetCountMod()), SLOT(sheetCountMod()));
  return f;
}

EntryFile *Notebook::createPage(int n) {
  ASSERT(!pgFiles.contains(n));
  EntryFile *f = ::createPage(root.filePath("pages"), n, this);
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
  EntryFile *pf = page(pgno);
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
  EntryData *pg = dynamic_cast<EntryData *>(sender());
  ASSERT(pg);
  TOCEntry *e = toc()->entry(pg->startPage());
  ASSERT(e);
  e->setTitle(pg->title()->current()->text());
}

void Notebook::sheetCountMod() {
  EntryData *pg = dynamic_cast<EntryData *>(sender());
  ASSERT(pg);
  TOCEntry *e = toc()->entry(pg->startPage());
  ASSERT(e);
  e->setSheetCount(pg->sheetCount());
}

BookData *Notebook::bookData() const {
  return bookFile_->data();
}

void Notebook::flush(bool mustcommit) {
  qDebug() << "Notebook::flush" << mustcommit;
  bool actv = false;
  bool ok = true;
  if (tocFile_->needToSave()) {
    actv = true;
    ok = ok && tocFile_->saveNow();
  }
  if (bookFile_->needToSave()) {
    actv = true;
    ok = ok && bookFile_->saveNow();
    RecentBooks::instance()->addBook(this);
  }
  foreach (EntryFile *pf, pgFiles) {
    if (pf->needToSave()) {
      actv = true;
      ok = ok && pf->saveNow();
    }
  }
  if (actv) {
    uncommitted = true;
    if (!ok)
      qDebug() << "Notebook flushed, with errors";
  }
  if (mustcommit 
      || (uncommitted
          && lastCommit.secsTo(QDateTime::currentDateTime()) > COMMIT_IVAL_S)) {
    if (style_->contains("vc"))
      VersionControl::commit(root.path(), style_->string("vc"));
    uncommitted = false;
    lastCommit = QDateTime::currentDateTime();
  }
}

Mode *Notebook::mode() const {
  return mode_;
}
