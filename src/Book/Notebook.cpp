
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

#include "Notebook.h"
#include "TOC.h"
#include "EntryFile.h"
#include "TitleData.h"
#include "Style.h"
#include "Assert.h"
#include "RecentBooks.h"
#include "Index.h"

#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include "RmDir.h"

QString Notebook::checkVersionControl(QString path) {
  QDir root(path);
  Style s0(root.filePath("style.json"));
  if (s0.contains("vc"))
    return s0.string("vc");
  else
    return "";
}

QString Notebook::checkVersionControl() {
  return checkVersionControl(dirPath());
}


Notebook::Notebook(QString path, bool ro0): root(QDir(path)), ro(ro0) {
  style_ = 0;
  index_ = 0;
  tocFile_ = 0;
  bookFile_ = 0;
}

void Notebook::load() {
  if (bookFile_)
    return;
  
  QString bookfile = root.exists("book.eln") ? "book.eln" : "book.json";    
  bookFile_ = BookFile::load(root.filePath(bookfile), this);
  if (!bookFile_)
    throw QString("Could not load book file");
  bookFile_->data()->setBook(this);

  tocFile_ = TOCFile::load(root.filePath("toc.json"), this);
  if (tocFile_ && !tocFile_->data()->verify(root.filePath("pages"))) {
    qDebug() << "TOC verification failed - will rebuild TOC and index";
    delete tocFile_;
    tocFile_ = 0;
    root.remove("toc.json");
    root.remove("index.json");
  }
  if (!tocFile_) {
    qDebug() << "No TOC file found, trying to rebuild";
    TOC *t = TOC::rebuild(root.filePath("pages"));
    if (!t)
      throw QString("Could not rebuild TOC");
    tocFile_ = TOCFile::createFromData(t, root.filePath("toc.json"));
    tocFile_->saveNow(true);
  }
  if (!tocFile_)
    throw QString("Could not load TOC");
  tocFile_->data()->setBook(this);

  index_ = new Index(dirPath(), toc(), this);

  style_ = new Style(root.filePath("style.json"));

  RecentBooks::instance()->addBook(this);
  
  connect(bookFile_->data(), SIGNAL(mod()), this, SIGNAL(mod()));
  connect(tocFile_->data(), SIGNAL(mod()), this, SIGNAL(mod()));
}

Notebook::~Notebook() {
}

Style const &Notebook::style() const {
  ASSERT(style_);
  return *style_;
}

QString &Notebook::errMsg() {
  static QString e;
  return e;
}

QString Notebook::errorMessage() {
  return errMsg();
}

Notebook *Notebook::open(QString path, bool readonly) {
  errMsg() = "";
  QDir d(path);
  if (!d.exists()) {
    errMsg() = "Path does not exist: '" + path + "'";
    return 0;
  }
  try {
    return new Notebook(d.absolutePath(), readonly);
  } catch (QString s) {
    errMsg() = s;
    return 0;
  }
}

QString Notebook::filePath(QString f) const {
  return root.filePath(f);
}

QString Notebook::dirPath() const {
  return root.path();
}

bool Notebook::create(QString path, QString vc) {
  errMsg() = "";
  QDir d(path);
  if (d.exists()) {
    errMsg() = "Cannot create new notebook at existing path: '" + path + "'";
    qDebug() << "Notebook:" << errMsg();
    return false;
  }

  if (!d.mkpath("pages")) {
    errMsg() = "Failed to create 'pages' directory at '" + path + "'";
    qDebug() << "Notebook:" << errMsg(); 
    return false;
  }
  
  delete TOCFile::create(d.filePath("toc.json"));
  delete BookFile::create(d.filePath("book.eln"));

  copyStyleFile(d, vc);

  if (vc == "git") {
    if (!createGitArchive(d)) {
      RmDir::recurse(path);
      return false;
    }
  }
  return true;
}

bool Notebook::createGitArchive(QDir d) {
  // true if created OK
  QProcess proc;
  proc.setWorkingDirectory(d.absolutePath());

  proc.start("git", QStringList() << "init");
  if (!proc.waitForFinished()
      || proc.exitStatus()!=QProcess::NormalExit
      || proc.exitCode()!=0) {
    errMsg() =  "Failed to initialize git archive";
    qDebug() << "Notebook: " << errMsg();
    return false;
  }

  proc.start("git", QStringList() << "add" << ".");
  if (!proc.waitForFinished()
      || proc.exitStatus()!=QProcess::NormalExit
      || proc.exitCode()!=0) {
    errMsg() = "Failed to add to git archive";
    qDebug() << "Notebook:" << errMsg();
    return false;
  }
      
  proc.start("git", QStringList() << "commit"
	     << "-m" << "New notebook");
  if (!proc.waitForFinished()
      || proc.exitStatus()!=QProcess::NormalExit
      || proc.exitCode()!=0) {
    errMsg() = "Failed to commit git archive";
    qDebug() << "Notebook:" << errMsg();
    return false;
  }

  return true;
}

