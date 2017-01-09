// Data/EntryData.cpp - This file is part of eln

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

// EntryData.C

#include "EntryData.h"
#include "BlockData.h"
#include "TitleData.h"
#include <QDebug>
#include "Notebook.h"
#include "ResManager.h"
#include "Assert.h"

#include "App.h"
#include "CUI.h"

static Data::Creator<EntryData> c("page");
// I would *like* to rename the typ:page to typ:entry, but I cannot, because
// TOCEntry already uses that. So I'll leave that for now.

EntryData::EntryData(Data *parent): Data(parent) {
  nb = 0;
  setType("page");
  startPage_ = 1;
  unlocked_ = false;
  stampTime_ = 0;
  wasEmpty = true;
  cui_ = App::instance()->cui()->current();

  maxSheet = 0;
}

EntryData::~EntryData() {
}

QList<class BlockData *> EntryData::blocks() const {
  return children<BlockData>();
}

bool EntryData::isEmpty() const {
  TitleData *title_ = firstChild<TitleData>();
  if (title_ && !title_->isDefault())
    return false;
  foreach (BlockData *b, blocks())
    if (!b->isEmpty())
      return false;
  return true;
}

void EntryData::insertBlockBefore(BlockData *b, Data *ref) {
  insertChildBefore(b, ref);
  connect(b, SIGNAL(newSheet(int)), SLOT(newSheet()));
  connect(b, SIGNAL(sheetCountMod(int)), SLOT(newSheet()));
  if (b->lastSheet()>maxSheet) {
    maxSheet = b->lastSheet();
    emit sheetCountMod();
  }
}

void EntryData::addBlock(BlockData *b) {
  addChild(b);
  connect(b, SIGNAL(newSheet(int)), SLOT(newSheet()));
  connect(b, SIGNAL(sheetCountMod(int)), SLOT(newSheet()));
  if (b->lastSheet()>maxSheet) {
    maxSheet = b->lastSheet();
    emit sheetCountMod();
  }
}

bool EntryData::deleteBlock(BlockData *b) {
  if (!deleteChild(b))
    return false;

  newSheet();
  return true;
}

void EntryData::newSheet() {
  int newMax = 0;
  // I think we can actually assume the last block is on the last page,
  // but I am going to be bloody minded about it.
  foreach (BlockData *b, blocks()) {
    if (b->lastSheet()>newMax)
      newMax = b->lastSheet();
  }
  bool doEmit = maxSheet>=0;
  if (newMax != maxSheet) {
    maxSheet = newMax;
    if (doEmit)
      emit sheetCountMod();
  }
}  

void EntryData::loadMore(QVariantMap const &src) {
  Data::loadMore(src);
  TitleData *title_ = firstChild<TitleData>();
  // Any old title has already been destructed by Data's loadChildren()
  ASSERT(title_);
  connect(title_, SIGNAL(textMod()), SIGNAL(titleMod()));

  maxSheet = 0;
  foreach (BlockData *b, blocks()) {
    if (b->lastSheet() > maxSheet)
      maxSheet = b->lastSheet();
    connect(b, SIGNAL(newSheet(int)), SLOT(newSheet()));
    connect(b, SIGNAL(sheetCountMod(int)), SLOT(newSheet()));
  }
}

TitleData *EntryData::title() {
  TitleData *title_ = firstChild<TitleData>();
  qDebug() << "title_ = " << title_;
  if (!title_) {
    title_ = new TitleData(this);
    connect(title_, SIGNAL(textMod()), SIGNAL(titleMod()));
  }
  
  return title_;
}

QString EntryData::titleText() {
  return title()->text()->text();
}

int EntryData::startPage() const {
  return startPage_;
}

QString EntryData::cui() const {
  return cui_;
}

void EntryData::setCui(QString c) {
  cui_ = c;
  markModified(InternalMod);
}

bool EntryData::isUnlocked() const {
  return unlocked_;
}

bool EntryData::isWritable() const {
  Notebook *b = book();
  
  if (!b || b->isReadOnly())
    return false;
  if (isUnlocked())
    return true;
  if (isEmpty())
    return true;
  return isRecent() && App::instance()->cui()->match(cui_);
}

bool EntryData::lateNotesAllowed() const {
  return !book()->isReadOnly();
}

int EntryData::stampTime() const {
  return stampTime_;
}

void EntryData::setStartPage(int s) {
  if (startPage_==s)
    return;
  startPage_ = s;
  markModified(InternalMod);
}

void EntryData::setUnlocked(bool u) {
  if (unlocked_==u)
    return;
  unlocked_ = u;
  markModified(InternalMod);
}

void EntryData::setStampTime(int s) {
  if (stampTime_==s)
    return;
  stampTime_ = s;
  markModified(InternalMod);
}

int EntryData::sheetCount() const {
  return maxSheet + 1;
}

void EntryData::setBook(Notebook *x) {
  nb = x;
}

Notebook *EntryData::book() const {
  return nb;
}

ResManager *EntryData::resManager() const {
  return firstChild<ResManager>();
}

EntryData const *EntryData::entry() const {
  return this;
}

EntryData *EntryData::entry() {
  return this;
}

void EntryData::resetCreation() {
  if (!isEmpty()) {
    qDebug() << "EntryData::resetCreation: Refusing because not empty";
    return;
  }

  setCreated(QDateTime::currentDateTime());
  setModified(QDateTime::currentDateTime());
  foreach (BlockData *b, blocks()) {
    b->setCreated(QDateTime::currentDateTime());
    b->setModified(QDateTime::currentDateTime());
  }
}

void EntryData::markModified(ModType mt) {
  Data::markModified(mt);
  bool empty = isEmpty();
  if (empty != wasEmpty) {
    wasEmpty = empty;
    emit emptyStatusChanged(empty);
  }
}

      
