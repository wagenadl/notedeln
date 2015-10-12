// ResourceMagic/MagicBiblio.cpp - This file is part of eln

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

// MagicBiblio.C

#include "MagicBiblio.h"

#include "JSONFile.h"
#include "Style.h"

#include <QFileInfo>
#include <QDateTime>
#include <QVariantMap>
#include <QRegExp>
#include <QDir>
#include <QDebug>

QVariantMap const &MagicBiblio::biblio(Style const &st) {
  static QVariantMap empty;
  static QMap<QString, QVariantMap> bbls;
  static QMap<QString, QDateTime> lastloaded;
  if (!st.contains("bib-file"))
    return empty;
  QString k = st.string("bib-file");
  QDateTime lastmod = QFileInfo(k).lastModified();
  if (!bbls.contains(k) || lastmod>lastloaded[k]) {
    bbls[k] = JSONFile::load(k);
    lastloaded[k] = lastmod;
  }
  return bbls[k];
}

MagicBiblio::MagicBiblio(QString tag, Style const &st) {
  if (tag.indexOf(QRegExp("[A-Z]"))==0)
    tag_ = tag.mid(1);
  else
    tag_ = tag;
  
  QVariantMap const &bbl(biblio(st));
  if (bbl.contains(tag_))
    ref_ = bbl[tag_].toString();
  
  if (st.contains("bib-dir")) {
    QDir dir(st.string("bib-dir"));
    if (dir.exists(tag + ".pdf"))
      url_ = QUrl("file://" + dir.absoluteFilePath(tag + ".pdf"));
    else if (tag_!=tag && dir.exists(tag_ + ".pdf"))
      url_ = QUrl("file://" + dir.absoluteFilePath(tag_ + ".pdf"));
  }
  // otherwise, url_ will be null
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
