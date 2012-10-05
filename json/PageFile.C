// PageFile.C

#include "PageFile.H"
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <qjson/parser.h>
#include <qjson/serializer.h>

double PageFile::saveDelay_ = 10; // save every 10 s
bool PageFile::saveImmediatelyToo_ = false;

void PageFile::setSaveDelay(double t_s) {
  saveDelay_ = t_s;
}

void PageFile::setSaveImmediatelyToo(bool t) {
  saveImmediatelyToo_ = t;
}

PageFile::PageFile(QString fn, QObject *parent):
  QObject(parent),
  data_(0),
  fn_(fn),
  needToSave_(false),
  saveTimer_(0) {
  QFile f(fn);
  if (!f.open(QFile::ReadOnly))
    return;
  
  QJson::Parser parser;
  bool ok;
  QVariant doc = parser.parse(&f, &ok);
  if (!ok) {
    qDebug() << "PageFile: Parse failed: "
	     << parser.errorString()
	     << " at line " << parser.errorLine();
    return;
  }

  data_ = new PageData();
  data_->load(doc.toMap());

  connect(data_, SIGNAL(mod()), this, SLOT(saveSoon()));
}

PageFile::PageFile(PageData *data, QString fn, QObject *parent):
  QObject(parent),
  data_(data),
  fn_(fn),
  needToSave_(false),
  saveTimer_(0) {
  if (data_)
    data_->setParent(this);
  else
    data_ = new PageData();
  connect(data_, SIGNAL(mod()), this, SLOT(saveSoon()));
}

bool PageFile::save() const {
  if (!data_) {
    qDebug() << "PageFile: Nothing to save";
    return false;
  }

  QJson::Serializer s;
  QByteArray ba = s.serialize(QVariant(data_->save()));
  if (ba.isEmpty()) {  
    qDebug() << "PageFile: Serialization failed";
    return false;
    /* Note that the serializer currently only handles

         bool, int (any size), double (and float), QString, QByteArray,

       and lists and maps of any depth, as long as the terminal types are
       restricted to those listed above.
       In addition, anything that QVariant can convert to a QString is
       supported. That includes QDateTime, QUrl, and some others.
       However, QPoint, QFont, etc., are not supported.
       Qt5 will have native JSON support, and I will likely switch to
       using that eventually.
    */
    
    /* As an aside, I discovered that the serializer puts a binary length
       marker in front of the json data if it writes directly to file. */
  }
  
  QFile f(fn_);
  
  if (f.exists()) {
    QFile f0(fn_ + "~");
    if (f0.exists())
      qDebug() << "(PageFile: Removing ancient file)";
    f0.remove();
    qDebug() << "(PageFile: Renaming old file)";
    f.rename(fn_ + "~");
    f.setFileName(fn_);
  }
  
  if (!f.open(QFile::WriteOnly)) {
    qDebug() << "PageFile: Cannot open file for writing";
    return false;
  }

  if (f.write(ba) != ba.size()) {
    qDebug() << "PageFile: Failed to write all contents";
    return false;
  }

  f.close();

  needToSave_ = false;
  return true;
}

PageFile *PageFile::create(QString fn) {
  PageData *data = new PageData();
  PageFile *file = new PageFile(data, fn, 0);

  if (!file->save())
    delete file;

  return file;
}

void PageFile::saveSoon() {
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

void PageFile::saveTimerTimeout() {
  if (needToSave_)
    save();
}

PageFile::~PageFile() {
  if (needToSave_) {
    qDebug() << "PageFile: Caution: PageFile destructed while waiting to save";
    save();
  }
}

PageData *PageFile::data() const {
  return data_;
}

QString PageFile::fileName() const {
  return fn_;
}
