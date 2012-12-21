// Markshapebar.H

#ifndef MARKSHAPEBAR_H

#define MARKSHAPEBAR_H

#include "Toolbar.H"
#include "GfxMarkData.H"

class MarkShapeBar: public Toolbar {
  Q_OBJECT;
public:
  MarkShapeBar(class Mode *mode, QGraphicsItem *parent);
  virtual ~MarkShapeBar();
public slots:
  void setColor(QColor);
  void setMarkSize(double);
protected:
  virtual void doLeftClick(QString id);
private:
  static GfxMarkData::Shape idToShape(QString);
  static QString shapeToId(GfxMarkData::Shape);
private:
  Mode *mode;
};

#endif
