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

#include "TOC.H"
#include <QDebug>
#include "TOCEntry.H"
#include "EntryData.H"
#include "TitleData.H"
#include "Assert.H"

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
