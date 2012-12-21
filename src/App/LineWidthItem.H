// LineWidthItem.H

#ifndef LINEWIDTHITEM_H

#define LINEWIDTHITEM_H

#include "ToolItem.H"

class LineWidthItem: public ToolItem {
  Q_OBJECT;
public:
  LineWidthItem(double lw);
  virtual ~LineWidthItem();
  virtual void paintContents(QPainter *);
public slots:
  void setColor(QColor);
  void setLineWidth(double);
private:
  double lw;
  QColor c;
};

#endif
