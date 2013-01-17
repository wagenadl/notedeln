// DWBibMagician.H

#ifndef DWBIBMAGICIAN_H

#define DWBIBMAGICIAN_H

#include "Magician.H"

class DWBibMagician: public Magician {
public:
  DWBibMagician();
  DWBibMagician(class Style const &);
  virtual bool matches(QString) const;
  virtual QUrl objectUrl(QString) const;
  virtual QString title(QString) const; 
  //  virtual bool keepAlways(QString) const;
private:
  Style const &style;
};

#endif
