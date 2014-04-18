
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
#include "BackgroundVC.H"
#include "Index.H"
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>

#define COMMIT_IVAL_S 600 // If vc, commit every once in a while
#define COMMIT_AVOID_S 60 // ... but not too soon after activity
#define UPDATE_IVAL_S 3600 // If vc, check for updates once in a while
#define UPDATE_AVOID_S 900 // ... but not if anything has recently changed

Notebook::Notebook(QString path) {
  commitTimer = 0;
  root = QDir(path);

  mode_ = new Mode(this);
  
  backgroundVC = 0;
  updateTimer = 0;
  commitTimer = 0;
  style_ = 0;
  index_ = 0;
  tocFile_ = 0;
  bookFile_ = 0;

  Style s0(root.filePath("style.json"));
  if (s0.contains("vc")) 
    VersionControl::update(root.path(), s0.string("vc"));

  loadme();
}

void Notebook::loadme() {
  bookFile_ = BookFile::load(root.filePath("book.json"), this);
  ASSERT(bookFile_);
  bookFile_->data()->setBook(this);

  tocFile_ = TOCFile::load(root.filePath("toc.json"), this);
  if (!tocFile_) {
    qDebug() << "No TOC file found, trying to rebuild";
    TOC *t = TOC::rebuild(root.filePath("pages"));
    if (!t) {
      QMessageBox mb(QMessageBox::Critical, "eln",
                     "No TOC file found in notebook folder and I could "
                     "not reconstruct it. Manual recovery will be needed. "
                     "See debug log for more information.",
                     QMessageBox::Abort);
        mb.addButton("Quit", QMessageBox::RejectRole);
        mb.exec();
        QApplication::quit();
    }
    tocFile_ = TOCFile::createFromData(t, root.filePath("toc.json"));
    tocFile_->saveNow(true);
  }
  ASSERT(tocFile_);
  tocFile_->data()->setBook(this);

  index_ = new Index(dirPath(), toc(), this);

  style_ = new Style(root.filePath("style.json"));

  RecentBooks::instance()->addBook(this);

  hasVC = style_->contains("vc");
  if (hasVC) {
    backgroundVC = new BackgroundVC(this);
    commitTimer = new QTimer(this);
    commitTimer->setSingleShot(true);
    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    updateTimer->start();
    connect(updateTimer, SIGNAL(timeout()), SLOT(updateNowUnless()));
    connect(commitTimer, SIGNAL(timeout()), SLOT(commitNowUnless()));
    connect(bookFile_->data(), SIGNAL(mod()), SLOT(commitSoonish()));
    connect(tocFile_->data(), SIGNAL(mod()), SLOT(commitSoonish()));
    connect(backgroundVC, SIGNAL(done(bool)), SLOT(committed(bool)));
  }
}

Notebook::~Notebook() {
  commitNow();
}

void Notebook::unloadme() {
  delete backgroundVC;
  backgroundVC = 0;

  delete updateTimer;
  updateTimer = 0;
  
  delete commitTimer;
  commitTimer = 0;

  delete index_;
  index_ = 0;

  delete style_;
  style_ = 0;

  delete tocFile_;
  tocFile_ = 0;

  delete bookFile_;
  bookFile_ = 0;
  
  pgFiles.clear();
}

bool Notebook::reload() {
  if (backgroundVC && backgroundVC->isBusy())
    return false;

  QMap<int, int> renumberedPages; // obviously, this should be filled

  unloadme();
  loadme();

  emit reloaded(renumberedPages);
  return true;
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
  styleIn.copy(d.filePath("style.json"));

  Notebook *nb = new Notebook(d.absolutePath());
  return nb;
}

TOC *Notebook::toc() const {
  return tocFile_->data();
}

bool Notebook::hasEntry(int n) const {
  return toc()->contains(n);
}

