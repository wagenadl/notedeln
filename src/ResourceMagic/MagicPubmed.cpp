// MagicPubmed.cpp

#include "MagicPubmed.h"
#include <QProcess>

MagicPubmed::MagicPubmed(QString tag, class Style const &) {
  ok_ = false;
  QStringList args;
  args << "-O-";
  args << QString("http://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi"
		  "?db=pubmed&id=%1&rettype=medline&retmode=text").arg(tag);
  QProcess wget;
  wget.start("wget", args);
  if (!wget.waitForFinished(10000))
    return;

  QString res = QString(wget.readAllStandardOutput());
  QStringList bits = res.split("\n");
  QMap<QString, QStringList> tags;
  QString lastkey;
  foreach (QString l, bits) {
    QString key = l.left(4).trimmed();
    QString val = l.mid(6).trimmed();
    if (key=="" && tags.contains(lastkey) && !tags[lastkey].isEmpty()) {
      tags[lastkey].last() += " " + val;
    } else {
      tags[key] << val;
      lastkey = key;
    }      
  }
  if (!tags.contains("PMID"))
    return;

  int nauth = tags["AU"].size();
  if (nauth==1) {
    ref_ = tags["AU"][0];
  } else if (nauth==2) {
    ref_ = tags["AU"][0] + " and " + tags["AU"][1];
  } else {
    ref_ = tags["AU"][0] + " et al.";
  }
  ref_ += ", " + tags["DA"].first().left(4) + ". ";
  ref_ += tags["TI"].first();
  if (!ref_.endsWith("."))
    ref_ += ".";
  ref_ += " <i>" + tags["TA"].first() + "</i>";
  ref_ += " <b>" + tags["VI"].first() + "</b>";
  QString pg = tags["PG"].first();
  pg.replace("-", QString::fromUtf8("–"));
  ref_ += ", " + pg + ".";

  ok_ = true;
}

bool MagicPubmed::ok() const {
  return ok_;
}

QUrl MagicPubmed::url() const {
  return url_;
}

QString MagicPubmed::ref() const {
  return ref_;
}
