
// Book/Notebook.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// Notebook.C

#include "Notebook.h"
#include "TOC.h"
#include "EntryFile.h"
#include "TitleData.h"
#include "Style.h"
#include "ElnAssert.h"
#include "RecentBooks.h"
#include "Index.h"
#include "Translate.h"
#include "Catalog.h"

#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include "RmDir.h"
#include "Mode.h"

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
  mode_ = new Mode(isReadOnly(), this);
}

void Notebook::load() {
  if (bookFile_)
    return;
  
  QString bookfile = root.exists("book.eln") ? "book.eln" : "book.json";    
  bookFile_ = BookFile::load(root.filePath(bookfile), this);
  if (!bookFile_)
    throw QString("Could not load book file.");
  bookFile_->data()->setBook(this);

  qDebug() << "Cataloging pages for " << root.absolutePath();
  Catalog cat(root.filePath("pages"));

  qDebug() << "Loading TOC for " << root.absolutePath();
  tocFile_ = TOCFile::load(root.filePath("toc.json"), this);
  if (tocFile_) {
    qDebug() << "Updating TOC";
    if (tocFile_->data()->update(cat)) {
      qDebug() << "TOC updated";
    } else {
      qDebug() << "TOC update failed - will rebuild TOC and index";
      delete tocFile_;
      tocFile_ = 0;
      root.remove("toc.json");
      root.remove("index.json");
    }
  } else {
    qDebug() << "No TOC file found";
  }
  
  if (!tocFile_) {
    qDebug() << "Trying to rebuild TOC";
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
  if (needToSave())
    qDebug() << "WARNING: Notebook destructed while needing to save";
}

Style const &Notebook::style() const {
  ASSERT(style_);
  return *style_;
}

QString &Notebook::errMsg() {
  static QString e = "";
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

  { QFile ignore(d.absoluteFilePath(".gitignore"));
    if (!ignore.open(QFile::WriteOnly))
      return false;
    ignore.write("*~\n");
    ignore.write(".*~\n");
    ignore.write("toc.json\n");
    ignore.write("index.json\n");
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
    out << l << "\n";
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
  index_->watchEntry(entry.obj());
  connect(entry.data(), SIGNAL(mod()), this, SIGNAL(mod()));
  return entry;
}

CachedEntry Notebook::createEntry(int n) {
  ASSERT(tocFile_);
  ASSERT(!isReadOnly());

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
  index_->watchEntry(entry.obj());
  connect(entry.data(), SIGNAL(mod()), this, SIGNAL(mod()));
  //  bookData()->setEndDate(QDate::currentDate());
  return entry;
}

QDate Notebook::endDate() const {
  return toc()->latestMod().date();
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
  
  index_->deleteEntry(pf.obj()); // this doesn't save, but see below

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

bool Notebook::needToSave() const {
  if (tocFile_->needToSave()) 
    return true;

  if (bookFile_->needToSave()) 
    return true;

  foreach (CachedEntry pf, pgFiles) 
    if (pf && pf.needToSave())
      return true;

  return false;
}  

bool Notebook::flush() {
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

  foreach (CachedEntry pf, pgFiles) {
    if (pf && pf.needToSave()) {
      actv = true;
      ok = ok && pf.saveNow();
    }
  }

  index_->flush();

  if (!ok)
    qDebug() << "Notebook flushed, with errors";

  return actv;
}


Index *Notebook::index() const {
  ASSERT(index_);
  return index_;
}


CachedEntry Notebook::recoverFromExistingEntry(int pgno) {
  QString eln = Translate::_("eln");
  QMessageBox::critical(0, eln,
                        QString("Page %1 already exists while trying to create"
                                " a new entry. This is a sign of TOC"
                                " corruption. %2 will exit now and attempt"
                                " to rebuild the TOC when you restart it.")
                        .arg(pgno).arg(eln), QMessageBox::Ok);
  flush();
  root.remove("toc.json");
  root.remove("index.json");
  ::exit(1);
  return CachedEntry();
}

EntryFile *Notebook::recoverFromMissingEntry(int pgno) {
  QString eln = Translate::_("eln");
  QMessageBox::critical(0, eln,
                        QString("Page %1 could not be loaded."
                                " This is a sign of TOC"
                                " corruption. %2 will exit now and attempt"
                                " to rebuild the TOC when you restart it.")
                        .arg(pgno).arg(eln), QMessageBox::Ok);
  flush();
  root.remove("toc.json");
  root.remove("index.json");
  ::exit(1);
  return 0;
}

void Notebook::markReadOnly() {
  ro = true;
}

Mode *Notebook::mode() const {
  return mode_;
}
