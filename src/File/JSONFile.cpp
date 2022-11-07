// File/JSONFile.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// JSONFile.C

#include "JSONFile.h"
#include <QFile>
#include <QJsonDocument>
#include <QDebug>


  

namespace JSONFile {

  QVariantMap load(QString fn, bool *ok) {
    if (ok)
      *ok = false;
    QFile f(fn);
    if (!f.open(QFile::ReadOnly)) {
      qDebug() << "JSONFile::load: file not found";
      return QVariantMap();
    }

    bool ok1;
    QTextStream ts(&f);
    //    ts.setCodec("UTF-8"); // default in qt6
    QVariantMap res = read(ts.readAll(), &ok1);
    if (!ok1) 
      qDebug() << "(while reading: " << fn << ")";
    if (ok)
      *ok = ok1;
    return res;
  }

  QVariantMap read(QString json, bool *ok) {
    if (ok)
      *ok = false;
    QJsonParseError err;
    QJsonDocument doc(QJsonDocument::fromJson(json.toUtf8(), &err));
    if (!doc.isObject()) {
      qDebug() << "JSON parse error" << err.errorString();
      return QVariantMap();
    }
    if (ok)
      *ok = true;
    return doc.toVariant().toMap();
  }

  QString write(QVariantMap const &src, bool compact) {
    QJsonDocument doc(QJsonDocument::fromVariant(src));
    QString s(QString::fromUtf8(doc.toJson(compact ? QJsonDocument::Compact
                                           : QJsonDocument::Indented)));
    return s;
  }
  
  bool save(QVariantMap const &src, QString fn, bool compact) {
    //    QJson::Serializer s;
    //    QByteArray ba = s.serialize(QVariant(src));
    QJsonDocument doc(QJsonDocument::fromVariant(src));
    QByteArray ba(doc.toJson(compact ? QJsonDocument::Compact
                             : QJsonDocument::Indented));
    if (ba.isEmpty()) {  
      qDebug() << "DataFile0: Serialization failed";
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
    ba += "\n";
  
    QFile f(fn);
  
    if (f.exists()) {
      QString fn0 = fn + "~";
      QFile f0(fn0);
      if (f0.exists())
	f0.remove();
      f.rename(fn0);
      f.setFileName(fn);
    }
  
    if (!f.open(QFile::WriteOnly)) {
      qDebug() << "JSONFile: Cannot open file for writing";
      return false;
    }

    if (f.write(ba) != ba.size()) {
      qDebug() << "JSONFile: Failed to write all contents";
      return false;
    }

    f.close();

    return true;
  }
};
