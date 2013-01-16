// Style.C

#include "Style.H"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QColor>
#include <qjson/parser.h>
#include "Assert.H"

Style const &Style::defaultStyle() {
  static Style s;
  return s;
}

Style::Style() {
  QFile f(":/style.json");
  ASSERT(f.open(QFile::ReadOnly));
  QJson::Parser p;
  bool ok = true;
  QVariant v = p.parse(&f, &ok);
  if (!ok) {
    qDebug() << "Style: JSON parse of :/style.json failed: " 
	     << p.errorString() << " at line " << p.errorLine();
    qFatal("style error");
  }
  options_ = v.toMap();
}

Style::Style(QString fn) {
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
  }

  // fill in missing values from default style
  Style const &s0 = defaultStyle();
  foreach (QString k, s0.options_.keys()) 
    if (!options_.contains(k))
      options_[k] = s0[k];
}

QVariant Style::operator[](QString k) const {
  if (options_.contains(k))
    return options_[k];
  qDebug() << "Style: No value for " << k;
  ASSERT(0);
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

QFont Style::font(QString k) const {
  QFont f(string(k + "-family"));
  f.setPointSizeF(real(k + "-size"));
  return f;
}

QColor Style::color(QString k) const {
  QColor c(string(k));
  if (!c.isValid()) {
    qDebug() << "Not a valid color: " << k << string(k);
    ASSERT(0); // this is not actually fatal, so don't crash unless debug
  }
  return c;    
}

QVariantMap const &Style::options() const {
  return options_;
}

bool Style::contains(QString k) const {
  return options_.contains(k);
}
