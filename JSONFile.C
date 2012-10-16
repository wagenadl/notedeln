// JSONFile.C

#include "JSONFile.H"
#include <qjson/parser.h>
#include <qjson/serializer.h>
#include <QFile>
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
    
    QJson::Parser parser;
    bool ok1;
    QVariant doc = parser.parse(&f, &ok1);
    f.close();
    if (!ok1) {
      qDebug() << "JSONFile: Parse failed: "
	       << parser.errorString()
	       << " at line " << parser.errorLine();
      return QVariantMap();
    }

    if (ok)
      *ok = true;
    return doc.toMap();
  }

  bool save(QVariantMap const &src, QString fn) {
    QJson::Serializer s;
    QByteArray ba = s.serialize(QVariant(src));
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
  
    QFile f(fn);
  
    if (f.exists()) {
      QString fn0 = fn + "~";
      QFile f0(fn0);
      if (f0.exists())
	qDebug() << "(JSONFile: Removing ancient file)";
      f0.remove();
      qDebug() << "(JSONFile: Renaming old file)";
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
