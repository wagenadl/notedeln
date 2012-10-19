// Style.C

#include "Style.H"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <qjson/parser.h>

Style::Style(QString fn) {
  if (fn=="-")
    fn = "/home/wagenaar/progs/eln/style.json";
    //    fn = "/home/wagenaar/.config/eln/style.json";

  QFile f(fn);
  if (f.open(QFile::ReadOnly)) {
    QJson::Parser p;
    bool ok = true;
    QVariant v = p.parse(&f, &ok);
    if (!ok) {
      qDebug() << "Style: JSON parse failed: " 
	       << p.errorString() << " at line " << p.errorLine();
      qFatal("style error");
    }
    options_ = v.toMap();
  } else {
    qDebug() << "Style: File not found";
  }
}

Style const &Style::defaultStyle() {
  static Style s("-");
  // qDebug() << "Default style: " << &s;
  return s;
}

QVariant Style::operator[](QString k) const {
  if (!options_.contains(k))
    qDebug() << "(Style: No value for " << k << ")";
  // qDebug() << "Style[]" << k;
  return options_[k];
}

QVariantMap const &Style::options() const {
  return options_;
}
