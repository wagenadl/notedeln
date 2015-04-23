// Random.cpp

#include "Random.h"
#include <QDateTime>
#include <QHash>
#include <QNetworkInterface>

quint16 Random::random() {
  if (!inited())
    srandom();
  quint16 x = qrand();
  return x;
}

bool &Random::inited() {
  static bool x = false;
  return x;
}

void Random::srandom() {
  qint64 t = QDateTime::currentDateTime().toMSecsSinceEpoch();
  qsrand(t);
  foreach (QHostAddress a, QNetworkInterface::allAddresses())
    t ^= qHash(a);
  inited() = true;
}

    
