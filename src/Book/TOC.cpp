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
#include "Catalog.h"

static Data::Creator<TOC> c("toc");


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
  e->setLastSeen(QDateTime::currentDateTime());
  entries_[e->startPage()] = e;
  return e;
}

TOCEntry *TOC::updateEntry(EntryData *data) {
  TOCEntry *e = find(data->startPage());
  if (!e)
    return e;
  e->setStartPage(data->startPage());
  e->setTitle(data->titleText());
  e->setSheetCount(data->sheetCount());
  e->setUuid(data->uuid());
  e->setCreated(data->created());
  e->setModified(data->modified());
  e->setLastSeen(QDateTime::currentDateTime());
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
  QRegExp re("(\\d\\d*)-([a-z0-9]+).json");
  return re.exactMatch(fn) ? re.cap(2) : "";
}

bool TOC::update(Catalog const &cat) {
  QStringList misc_errors = cat.errors();
  QMultiMap<int, QString> pg2file = cat.pageToFileMap();
  
  QStringList missing_from_directory;
  QStringList missing_from_index;
  QStringList duplicates_in_directory;
  QList<int> outdated_or_missing_page_in_index;

  foreach (int pgno, entries().keys()) {
    QString uuid = entries()[pgno]->uuid();
    QString fn = uuid.isEmpty()
      ? QString("%1.json").arg(pgno)
      : QString("%1-%2.json").arg(pgno, 4, 10, QChar('0')).arg(uuid);
    if (pg2file.contains(pgno, fn)) {
      if (cat.fileMod(fn) > entries()[pgno]->lastSeen().addSecs(1)) {
        outdated_or_missing_page_in_index << pgno;
	qDebug() << "Outdated" << pgno << fn;
      }
    } else {
      missing_from_directory
        << (uuid.isEmpty()
            ? QString("%1").arg(pgno)
            : QString("%1 (%2)").arg(pgno, 4, 10, QChar('0')).arg(uuid));
    }
  }
  QSet<int> seen;
  foreach (int pgno, pg2file.keys()) {
    if (seen.contains(pgno))
      continue;
    seen.insert(pgno);
    QString fn = *pg2file.find(pgno);
    QString uuid = extractUUIDFromFilename(fn);
    if (pg2file.count(pgno)>1) {
      duplicates_in_directory << QString("%1").arg(pgno);
    } else if (!entries().contains(pgno) || entries()[pgno]->uuid()!=uuid) {
      missing_from_index
        << (uuid.isEmpty()
            ? QString("%1").arg(pgno)
            : QString("%1 (%2)").arg(pgno, 4, 10, QChar('0')).arg(uuid));
      outdated_or_missing_page_in_index << pgno;
    }
  }

  if (!missing_from_directory.isEmpty()
      || !duplicates_in_directory.isEmpty()
      || !misc_errors.isEmpty()) {
    reportMismatch(missing_from_directory, missing_from_index,
                   duplicates_in_directory, misc_errors);
    return false;
  }

  if (outdated_or_missing_page_in_index.isEmpty())
    return true;

  // So there is some stuff to fix
  // We can probably do it, but we should check carefully
  return doUpdate(cat, outdated_or_missing_page_in_index,
                  missing_from_index);
}

static bool quickRead(QString fn,
		      int &pgno, QString &uuid, QDateTime &date) {
  QFile f(fn);
  if (!f.open(QFile::ReadOnly))
    return false;
  char buffer[100];
  QRegExp re1(": *\"(.*)\"");
  QRegExp re2(": (\\d+)");
  int n = 0;
  while (n<3 && f.readLine(buffer, 100)>0) {
    QString line(buffer);
    qDebug() << "quickread read line " << line;
    if (line.indexOf("\"mod\"")>=0 && re1.indexIn(line)) {
      date = QVariant(re1.cap(1)).toDateTime();
      n++;
    } else if (line.indexOf("\"startPage\"")>=0 && re2.indexIn(line)) {
      pgno = re2.cap(1).toInt();
      n++;
    } else if (line.indexOf("\"uuid\"")>=0 && re1.indexIn(line)) {
      uuid = re1.cap(1);
      n++;
    } else if (line.indexOf("\"cc\"")>=0) {
      return true;
    }
  }
  return n>=3;
}


bool TOC::doUpdate(Catalog const &cat,
                   QList<int> const &outdated_or_missing_page_in_index,
                   QStringList missing_from_index) {

  QProgressDialog mb("Updating table of contents...",
		     "Cancel", 0,
		     2 * outdated_or_missing_page_in_index.size());
  mb.setWindowModality(Qt::WindowModal);
  mb.setMinimumDuration(200);
  mb.setValue(0);
  
  QMap<int, QString> entryUuid;
  QDir pages(cat.path());

  int k = 0;
  foreach (int pgno, outdated_or_missing_page_in_index) {
    if (mb.wasCanceled()) 
      return false;

    QString fn = *cat.pageToFileMap().find(pgno);
    QRegExp re("^(\\d\\d*)-(.*).json");
    QString namedid = re.exactMatch(fn) ? re.cap(2) : "";
    int storedpgno;
    QString storeduuid;
    QDateTime storeddate;
    if (!quickRead(pages.absoluteFilePath(fn),
		   storedpgno, storeduuid, storeddate)
	|| storedpgno!=pgno || storeduuid!=namedid) {
      mb.close();
      reportMismatch(QStringList(), missing_from_index,
                     QStringList(), QStringList());
      return false;
    }
    if (!entries_.contains(pgno)
	|| storeddate > entries_[pgno]->lastSeen()) 
      entryUuid[pgno] = storeduuid;
    mb.setValue(++k);
  }
  mb.setMaximum(k + entryUuid.size());
  for (int pgno: entryUuid.keys()) {
    EntryFile *f = ::loadEntry(cat.path(), pgno, entryUuid[pgno], 0);
    if (f) {
      if (!updateEntry(f->data()))
	addEntry(f->data());
      delete f;
    } else {
      delete f;
      errorReturn(QString("Failed to load %1-%2.")
		  .arg(pgno).arg(entryUuid[pgno]));
    }
    mb.setValue(++k);
  }
  return true;
}

