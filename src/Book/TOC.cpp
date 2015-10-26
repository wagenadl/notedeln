// Book/TOC.cpp - This file is part of eln

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

// TOC.C

#include "TOC.h"
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include "TOCEntry.h"
#include "EntryData.h"
#include "EntryFile.h"
#include "TitleData.h"
#include "Assert.h"
#include <QProgressDialog>

static Data::Creator<TOC> c("toc");

TOC::TOC(Data *parent): Data(parent) {
  setType("toc");
  nb = 0;
}

void TOC::loadMore(QVariantMap const &) {
  entries_.clear();
  foreach (TOCEntry *e, children<TOCEntry>()) 
    entries_[e->startPage()] = e;
}

TOC::~TOC() {
}

QMap<int, TOCEntry *> const &TOC::entries() const {
  return entries_;
}

TOCEntry *TOC::tocEntry(int startPage) const {
  ASSERT(entries_.contains(startPage));
  return entries_[startPage];
}

TOCEntry *TOC::find(int page) const {
  // This is not an efficient implementation, but it's fine for reasonably
  // sized notebooks
  TOCEntry *r = 0;
  foreach (TOCEntry *e, entries_) 
    if (e->startPage() <= page && e->startPage()+e->sheetCount() > page)
      r = e;
  return r;
}

TOCEntry *TOC::findBackward(int page) const {
  TOCEntry *r = 0;
  foreach (TOCEntry *e, entries_) 
    if (e->startPage() <= page && (r==0 || e->startPage() > r->startPage()))
      r = e;
  return r;
}

TOCEntry *TOC::findForward(int page) const {
  TOCEntry *r = 0;
  foreach (TOCEntry *e, entries_) 
    if (e->startPage()+e->sheetCount()-1 >= page
        && (r==0 || e->startPage() < r->startPage()))
      r = e;
  return r;
}
  

TOCEntry *TOC::entryAfter(TOCEntry *te) const {
  bool has = false;
  /* Next magic returns first entry after match */
  foreach (TOCEntry *e, entries_) {
    if (has)
      return e;
    else if (e==te)
      has = true;
  }
  return 0;
}

TOCEntry *TOC::addEntry(EntryData *data) {
  TOCEntry *e = new TOCEntry(this);
  e->setStartPage(data->startPage());
  e->setTitle(data->titleText());
  e->setSheetCount(data->sheetCount());
  e->setUuid(data->uuid());
  e->setCreated(data->created());
  e->setModified(data->modified());
  entries_[e->startPage()] = e;
  return e;
}

bool TOC::isLast(TOCEntry const *e) const {
  return e == lastEntry();
}

bool TOC::contains(int p) const {
  return entries_.contains(p);
}

bool TOC::deleteEntry(TOCEntry *e) {
  if (!e)
    return false;
  int p = e->startPage();
  if (entries_.remove(p)) {
    Data::deleteChild(e);
    return true;
  } else {
    return false;
  }
}

TOCEntry const *TOC::lastEntry() const {
  if (entries_.isEmpty())
    return 0;
  EntryMap::const_iterator i = entries_.constEnd();
  --i;
  return i.value();
}

int TOC::newPageNumber() const {
  TOCEntry const *e = lastEntry();
  if (!e)
    return 1;
  return e->startPage() + e->sheetCount();
}

void TOC::setBook(Notebook *n) {
  nb = n;
}

Notebook *TOC::book() const {
  return nb;
}

QString TOC::extractUUIDFromFilename(QString fn) {
  QRegExp re("^(\\d\\d*)-(.*).json");
  return re.exactMatch(fn) ? re.cap(2) : "";
}

bool TOC::verify(QDir pages) const {
  QStringList misc_errors;
  QMultiMap<int, QString> pg2file = readPageDir(pages, misc_errors);

  QStringList missing_from_directory;
  QStringList missing_from_index;
  QStringList duplicates_in_directory;

  foreach (int pgno, entries().keys()) {
    QString uuid = entries()[pgno]->uuid();
    if (pg2file.contains(pgno)
        && extractUUIDFromFilename(*pg2file.find(pgno))==uuid) {
      // good
    } else {
      missing_from_directory
        << QString("%1 (%2)").arg(pgno).arg(entries()[pgno]->uuid());
    }
  }
  foreach (int pgno, pg2file.keys()) {
    QString uuid = extractUUIDFromFilename(*pg2file.find(pgno));
    if (pg2file.count(pgno)>1) {
      duplicates_in_directory << QString("%1").arg(pgno);
    } else if (entries().contains(pgno) && entries()[pgno]->uuid() == uuid) {
      // good
    } else {
      missing_from_index
        << QString("%1 (%2)").arg(pgno).arg(uuid);
    }
  }

  if (missing_from_directory.isEmpty()
      && missing_from_index.isEmpty()
      && duplicates_in_directory.isEmpty()
      && misc_errors.isEmpty())
    return true;

  // Mismatch
  QString msg = "Detected a mismatch between the table of contents"
    " and the actual contents of the notebook:\n";
  if (!missing_from_directory.isEmpty())
    msg += "The TOC contains entries that the notebook does not: "
      + missing_from_directory.join("; ") + "\n";
  if (!missing_from_index.isEmpty())
    msg += "The notebook contains entries that the TOC does not: "
      + missing_from_index.join("; ") + "\n";
  if (!duplicates_in_directory.isEmpty())
    msg += "The notebook contains more than one entry that start "
      "on each of the following page numbers: "
      + duplicates_in_directory.join("; ") + "\n";
  if (!misc_errors.isEmpty()) 
    msg += misc_errors.join("; ") + "\n";
  msg += "Click OK to remove the TOC file; it will be rebuilt automatically. "
    "Alternatively, click Abort to quit.";

  if (QMessageBox::warning(0, "eln", msg,
                           QMessageBox::Ok | QMessageBox::Abort)
      == QMessageBox::Ok)
    return false;
  
  QApplication::quit();
  ::exit(1);
}

