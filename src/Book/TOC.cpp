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

TOCEntry *TOC::entry(int startPage) const {
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
    return 0;
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

bool TOC::verify(QDir pages) const {
  /* Will verify that there is a 1:1 relationship between the TOC and the
     actual contents of the pages directory.
   */
  QMap<int, QString> pg2file;
  QMap<int, QString> pg2uuid;
  readPageDir(pages, pg2file, pg2uuid);
  QStringList missing_from_directory;
  QStringList missing_from_index;
  foreach (int pgno, entries().keys()) {
    if (pg2file.contains(pgno)
        && entries()[pgno]->uuid() == pg2uuid[pgno]) {
      // good
    } else {
      missing_from_directory
        << QString("%1 (%2)").arg(pgno).arg(entries()[pgno]->uuid());
    }
  }
  foreach (int pgno, pg2file.keys()) {
    if (entries().contains(pgno)
        && entries()[pgno]->uuid() == pg2uuid[pgno]) {
      // good
    } else {
      missing_from_index
        << QString("%1 (%2)").arg(pgno).arg(pg2uuid[pgno]);
    }
  }

  if (missing_from_directory.isEmpty()
      && missing_from_index.isEmpty())
    return true;

  // Mismatch
  QString msg = "Detected a mismatch between the table of contents"
    " and the actual contents of the notebook.\n";
  if (!missing_from_directory.isEmpty())
    msg += "The TOC contains pages that the notebook does not: "
      + missing_from_directory.join("; ") + "\n";
  if (!missing_from_index.isEmpty())
    msg += "The notebook contains pages that the TOC does not: "
      + missing_from_index.join("; ") + "\n";
  msg += "Click OK to remove the TOC file. (It will be rebuilt automatically.)"
    " Or click Abort to quit.";

  if (QMessageBox::warning(0, "eln", msg,
                           QMessageBox::Ok | QMessageBox::Abort)
      == QMessageBox::Ok)
    return false;
  
  QApplication::quit();
  ::exit(1);
}

void TOC::readPageDir(QDir pages,
                      QMap<int, QString> &pg2file,
                      QMap<int, QString> &pg2uuid) {
  foreach (QFileInfo const &fi, pages.entryInfoList()) {
    if (!fi.isFile())
      continue;
    QString fn = fi.fileName();
    if (fn.endsWith(".moved") || fn.endsWith(".THIS")
        || fn.endsWith(".OTHER") || fn.endsWith(".BASE"))
       errorReturn("Presence of " + fn
                   + " indicates unsuccessful bzr update.");
    if (!fn.endsWith(".json"))
      continue;
    QRegExp re("^(\\d\\d*)-(.*).json");
    if (re.exactMatch(fn)) {
      int n = re.cap(1).toInt();
      pg2uuid[n] = re.cap(2);
      if (pg2file.contains(n))
        errorReturn("Duplicate page number: "
                    + QString::number(n) + ".");
      pg2file[n] = fn;
    } else {
      QRegExp re("^(\\d\\d*).json");
      if (re.exactMatch(fn)) {
	int n = re.cap(1).toInt();
	pg2uuid[n] = "";
	if (pg2file.contains(n))
	  errorReturn("Duplicate page number: "
                      + QString::number(n) + ".");
	pg2file[n] = fn;
      } else {      
	 errorReturn("Cannot parse " + fn + " as a page file name.");
      }
    }
  }
}  

TOC *TOC::rebuild(QDir pages) {
  QMap<int, QString> pg2file;
  QMap<int, QString> pg2uuid;
  QProgressDialog mb("Table of contents found missing or corrupted."
                     " Attempting to rebuild...", "Cancel", 0, 1000);
  mb.setWindowModality(Qt::WindowModal);
  mb.setMinimumDuration(0);
  mb.setValue(1);

  readPageDir(pages, pg2file, pg2uuid);

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

    QString fn = pg2file[n];
    EntryFile *f = EntryFile::load(pages.absoluteFilePath(fn), 0);
    if (!f) 
      return errorReturn("Failed to load " + fn + ".");
    bool mustsave = false;
    int m = f->data()->startPage();
    if (m!=n) {
      qDebug() << "TOC::rebuildTOC " << n << ":" << fn
               << ": Page number in file is " << m << ". Corrected.";
      f->data()->setStartPage(n);
      mustsave = true;
    }
    QString id = f->data()->uuid();
    if (id!=pg2uuid[n]) {
      qDebug() << "TOC::rebuildTOC " << n << ":" << fn
               << ": UUID in file is " << id << ". Corrected to " << pg2uuid[n];
      f->data()->setUuid(pg2uuid[n]);
      mustsave = true;
    }
    if (mustsave)
      f->saveNow();
    
    toc->addEntry(f->data());
    delete f;
  }

  return toc;
}
