// SimpleNavbar.H

#ifndef SIMPLENAVBAR_H

#define SIMPLENAVBAR_H

#include "Toolbar.H"

class SimpleNavbar: public Toolbar {
  Q_OBJECT;
public:
  SimpleNavbar(QGraphicsScene *scene);
  virtual ~SimpleNavbar();
signals:
  void goRelative(int); // forward by n pages (n may be negative)
protected:
  virtual void doLeftClick(QString);
};

#endif
