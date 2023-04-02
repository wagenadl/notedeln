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
  /* The following portion of this file has been adapted from the source code of
     qjson. The following copyright message applies:
   */
/* This file is part of qjson
  *
  * Copyright (C) 2009 Till Adam <adam@kde.org>
  * Copyright (C) 2009 Flavio Castelli <flavio@castelli.name>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */
  class Serializer {
  public:
    Serializer(bool compact=false): compact(compact) { }
    QString serializeMap(QVariantMap const &v,
                         int indent, bool parentIsArray);  
    QString serializeList(QVariantList const &v,
                          int indent, bool parentIsArray);
    QString serializeString(QString s);
    QString serializeDouble(double v);
    QString serialize(QVariant v,
                      int indent=0, bool parentIsArray=false);
  private:
    bool compact;
  };

  QString Serializer::serializeMap(QVariantMap const &v,
                                   int indent, bool parentIsArray) {
    if (v.isEmpty())
      return compact ? "{}" : "{ }";
    QString ind = "";
    if (!compact) 
      for (int k=0; k<indent; k++)
        ind += "  ";
    QString s = compact ? "{"
      : (parentIsArray ? "{ " : "{\n" + ind + "  ");
    bool first = true;
    QList<QString> kk = v.keys();
    // do a little reordering
    if (kk.removeOne("mod"))
      kk.insert(0, "mod");
    if (kk.removeOne("cre"))
      kk.insert(0, "cre");
    if (kk.removeOne("typ"))
      kk.insert(0, "typ");
    if (kk.removeOne("cc"))
      kk.append("cc");
    foreach (QString k, kk) {
      if (first)
	first = false;
      else
	s += compact ? ",\n" : (",\n" + ind + "  ");
      s += serializeString(k);
      s += compact ? ":" : ": ";
      QString s1 = serialize(v[k], indent+1, false);
      // lastML = s1.contains("\n");
      s += s1;
    }
    s += compact ? "}" : ("\n" + ind + "}");
    return s;
  }

  QString Serializer::serializeList(QVariantList const &v,
                                    int indent, bool parentIsArray) {
    if (v.isEmpty())
      return compact ? "[]" : "[ ]";
    QString ind = "";
    if (!compact)
      for (int k=0; k<indent; k++)
        ind += "  ";
    QString s = compact ? "[" : (parentIsArray ? "[ " : "[\n" + ind + "  ");
    bool first = true;
    foreach (QVariant k, v) {
      if (first)
	first = false;
      else
	s += compact ? "," : (",\n" + ind + "  ");
      QString s1 = serialize(k, indent+1, true);
      s += s1;
    }
    s += compact ? "]" : ("\n" + ind + "]");
    return s;
  }

  QString Serializer::serializeString(QString s) {
    s.replace("\\", "\\\\");
    s.replace("\"", "\\\"");
    s.replace("\b", "\\b");
    s.replace("\f", "\\f");
    s.replace("\n", "\\n");
    s.replace("\r", "\\r");
    s.replace("\t", "\\t");
    return "\"" + s + "\"";
  }

  QString Serializer::serializeDouble(double d) {
    QString s = QString::number(d);
    if (!s.contains(".") && !s.contains("e"))
      s += ".0";
    return s;
  }
  
  QString Serializer::serialize(QVariant v, int indent, bool parentIsArray) {
    if (!v.isValid())
      return QString(); // invalid
    switch (v.typeId()) {
    case QMetaType::QVariantList:
      return serializeList(v.toList(), indent, parentIsArray);
    case QMetaType::QVariantMap:
      return serializeMap(v.toMap(), indent, parentIsArray);
    case QMetaType::QString: 
      return serializeString(v.toString());
    case QMetaType::QByteArray:
      return serializeString(v.toString());
    case QMetaType::Double:
      return serializeDouble(v.toDouble());
    case QMetaType::Bool:
      return v.toBool() ? "true" : "false";
    case QMetaType::ULongLong:
      return QString::number(v.value<qulonglong>());
    default:
      if (v.canConvert<qlonglong>())
        return QString::number(v.value<qlonglong>());
      else if (v.canConvert<QString>())
        // this will catch QDate, QDateTime, QUrl, ...
        return serializeString(v.toString());
      qDebug() << "json serialize failure" << v;
      return QString();
    }
  }

  /* End of adapted section.
     The rest of this file was written by Daniel Wagenaar. */

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
    Serializer s(compact);
    return s.serialize(src, 0, true);
    /*
    QJsonDocument doc(QJsonDocument::fromVariant(src));
    QString s(QString::fromUtf8(doc.toJson(compact ? QJsonDocument::Compact
                                           : QJsonDocument::Indented)));
    return s;
    */
  }
  
  bool save(QVariantMap const &src, QString fn, bool compact) {
    //    QJson::Serializer s;
    //    QByteArray ba = s.serialize(QVariant(src));
    Serializer s(compact);
    QByteArray ba = s.serialize(src, 0, true).toUtf8();

    /*
    QJsonDocument doc(QJsonDocument::fromVariant(src));
    QByteArray ba(doc.toJson(compact ? QJsonDocument::Compact
                             : QJsonDocument::Indented));
    */
    
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