QMultiMap<int, QString> TOC::readPageDir(QDir pages, QStringList &error_out) {
  QMultiMap<int, QString> pg2file;
  QRegExp re1("^(\\d\\d*)-([a-z0-9]+).json");
  QRegExp re0("^(\\d\\d*).json");
  foreach (QFileInfo const &fi, pages.entryInfoList()) {
    if (!fi.isFile())
      continue;
    QString fn = fi.fileName();
    if (fn.endsWith(".moved") || fn.endsWith(".THIS")
        || fn.endsWith(".OTHER") || fn.endsWith(".BASE")) 
      error_out << "Presence of " + fn + " indicates unsuccessful bzr update.";
    if (!fn.endsWith(".json"))
      continue;
    if (re1.exactMatch(fn)) {
      int n = re1.cap(1).toInt();
      pg2file.insert(n, fn);
    } else if (re0.exactMatch(fn)) {
      int n = re0.cap(1).toInt();
      pg2file.insert(n, fn);
    } else {
      qDebug() << "unparsed" << fn;
      error_out << "Cannot parse " + fn + " as a page file name.";
    }
  }
  return pg2file;
}  

static TOC *errorReturn(QString s) {
  QMessageBox mb(QMessageBox::Critical, "Failure to rebuild TOC",
		 QString("No TOC file found in notebook folder and I could ")
		 + "not reconstruct it:\n\n"
		 + s + "\n\n"
		 + "Manual recovery will be needed.",
		 0);
  mb.addButton("Quit", QMessageBox::RejectRole);
  mb.exec();
  QApplication::quit();
  ::exit(1);
  return 0;
}

TOC *TOC::rebuild(QDir pages) {
  QProgressDialog mb("Table of contents found missing or corrupted."
                     " Attempting to rebuild...", "Cancel", 0, 1000);
  mb.setWindowModality(Qt::WindowModal);
  mb.setMinimumDuration(0);
  mb.setValue(1);

  QStringList error_accum;
  QMultiMap<int, QString> pg2file = readPageDir(pages, error_accum);

  if (!error_accum.isEmpty())
    return errorReturn(error_accum.first());

  TOC *toc = new TOC();
  int N = 0;
  foreach (int n, pg2file.keys())
    if (n>N)
      N = n;
  mb.setMaximum(N);
  
  foreach (int n, pg2file.keys()) {
    mb.setValue(n);
    if (mb.wasCanceled()) 
      return 0;

    if (pg2file.count(n)>1)
      errorReturn("Duplicate page number: " + QString::number(n));

    QString fn = *pg2file.find(n);
    EntryFile *f = EntryFile::load(pages.absoluteFilePath(fn), 0);
    if (!f) {
      QFile fd(pages.absoluteFilePath(fn));
      QFileInfo fi(fd);
      if (fi.exists() && fi.size()>0) {
	return errorReturn("Failed to load " + fn + ".");
      } else {
	fd.remove();
	continue;
      }
    }
    bool mustsave = false;
    int m = f->data()->startPage();
    if (m!=n) {
      qDebug() << "TOC::rebuildTOC " << n << ":" << fn
               << ": Page number in file is " << m << ". Corrected.";
      f->data()->setStartPage(n);
      mustsave = true;
    }
    QString storedid = f->data()->uuid();
    QRegExp re("^(\\d\\d*)-(.*).json");
    QString namedid = re.exactMatch(fn) ? re.cap(2) : "";
    
    if (storedid != namedid) {
      qDebug() << "TOC::rebuildTOC " << n << ":" << fn
               << ": UUID in file is " << storedid
	       << ". Corrected to " << namedid;
      f->data()->setUuid(namedid);
      mustsave = true;
    }
    if (mustsave)
      f->saveNow();
    
    toc->addEntry(f->data());
    delete f;
  }

  return toc;
}
