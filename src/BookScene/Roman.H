// Roman.H

#ifndef ROMAN_H

#define ROMAN_H

#include <QString>

class Roman {
  /* Simple class to convert integers to Roman numerals */
public:
  Roman(int);
  QString lc() const;
  QString uc() const;
private:
  void build(int dn, QString c);
private:
  int n;
  QString s;
};

#endif