void Notebook::copyStyleFile(QDir d, QString vc) {
  QFile styleIn(":/style.json");
  QFile styleOut(d.filePath("style.json"));
  styleIn.open(QFile::ReadOnly);
  styleOut.open(QFile::WriteOnly);
  QTextStream in(&styleIn);
  QTextStream out(&styleOut);
  while (!in.atEnd()) {
    QString l = in.readLine();
    if (l.indexOf("\"vc\"")>=0)
      l.replace("\"\"", "\"" + vc + "\"");
    out << l;
  }
}  

TOC *Notebook::toc() const {
  ASSERT(tocFile_);
  return tocFile_->data();
}

bool Notebook::hasEntry(int n) const {
  return toc()->contains(n);
}

CachedEntry Notebook::entry(int n)  {
  ASSERT(tocFile_);

  if (pgFiles.contains(n)) {
    CachedEntry ce = pgFiles[n];
    if (ce)
      return ce;
  }

  EntryFile *f = 0;
  if (toc()->contains(n)) {
    QString uuid = toc()->tocEntry(n)->uuid();
    f = ::loadEntry(QDir(root.filePath("pages")), n, uuid, this);
    if (!f) 
      f = recoverFromMissingEntry(n);
  } else {
    f = recoverFromMissingEntry(n);
  }
  ASSERT(f);
  CachedEntry entry(f);
  pgFiles[n] = entry;

  entry.setBook(this);
  connect(entry.data(), SIGNAL(titleMod()), SLOT(titleMod()));
  connect(entry.data(), SIGNAL(sheetCountMod()), SLOT(sheetCountMod()));
  index_->watchEntry(entry.data());
  connect(entry.data(), SIGNAL(mod()), this, SIGNAL(mod()));
  return entry;
}

CachedEntry Notebook::createEntry(int n) {
  ASSERT(tocFile_);

  if (pgFiles.contains(n)) {
    return recoverFromExistingEntry(n);
  }

  EntryFile *f = ::createEntry(root.filePath("pages"), n, this);
  if (!f)
    return CachedEntry();
  CachedEntry entry(f);
  pgFiles[n] = entry;
  entry.data()->setStartPage(n);
  toc()->addEntry(entry.data());

  entry.setBook(this);
  connect(entry.data(), SIGNAL(titleMod()), SLOT(titleMod()));
  connect(entry.data(), SIGNAL(sheetCountMod()), SLOT(sheetCountMod()));
  index_->watchEntry(entry.data());
  connect(entry.data(), SIGNAL(mod()), this, SIGNAL(mod()));
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
  pgFiles.remove(pgno);

  if (!toc()->deleteEntry(toc()->find(pgno))) {
    // deleteentry triggers mod() and hence flush of index too
    qDebug() << "Failure to delete entry from TOC";
    ASSERT(0);
  }
  if (!::deleteEntryFile(QDir(root.filePath("pages")), pgno, uuid)) {
    qDebug() << "Failure to delete entry file";
    ASSERT(0);
  }
  return true;
}

void Notebook::titleMod() {
  EntryData *pg = dynamic_cast<EntryData *>(sender());
  ASSERT(pg);
  TOCEntry *e = toc()->tocEntry(pg->startPage());
  ASSERT(e);
  e->setTitle(pg->title()->text()->text());
}

void Notebook::sheetCountMod() {
  EntryData *pg = dynamic_cast<EntryData *>(sender());
  ASSERT(pg);
  TOCEntry *e = toc()->tocEntry(pg->startPage());
  ASSERT(e);
  e->setSheetCount(pg->sheetCount());
}

BookData *Notebook::bookData() const {
  ASSERT(bookFile_);
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
      ok = ok && pf.saveNow();

  index_->flush();

  if (!ok)
    qDebug() << "Notebook flushed, with errors";
}


Index *Notebook::index() const {
  ASSERT(index_);
  return index_;
}


CachedEntry Notebook::recoverFromExistingEntry(int pgno) {
  QMessageBox::critical(0, "eln",
                        QString("Page %1 already exists while trying to create"
                                " a new entry. This is a sign of TOC"
                                " corruption. ELN will exit now and attempt"
                                " to rebuild the TOC when you restart it.")
                        .arg(pgno), QMessageBox::Ok);
  flush();
  root.remove("toc.json");
  root.remove("index.json");
  ::exit(1);
  return CachedEntry();
}

EntryFile *Notebook::recoverFromMissingEntry(int pgno) {
  QMessageBox::critical(0, "eln",
                        QString("Page %1 could not be loaded."
                                " This is a sign of TOC"
                                " corruption. ELN will exit now and attempt"
                                " to rebuild the TOC when you restart it.")
                        .arg(pgno), QMessageBox::Ok);
  flush();
  root.remove("toc.json");
  root.remove("index.json");
  ::exit(1);
  return 0;
}

void Notebook::markReadOnly() {
  ro = true;
}
