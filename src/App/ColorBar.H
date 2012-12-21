// Colorbar.H

#ifndef COLORBAR_H

#define COLORBAR_H

#include "Toolbar.H"
#include "GfxMarkData.H"

class ColorBar: public Toolbar {
  Q_OBJECT;
public:
  ColorBar(class Mode *mode, bool asLine, QGraphicsItem *parent);
  virtual ~ColorBar();
public slots:
  void setMarkSize(double);
  void setLineWidth(double);
  void setShape(GfxMarkData::Shape);
  void setColor(QColor c);
protected:
  virtual void doLeftClick(QString id);
private:
  static QColor idToColor(QString);
  static QString colorToId(QColor);
private:
  Mode *mode;
};

#endif
