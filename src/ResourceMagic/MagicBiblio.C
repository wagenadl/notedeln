// MagicBiblio.C

#include "MagicBiblio.H"

#include "JSONFile.H"
#include "Style.H"

#include <QVariantMap>
#include <QRegExp>
#include <QDir>
#include <QDebug>

QVariantMap const &MagicBiblio::biblio(Style const &st) {
  static QVariantMap empty;
  static QMap<QString, QVariantMap> bbls;
  if (!st.contains("bib-file"))
    return empty;
  QString k = st.string("bib-file");
  if (!bbls.contains(k))
    bbls[k] = JSONFile::load(k);
  return bbls[k];
}

MagicBiblio::MagicBiblio(QString tag, Style const &st) {
  qDebug() << "MagicBiblio" << tag;
  if (tag.indexOf(QRegExp("[A-Z]"))==0)
    tag_ = tag.mid(1);
  else
    tag_ = tag;
  
  QVariantMap const &bbl(biblio(st));
  qDebug() << "got" << bbl.size() << "entries";
  if (bbl.contains(tag_))
    ref_ = bbl[tag_].toString();
  
  if (st.contains("bib-dir")) {
    QDir dir(st.string("bib-dir"));
    if (dir.exists(tag + ".pdf"))
      url_ = QUrl("file://" + dir.absoluteFilePath(tag + ".pdf"));
    else if (tag_!=tag && dir.exists(tag_ + ".pdf"))
      url_ = QUrl("file://" + dir.absoluteFilePath(tag_ + ".pdf"));
  }
}

bool MagicBiblio::ok() const {
  return !ref_.isEmpty();
}

QString MagicBiblio::ref() const {
  return ref_;
}

QUrl MagicBiblio::url() const {
  return url_;
}
