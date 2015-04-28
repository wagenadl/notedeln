// Random.H

#ifndef RANDOM_H

#define RANDOM_H

#include <QtGlobal>

class Random {
public:
  static quint16 random();
private:
  static bool &inited();
  static void srandom();
};

#endif
