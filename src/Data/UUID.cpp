// UUID.cpp

#include "UUID.h"
#include "Random.h"

QString UUID::create(int bits) {
  QString res;
  for (int k=0; k<bits; k+=16) {
    int x = Random::random();
    for (int l=0; l<16; l+=4) {
      if (l>=bits)
	break;
      int y = x & 15;
      if (y<10)
	res += ('0' + y);
      else
	res += ('a' + y - 10);
      x >>= 4;
    }
  }
  return res;
}
