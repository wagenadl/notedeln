// UUID.cpp

#include "UUID.H"
#include "Random.H"

// This is a lame version

QString UUID::create(int bits) {
  QString res;
  for (int k=0; k<bits; k+=4) {
    quint16 r = Random::random();
    r = r & 15;
    if (r<10)
      res += '0' + r;
    else
      res += 'a' + r - 10;
  }
  return res;
}
