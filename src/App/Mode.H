// Mode.H

#ifndef MODE_H

#define MODE_H

#include <QObject>
#include "GfxMarkData.H"

class Mode: public QObject {
  Q_OBJECT;
public:
  enum M {
    Browse,
    Type,
    MoveResize,
    Mark,
    Freehand,
    Annotate
  };
public:
  Mode(QObject *parent=0);
  virtual ~Mode();
  M mode() const;
  double lineWidth() const;
  QColor color() const;
  GfxMarkData::Shape shape() const;
  double markSize() const;
public slots:
  void setMode(Mode::M);
  void setLineWidth(double);
  void setColor(QColor);
  void setShape(GfxMarkData::Shape);
  void setMarkSize(double);
signals:
  void modeChanged(Mode::M);
  void lineWidthChanged(double);
  void colorChanged(QColor);
  void shapeChanged(GfxMarkData::Shape);
  void markSizeChanged(double);
private:
  M m;
  double lw;
  QColor c;
  GfxMarkData::Shape shp;
  double ms;
};

#endif
