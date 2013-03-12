// Navbar.H

#ifndef NAVBAR_H

#define NAVBAR_H

#include "Toolbar.H"

class Navbar: public Toolbar {
  Q_OBJECT;
public:
  Navbar(QGraphicsItem *parent);
  virtual ~Navbar();
signals:
  void goTOC();
  void goEnd();
  void goRelative(int); // forward by n pages (n may be negative)
protected:
  virtual void doLeftClick(QString);
};

#endif
