// PubMedMagician.H

#ifndef PUBMEDMAGICIAN_H

#define PUBMEDMAGICIAN_H

#include "Magician.H"

class PubMedMagician: public Magician {
public:
  virtual bool matches(QString) const; // base version returns true
  virtual QUrl webUrl(QString) const; // base version returns null
  virtual QUrl objectUrl(QString) const; // base version returns null
};

#endif
