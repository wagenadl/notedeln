// File/Entry.cpp - This file is part of eln

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

// Entry.C

#include "Entry.h"
#include "EntryData.h"
#include "EntryFile.h"
#include <QDebug>
#include "Assert.h"
#include "LateNoteManager.h"

Entry::Entry(EntryData *data): data_(data), file_(0) {
}

Entry::Entry(EntryFile *file): data_(file ? file->data(): 0), file_(file) {
  if (file) {
    QString path = file->fileName();
    path.replace(".json", ".notes");
    lnm_ = new LateNoteManager(path, this);
  } else {
    lnm_ = 0;
  }
}

Entry::~Entry() {
  if (file_) {
    file_->saveNow();
    delete file_;
  } else {
    delete data_;
  }
}

void Entry::setBook(class Notebook *nb) {
  if (data_)
    data_->setBook(nb);
  if (lnm_)
    lnm_->setBook(nb);
}

EntryData *Entry::data() const {
  ASSERT(isValid());
  return data_;
}

EntryFile *Entry::file() const {
  ASSERT(hasFile());
  return file_;
}

LateNoteManager *Entry::lateNoteManager() const {
  ASSERT(lnm_);
  return lnm_;
}

bool Entry::needToSave() const {
  if (file_ && file_->needToSave())
    return true;
  if (lnm_ && lnm_->needToSave())
    return true;
  return false;
}

bool Entry::saveNow() const {
  bool ok = true;
  if (file_ && file_->needToSave()) 
    ok &= file_->saveNow();
  if (lnm_)
    ok &= lnm_->saveAll();
  return ok;
}

QSet<QString> Entry::wordSet() const {
  QSet<QString> d = data()->wordSet();
  if (lnm_)
    d |= lnm_->wordSet();
  return d;
}
