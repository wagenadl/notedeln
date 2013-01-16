// DWBibMagician.H

#ifndef DWBIBMAGICIAN_H

#define DWBIBMAGICIAN_H

#include "Magician.H"

class DWBibMagician: public Magician {
public:
  DWBibMagician();
  DWBibMagician(class Style const &);
  virtual bool matches(QString) const;
  virtual QUrl objectUrl(QString) const; // pdf if available
  virtual QString title(QString) const; // html-formatted reference
private:
  Style const &style;
};

#endif
