// Book/Notebook.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

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

#include <QTimer>
#include <QDebug>

#define COMMIT_IVAL_S 600 // if vc, commit every once in a while
#define COMMIT_AVOID_S 60 // ... but not too soon after activity

Notebook::Notebook(QString path) {
  commitTimer = 0;
  root = QDir(path);
  Style s0(root.filePath("style.json"));
  hasVC = s0.contains("vc");

  if (hasVC) {
    mostRecentChange = QDateTime::currentDateTime();
    VersionControl::update(root.path(), s0.string("vc"));
    commitTimer = new QTimer(this);
    commitTimer->setSingleShot(true);
    connect(commitTimer, SIGNAL(timeout()), SLOT(commitNowUnless()));
  }
   

  tocFile_ = TOCFile::load(root.filePath("toc.json"), this);
  ASSERT(tocFile_);
  tocFile_->data()->setBook(this);
  if (hasVC)
    connect(tocFile_->data(), SIGNAL(mod()), SLOT(commitSoonish()));
  bookFile_ = BookFile::load(root.filePath("book.json"), this);
  ASSERT(bookFile_);
  if (hasVC)
    connect(bookFile_->data(), SIGNAL(mod()), SLOT(commitSoonish()));

  style_ = new Style(root.filePath("style.json"));
  mode_ = new Mode(this);

  RecentBooks::instance()->addBook(this);
}

Notebook::~Notebook() {
  commitNow();
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
  if (hasVC)
    connect(f->data(), SIGNAL(mod()), this, SLOT(commitSoonish()));
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
  if (hasVC)
    connect(f->data(), SIGNAL(mod()), this, SLOT(commitSoonish()));
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

void Notebook::flush() {
  bool ok = true;

  if (tocFile_->needToSave()) 
    ok = ok && tocFile_->saveNow();

  if (bookFile_->needToSave()) {
    ok = ok && bookFile_->saveNow();
    RecentBooks::instance()->addBook(this);
  }

  foreach (EntryFile *pf, pgFiles) 
    if (pf->needToSave()) 
      ok = ok && pf->saveNow();

  if (!ok)
    qDebug() << "Notebook flushed, with errors";
}

void Notebook::commitSoonish() {
  mostRecentChange = QDateTime::currentDateTime();
  if (commitTimer && !commitTimer->isActive()) {
    commitTimer->setInterval(COMMIT_IVAL_S * 1000);
    commitTimer->start();
  }
}

void Notebook::commitNowUnless() {
  ASSERT(commitTimer);
  if (mostRecentChange.secsTo(QDateTime::currentDateTime()) < COMMIT_AVOID_S) {
    // let's not do it quite yet (test again in 5 s; I am pretty lazy)
    commitTimer->setInterval(5000);
    commitTimer->start();
  } else {
    commitNow();
  }
}
  
void Notebook::commitNow() {
  flush();
  if (hasVC)
    VersionControl::commit(root.path(), style_->string("vc"));
}

Mode *Notebook::mode() const {
  return mode_;
}
