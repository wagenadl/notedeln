// Toolbars.H

#ifndef TOOLBARS_H

#define TOOLBARS_H

#include "Mode.H"

#include <QGraphicsObject>
#include <Qt>

class Toolbars: public QGraphicsObject {
  Q_OBJECT;
public:
  Toolbars(class Mode *mode, QGraphicsItem *parent=0);
  virtual ~Toolbars();
  void setOrientation(Qt::Orientation);
  Qt::Orientation orientation() const;
  QRectF boundingRect() const { return QRectF(); }
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) { }
  class Navbar *navbar() const; // use this to connect to its signals
  void hideTools();
  void showTools();
public slots:
  void setMode(Mode::M);
private:
  void placeChildren();
private:
  Qt::Orientation orient;
  class Toolbar *modes;
  class Toolbar *mcolors, *lcolors;
  class Toolbar *shapes;
  class Toolbar *sizes;
  class Toolbar *widths;
  class Navbar *nav;
  Mode::M m;
};

#endif
