// Roman.C

#include "Roman.H"

Roman::Roman(int n): n(n) {
  s = "";
  build(1000, "m");
  build(900, "cm");
  build(500, "d");
  build(400, "cd");
  build(100, "c");
  build(90, "xc");
  build(50, "l");
  build(40, "xl");
  build(10, "x");
  build(9, "ix");
  build(5, "v");
  build(4, "iv");
  build(1, "i");
}

QString Roman::lc() const {
  return s;
}

QString Roman::uc() const {
  return s.toUpper();
}

void Roman::build(int dn, QString c) {
  while (n>=dn) {
    n-=dn;
    s+=c;
  }
}
