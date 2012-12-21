// Modebar.H

#ifndef MODEBAR_H

#define MODEBAR_H

#include "Toolbar.H"
#include "Mode.H"

class Modebar: public Toolbar {
public:
  Modebar(Mode *mode, QGraphicsView *view);
  virtual ~Modebar();
protected:
  virtual void doLeftClick(QString id);
private:
  static Mode::M stringToMode(QString);
  static QString modeToString(Mode::M);
private:
  Mode *mode;
  
};

#endif
