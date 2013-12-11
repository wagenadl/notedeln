// repairtoc/JSONFile.cpp - This file is part of eln

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

// JSONFile.C

#include "JSONFile.H"
#include <QFile>
#include <QDebug>

#include "JSONParser.H"
  

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
  QString serializeMap(QVariantMap const &v, int indent, bool parentIsArray);  
  QString serializeList(QVariantList const &v, int indent, bool parentIsArray);
  QString serializeString(QString s);
  QString serializeDouble(double v);
  QString serialize(QVariant v, int indent=0, bool parentIsArray=false);
  
  QString serializeMap(QVariantMap const &v, int indent, bool parentIsArray) {
    if (v.isEmpty())
      return "{ }";
    QString ind = "";
    for (int k=0; k<indent; k++)
      ind += "  ";
    QString s = parentIsArray ? "{ " : "{\n" + ind + "  ";
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
    // bool lastML = 0;
    foreach (QString k, kk) {
      if (first)
	first = false;
      else
	s += ",\n" + ind + "  ";
      s += serializeString(k);
      s += ": ";
      QString s1 = serialize(v[k], indent+1, false);
      // lastML = s1.contains("\n");
      s += s1;
    }
    s += "\n" + ind + "}";
    // s += (parentIsArray && !lastML) ? " }" : "\n" + ind + "}";
    return s;
  }

  QString serializeList(QVariantList const &v, int indent, bool parentIsArray) {
    if (v.isEmpty())
      return "[ ]";
    QString ind = "";
    for (int k=0; k<indent; k++)
      ind += "  ";
    QString s = parentIsArray ? "[ " : "[\n" + ind + "  ";
    bool first = true;
    //    bool lastML = false;
    foreach (QVariant k, v) {
      if (first)
	first = false;
      else
	s += ",\n" + ind + "  ";
      QString s1 = serialize(k, indent+1, true);
      //      lastML = s1.contains("\n");
      s += s1;
    }
    s += "\n" + ind + "]";
    //    s += (parentIsArray && ! lastML) ? " ]" : "\n" + ind + "]";
    return s;
  }

  QString serializeString(QString s) {
    s.replace("\\", "\\\\");
    s.replace("\"", "\\\"");
    s.replace("\b", "\\b");
    s.replace("\f", "\\f");
    s.replace("\n", "\\n");
    s.replace("\r", "\\r");
    s.replace("\t", "\\t");
    return "\"" + s + "\"";
  }

  QString serializeDouble(double d) {
    QString s = QString::number(d);
    if (!s.contains(".") && !s.contains("e"))
      s += ".0";
    return s;
  }
  
  QString serialize(QVariant v, int indent, bool parentIsArray) {
    if (!v.isValid())
      return QString(); // invalid
    if (v.type()==QVariant::List) 
      return serializeList(v.toList(), indent, parentIsArray);
    else if (v.type()==QVariant::Map)
      return serializeMap(v.toMap(), indent, parentIsArray);
    else if (v.type()==QVariant::String || v.type()==QVariant::ByteArray)
      return serializeString(v.toString());
    else if (v.type()==QVariant::Double)
      return serializeDouble(v.toDouble());
    else if (v.type()==QVariant::Bool)
      return v.toBool() ? "true" : "false";
    else if (v.type() == QVariant::ULongLong )
      return QString::number(v.value<qulonglong>());
    else if (v.canConvert<qlonglong>())
      return QString::number(v.value<qlonglong>());
    else if (v.canConvert<QString>())
      // this will catch QDate, QDateTime, QUrl, ...
      return serializeString(v.toString());
    else
      return QString();
  }

  /* End of adapted section. The rest of this file was written by Daniel Wagenaar. */

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
    ts.setCodec("UTF-8");
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
    JSONParser parser(json);
    try {
      QVariantMap v = parser.readObject();
      parser.assertEnd();
      if (ok)
	*ok = true;
      return v;
    } catch (JSONParser::Error const &e) {
      e.report();
      return QVariantMap();
    }
  }

  QString write(QVariantMap const &src) {
    return serialize(src, 0, true);
  }
  
  bool save(QVariantMap const &src, QString fn) {
    //    QJson::Serializer s;
    //    QByteArray ba = s.serialize(QVariant(src));
    QByteArray ba = serialize(src, 0, true).toUtf8();
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
