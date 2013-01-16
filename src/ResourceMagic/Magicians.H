// Magicians.H

#ifndef MAGICIANS_H

#define MAGICIANS_H

#include "Magician.H"
#include "Style.H"

class Magicians {
public:
  Magician const *first(QString refText) const;
  Magician const *next(QString refText, Magician const *) const;
  static Magicians const &magicians(Style const &);
private:
  static QMap<Style const *, Magicians *> &stylemap();
  Magicians(Style const &st);
  QList<Magician const *> mm;  
};

#endif
