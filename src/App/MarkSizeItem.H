// MarkSizeItem.H

#ifndef MARKSIZEITEM_H

#define MARKSIZEITEM_H

#include "ToolItem.H"
#include "GfxMarkData.H"

class MarkSizeItem: public ToolItem {
  Q_OBJECT;
public:
  MarkSizeItem(double ms);
  virtual ~MarkSizeItem();
  virtual void paintContents(QPainter *);
public slots:
  void setColor(QColor);
  void setShape(GfxMarkData::Shape);
  void setMarkSize(double ms);
private:
  double ms;
  QColor c;
  GfxMarkData::Shape shp;
};

#endif
