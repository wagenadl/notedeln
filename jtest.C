// jtest.C

#include <qjson/serializer.h>
#include <qjson/parser.h>
#include <QFile>
#include <QPointF>
#include <QList>
#include <QMap>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QDebug>

/*
g++ -o jtest -W -Wall jtest.C -lqjson -I/usr/include/qt4 -I/usr/include/qt4/QtCore
*/

int main() {
  double x = 3.14;
  double y = 2.71;
  QPointF p(x, y);
  QString t("Foo bar");
  QList<int> fibo;
  int k0=1;
  int k1=1;
  for (int i=0; i<10; i++) {
    fibo.append(k0);
    int k2 = k0+k1;
    k0 = k1;
    k1 = k2;
  }
  QDateTime now = QDateTime::currentDateTime();
  
  QMap<QString, QString> cities;
  cities["France"] = "Paris";
  cities["Italy"] = "Rome";
  
  QVariantMap m;

  m["x"] = x;
  m["y"] = y;
  // m["p"] = p; // causes failure
  m["t"] = t;
  m["now"] = now;

  QVariantList vl;
  foreach (int f, fibo)
    vl.append(f);

  QVariantMap vm;
  foreach (QString k, cities.keys())
    vm[k] = cities[k];
  
  m["fibo"] = vl;
  m["cities"] = vm;

  QJson::Serializer s;
  QFile foo("/tmp/foo.json");
  bool ok = foo.open(QFile::WriteOnly);
  qDebug() << "open: " << ok;

  QVariant v(m);

  QByteArray ba = s.serialize(v);
  foo.write(ba);
  foo.close();

  QFile bar("/tmp/foo.json");
  ok = bar.open(QFile::ReadOnly);
  qDebug() << "open: " << ok;
  QJson::Parser pa;
  QVariant vv = pa.parse(&bar);
  qDebug() << "read: " << vv;

  QDateTime dt = vv.toMap()["now"].toDateTime();
  qDebug() << "Time: " << dt;
  return 0;
}

