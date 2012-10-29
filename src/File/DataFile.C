// DataFile.C

#include "DataFile.H"
#include <QDebug>
#include <QTimer>
#include "JSONFile.H"

double DataFile0::saveDelay_ = 10; // save every 10 s
bool DataFile0::saveImmediatelyToo_ = false;

void DataFile0::setSaveDelay(double t_s) {
  saveDelay_ = t_s;
}

void DataFile0::setSaveImmediatelyToo(bool t) {
  saveImmediatelyToo_ = t;
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
  needToSave_(false),
  saveTimer_(0) {
  ok_ = data_ != 0;
  Q_ASSERT(data_);
  if (!ok_)
    return;
  data_->setParent(this);
  ok_ = save();
  connect(data_, SIGNAL(mod()), this, SLOT(saveSoon()));
}

bool DataFile0::ok() const {
  return ok_;
}

bool DataFile0::save(bool onlyIfNeeded) const {
  if (onlyIfNeeded && !needToSave_) {
    ok_ = data_!=0;
    return true;
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

void DataFile0::saveSoon() {
  if (!saveTimer_) {
    saveTimer_ = new QTimer(this);
    connect(saveTimer_, SIGNAL(timeout()),
	    this, SLOT(saveTimerTimeout()));
  }
  
  if (saveTimer_->isActive()) {
    // saveTimer already active: do nothing except mark that we want to save
    needToSave_ = true;
  } else {
    // saveTimer not active: activate it, and optionally save right away
    if (saveImmediatelyToo_) {
      save(); 
      needToSave_ = false;
    } else {
      needToSave_ = true;
    }
    saveTimer_->setSingleShot(true);
    saveTimer_->start(int(saveDelay_ * 1e3));
  }
}

void DataFile0::saveTimerTimeout() {
  if (needToSave_)
    save();
}

DataFile0::~DataFile0() {
  if (needToSave_) {
    qDebug() << "DataFile0: Caution: DataFile0 destructed while waiting to save";
    save();
  }
}

Data *DataFile0::data() const {
  return data_;
}

QString DataFile0::fileName() const {
  return fn_;
}
