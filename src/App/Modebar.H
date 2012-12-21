// Modebar.H

#ifndef MODEBAR_H

#define MODEBAR_H

#include "Toolbar.H"
#include "Mode.H"

class Modebar: public Toolbar {
  Q_OBJECT;
public:
  Modebar(Mode *mode, QGraphicsItem *parent);
  virtual ~Modebar();
protected:
  virtual void doLeftClick(QString id);
private:
  static Mode::M idToMode(QString);
  static QString modeToId(Mode::M);
private:
  Mode *mode;
  
};

#endif
