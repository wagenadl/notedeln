// Style.C

#include "Style.H"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QColor>
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
      qDebug() << "Style: JSON parse of " << fn << " failed: " 
	       << p.errorString() << " at line " << p.errorLine();
      qFatal("style error");
    }
    options_ = v.toMap();
  } else {
    qDebug() << "Style: File not found: " << fn;
    qFatal("style error");
  }
}

QVariant Style::operator[](QString k) const {
  if (options_.contains(k))
    return options_[k];
  qDebug() << "Style: No value for " << k;
  // Q_ASSERT(0);
  return QVariant();
}

double Style::real(QString k) const {
  return (*this)[k].toDouble();
}

QString Style::string(QString k) const {
  return (*this)[k].toString();
}

bool Style::flag(QString k) const {
  return (*this)[k].toBool();
}

int Style::integer(QString k) const {
  return (*this)[k].toInt();
}

QColor Style::color(QString k) const {
  return QColor(string(k));
}

QVariantMap const &Style::options() const {
  return options_;
}