CachedEntry Notebook::entry(int n)  {
  if (pgFiles.contains(n)) {
    CachedEntry ce = pgFiles[n];
    if (ce)
      return ce;
  }

  QString uuid = toc()->entry(n)->uuid();

  EntryFile *f = ::loadEntry(QDir(root.filePath("pages")), n, uuid, this);
  ASSERT(f);
  CachedEntry entry(f);
  pgFiles[n] = entry;

  f->data()->setBook(this);
  connect(f->data(), SIGNAL(titleMod()), SLOT(titleMod()));
  connect(f->data(), SIGNAL(sheetCountMod()), SLOT(sheetCountMod()));
  index_->watchEntry(f->data());
  if (hasVC)
    connect(f->data(), SIGNAL(mod()), this, SLOT(commitSoonish()));
  return entry;
}

CachedEntry Notebook::createEntry(int n) {
  ASSERT(!pgFiles.contains(n));
  EntryFile *f = ::createEntry(root.filePath("pages"), n, this);
  if (!f)
    return CachedEntry();
  CachedEntry entry(f);
  pgFiles[n] = entry;
  f->data()->setStartPage(n);
  toc()->addEntry(f->data());

  f->data()->setBook(this);
  connect(f->data(), SIGNAL(titleMod()), SLOT(titleMod()));
  connect(f->data(), SIGNAL(sheetCountMod()), SLOT(sheetCountMod()));
  index_->watchEntry(f->data());
  if (hasVC)
    connect(f->data(), SIGNAL(mod()), this, SLOT(commitSoonish()));
  bookData()->setEndDate(QDate::currentDate());
  return entry;
}

bool Notebook::deleteEntry(int pgno) {
  CachedEntry pf(entry(pgno));
  if (!pf) {
    qDebug() << "Notebook: cannot delete nonexistent entry";
    return false;
  }
  ASSERT(pf);
  if (!pf->isEmpty()) {
    qDebug() << "Notebook: refusing to delete non-empty entry";
    return false;
  }

  QString uuid = pf->uuid();
  
  index_->deleteEntry(pf); // this doesn't save, but see below

  pf.file()->cancelSave();
  qDebug() << "pgFiles.removing " << pgno;
  pgFiles.remove(pgno);
  qDebug() << "pgFiles.removed " << pgno;

  ASSERT(toc()->deleteEntry(toc()->find(pgno))); // this triggers mod() and hence flush of index too
  ASSERT(::deleteEntryFile(QDir(root.filePath("pages")), pgno, uuid));
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

  foreach (CachedEntry pf, pgFiles) 
    if (pf)
      if (pf.file()->needToSave()) 
	ok = ok && pf.file()->saveNow();

  index_->flush();

  if (ok)
    qDebug() << "Notebook flushed OK";
  else
    qDebug() << "Notebook flushed, with errors";
}

void Notebook::updateNowUnless() {
  ASSERT(updateTimer);
  if (mostRecentChange.secsTo(QDateTime::currentDateTime()) < UPDATE_AVOID_S) {
    updateTimer->setInterval(500 * UPDATE_AVOID_S); // that's 1/2 x avoid ival
    updateTimer->start();
  } else {
    // let's see if there is anything to update
    updateTimer->setInterval(1000 * UPDATE_IVAL_S);
    updateTimer->start();
  }
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
    // let's not do it quite yet (test again in a while)
    commitTimer->setInterval(500 * COMMIT_AVOID_S); // that's 1/2 x avoid ival
    commitTimer->start();
  } else {
    flush();
    if (backgroundVC && !mostRecentChange.isNull()) {
      mostRecentChange = QDateTime(); // invalidate
      backgroundVC->commit(root.path(), style_->string("vc"));
    }
  }
}
  
void Notebook::commitNow() {
  qDebug() << "Notebook::commitNow";
  flush();
  if (hasVC && !mostRecentChange.isNull()) {
    VersionControl::commit(root.path(), style_->string("vc"));
    mostRecentChange = QDateTime(); // invalidate
  }
}

void Notebook::committed(bool ok) {
  if (ok) {
    // all good
  } else {
    // we'll have to try again
    commitSoonish();
  }
}

Mode *Notebook::mode() const {
  return mode_;
}

Index *Notebook::index() const {
  return index_;
}
