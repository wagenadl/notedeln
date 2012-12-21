// Marksizebar.H

#ifndef MARKSIZEBAR_H

#define MARKSIZEBAR_H

#include "Toolbar.H"
#include "GfxMarkData.H"

class MarkSizeBar: public Toolbar {
  Q_OBJECT;
public:
  MarkSizeBar(class Mode *mode, QGraphicsItem *parent);
  virtual ~MarkSizeBar();
public slots:
  void setColor(QColor);
  void setShape(GfxMarkData::Shape);
protected:
  virtual void doLeftClick(QString id);
private:
  static double idToSize(QString);
  static QString sizeToId(double);
private:
  Mode *mode;
};

#endif
