// FindRegion.H

#ifndef FINDREGION_H

#define FINDREGION_H

#include "HoverRegion.H"

class FindRegion: public HoverRegion {
  Q_OBJECT;
public:
  FindRegion(class MarkupData *md, class TextItem *item,
	      QGraphicsItem *parent);
  virtual ~FindRegion();
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
private:
  QColor c;
};

#endif