void TOC::reportMismatch(QStringList missing_from_directory,
                         QStringList missing_from_index,
                         QStringList duplicates_in_directory,
                         QStringList misc_errors) {
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
      != QMessageBox::Ok) {
    QApplication::quit();
    ::exit(1);
  }
}

struct Entry_ {
  Entry_(QString fn): fn(fn) { }
  QString fn;
  QString uuid;
  QDateTime cre;
  bool operator<(Entry_ const &other) const {
    if (uuid.isEmpty())
      return true;
    if (other.uuid.isEmpty())
      return false;
    return cre<other.cre;
  }
};

void TOC::resolveDuplicates(QMultiMap<int, QString> &pg2file,
			    QDir pages) {
  bool dups = true;
  while (dups) {
    dups = false;
    foreach (int n, pg2file.keys()) {
      if (pg2file.count(n)>1) {
	resolveDuplicates(pg2file, n, pages);
	dups = true;
      }
    }
  }
}

void TOC::resolveDuplicates(QMultiMap<int, QString> &pg2file, int pgno,
			    QDir pages) {
  // Resolves duplicates for pgno, possibly generating further duplicates
  // at subsequent pages
  QList<Entry_> entries;
  for (auto it=pg2file.find(pgno); it!=pg2file.end() && it.key()==pgno;
       ++it)
    entries << it.value();
  QStringList uuids;
  for (auto &it: entries)
    it.uuid =  extractUUIDFromFilename(it.fn);
  for (auto &it: entries) {
    EntryFile *f = EntryFile::load(pages.absoluteFilePath(it.fn), 0);
    if (f) {
      it.cre = f->data()->created();
      delete f;
    } else {
      QFile fd(pages.absoluteFilePath(it.fn));
      QFileInfo fi(fd);
      if (fi.exists() && fi.size()>0) {
	errorReturn("Failed to load " + it.fn + ".");  
      } else {
	fd.remove(); // !? I don't understand this - DW 3/11/16
	continue;
      }
    }
  }
  qSort(entries);
  int k = 0;
  for (auto &it: entries) {
    if (it.cre.isNull())
      continue;
    if (k) {
      QString fn1 = it.fn;
      fn1.replace(QRegExp("^(\\d\\d+)"), QString("%1")
		  .arg(pgno + k, 4, 10, QChar('0')));
      if (!pages.rename(it.fn, fn1))
	errorReturn("Failed to rename " + it.fn + " to " + fn1 + ".");
      qDebug() << "Renamed" << it.fn << "to" << fn1;
      QString res0 = it.fn; res0.replace(".json", ".res");
      if (pages.exists(res0)) {
	QString res1 = fn1; res1.replace(".json", ".res");
	if (!pages.rename(res0, res1))
	  errorReturn("Failed to rename " + res0 + " to " + res1 + ".");
	qDebug() << "Renamed" << res0 << "to" << res1;
      }	  
      QString notes0 = it.fn; notes0.replace(".json", ".notes");
      if (pages.exists(notes0)) {
	QString notes1 = fn1; notes1.replace(".json", ".notes");
	if (!pages.rename(notes0, notes1))
	  errorReturn("Failed to rename " + notes0 + " to " + notes1 + ".");
	qDebug() << "Renamed" << notes0 << "to" << notes1;  
      }
      pg2file.remove(pgno, it.fn);
      pg2file.insert(pgno+k, fn1);
      it.fn = fn1;
    }
    k++;
  }
}

TOC *TOC::rebuild(QDir pages) {
  QProgressDialog mb("Table of contents found missing or corrupted."
                     " Attempting to rebuild...", "Cancel", 0, 1000);
  mb.setWindowModality(Qt::WindowModal);
  mb.setMinimumDuration(200);
  mb.setValue(0);

  Catalog cat(pages.absolutePath());
  QStringList error_accum = cat.errors();
  QMultiMap<int, QString> pg2file = cat.pageToFileMap();

  if (!error_accum.isEmpty())
    return errorReturn(error_accum.join("\n"));

  TOC *toc = new TOC();

  resolveDuplicates(pg2file, pages);

  mb.setMaximum(pg2file.keys().size());
  int k = 0;
  
  foreach (int n, pg2file.keys()) {
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
    mb.setValue(++k);
  }

  return toc;
}

TOCEntry *TOC::findUUID(QString uuid) const {
  for (TOCEntry *e: entries_) 
    if (e->uuid()==uuid)
      return e;
  return 0;
}

QDateTime TOC::latestMod() const {
  QDateTime d;
  for (TOCEntry const *e: entries_) 
    if (e->modified() > d)
      d = e->modified();
  return d;
}
