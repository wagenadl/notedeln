// FilePage.C

#include "FilePage.H"
#include <QFile>

FilePage::FilePage(QString fn): data_(0), fn(fn) {
  QFile f(fn);
  if (!f.open(QFile::ReadOnly))
    return;
  if (!doc.setContent(&f))
    return;

  data_ = new DataPage(doc.documentElement().firstChildElement("page"));
}

FilePage::~FilePage() {
  if (data_)
    delete data_;
}

bool FilePage::ok() const {
  return data_ != 0;
}

DataPage &FilePage::data() {
  if (!data_)
    data_ = new DataPage(doc.documentElement()); // this is an ERROR situation
  return *data_;
}

bool FilePage::save() const {
  QFile f(fn);
  if (f.exists()) {
    QFile f0(fn + "~");
    f0.remove();
    f.rename(fn + "~");
    f.setFileName(fn);
  }
  if (!f.open(QFile::WriteOnly)) {
    f.remove();
    return false;
  }
  if (f.write(doc.toByteArray())<0) {
    f.remove();
    return false;
  }
  f.close();
  return true;
}
  

FilePage *FilePage::create(QString fn) {
  QFile f(fn);
  if (f.exists())
    return 0;
  if (!f.open(QFile::WriteOnly))
    return 0;

  QDomDocument d("elnPage");
  QDomElement p = d.createElement("elnPage");
  d.appendChild(p);
  DataPage dp("", p);
  p.appendChild(dp.defn);
  f.write(d.toByteArray());
  f.close();
  
  return new FilePage(fn);
}
