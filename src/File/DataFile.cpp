// File/DataFile.cpp - This file is part of eln

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

// DataFile.C

#include "DataFile.H"
#include <QDebug>
#include <QTimer>
#include "JSONFile.H"
#include "Assert.H"
#include "DFBlocker.H"
#include "PointerSet.H"

double DataFile0::saveDelay_ = 10; // save every 10 s

void DataFile0::setSaveDelay(double t_s) {
  saveDelay_ = t_s;
}

DataFile0::DataFile0(QString fn, QObject *parent):
  QObject(parent),
  data_(0),
  fn_(fn),
  needToSave_(false),
  saveTimer_(0) {
  
  QVariantMap v = JSONFile::load(fn, &ok_);

  if (!ok_) {
    qDebug() << "DataFile: failed to load " << fn;
    return;
  }
  data_ = Data::create(v["typ"].toString());
  ok_ = data_!=0;
  if (!ok_) {
    qDebug() << "DataFile: No creator for type "
	     << v["typ"].toString();
    return;
  }

  data_->setParent(this); // just a QObject as a parent
  data_->load(v);
  connect(data_, SIGNAL(mod()), this, SLOT(saveSoon()));
}

DataFile0::DataFile0(Data *data, QString fn, QObject *parent):
  QObject(parent),
  data_(data),
  fn_(fn),
  needToSave_(true),
  saveTimer_(0) {
  ok_ = data_ != 0;
  ASSERT(data_);
  if (!ok_)
    return;
  data_->setParent(this);
  ok_ = saveNow();
  connect(data_, SIGNAL(mod()), this, SLOT(saveSoon()));
}

bool DataFile0::ok() const {
  return ok_;
}

bool DataFile0::saveNow(bool force) const {
  if (force)
    needToSave_ = true;
    
  if (!needToSave_) {
    ok_ = data_!=0;
    return ok_;
  }
  
  if (!data_) {
    qDebug() << "DataFile: Nothing to save";
    return false;
  }

  ok_ = JSONFile::save(data_->save(), fn_);  
  if (ok_)
    needToSave_ = false;
  return ok_;
}

bool DataFile0::needToSave() const {
  return needToSave_;
}

void DataFile0::cancelSave() {
  needToSave_ = false;
}
  

void DataFile0::saveSoon() {
  if (!saveTimer_) {
    saveTimer_ = new QTimer(this);
    connect(saveTimer_, SIGNAL(timeout()),
	    this, SLOT(saveTimerTimeout()));
  }
  
  needToSave_ = true;
  if (!saveTimer_->isActive()) {
    // saveTimer not active: activate it
    saveTimer_->setSingleShot(true);
    saveTimer_->start(int(saveDelay_ * 1e3));
  }
}

void DataFile0::saveTimerTimeout() {
  if (needToSave_) {
    if (isBlocked())
      saveWhenUnblocked();
    else
      saveNow();
  }
}

DataFile0::~DataFile0() {
  if (needToSave_) {
    qDebug() << "DataFile0: Caution: DataFile0 destructed while waiting to save";
    saveNow();
  }
}

Data *DataFile0::data() const {
  return data_;
}

QString DataFile0::fileName() const {
  return fn_;
}

//////////////////////////////////////////////////////////////////////
PointerSet &DataFile0::blockers() {
  static PointerSet *ps = new PointerSet();
  return *ps;
}

PointerSet &DataFile0::saveNeeders() {
  static PointerSet *ps = new PointerSet();
  return *ps;
}

int &DataFile0::maxBlockDur() {
  static int mbd = 120;
  return mbd;
}

void DataFile0::addBlocker(DFBlocker *b) {
  blockers().insert(b);
}

void DataFile0::removeBlocker(DFBlocker *b) {
  bool wasBlocked = isBlocked();
  blockers().remove(b);
  if (wasBlocked && !isBlocked()) {
    foreach (DataFile0 *df, saveNeeders().toList<DataFile0>())
      df->saveNow();
    saveNeeders().clear();
  }
}

bool DataFile0::isBlocked() {
  return !blockers().isEmpty();
}
      
void DataFile0::setMaxBlockDuration(int seconds) {
  maxBlockDur() = seconds;
}

int DataFile0::maxBlockDuration() {
  return maxBlockDur();
}

void DataFile0::saveWhenUnblocked() {
  if (isBlocked())
    saveNeeders().insert(this);
  else
    saveNow();
}

void DataFile0::dontSaveWhenUnblocked() {
  saveNeeders().remove(this);
}
