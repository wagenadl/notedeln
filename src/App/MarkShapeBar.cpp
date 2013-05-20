// MarkShapeBar.C

#include "MarkShapeBar.H"
#include "MarkSizeItem.H"
#include "Mode.H"
#include <QDebug>

MarkShapeBar::MarkShapeBar(Mode *mode, QGraphicsItem *parent):
  Toolbar(parent), mode(mode) {
  for (int i=0; i<=GfxMarkData::LAST; i++) {
    GfxMarkData::Shape s = (GfxMarkData::Shape)(i);
    MarkSizeItem *it = new MarkSizeItem(mode->markSize());
    it->setShape(s);
    addTool(shapeToId(s), it);
  }
  mode->setShape(GfxMarkData::SolidCircle);
  select(shapeToId(mode->shape()));
  setColor(mode->color());
}

MarkShapeBar::~MarkShapeBar() {
}

void MarkShapeBar::setColor(QColor c) {
  foreach (QGraphicsItem *gi, childItems()) {
    MarkSizeItem *i = dynamic_cast<MarkSizeItem *>(gi);
    if (i)
      i->setColor(c);
  }
}

void MarkShapeBar::setMarkSize(double s) {
  foreach (QGraphicsItem *gi, childItems()) {
    MarkSizeItem *i = dynamic_cast<MarkSizeItem *>(gi);
    if (i)
      i->setMarkSize(s);
  }
}
 
void MarkShapeBar::doLeftClick(QString id) {
  mode->setShape(idToShape(id));
}

GfxMarkData::Shape MarkShapeBar::idToShape(QString s) {
  return (GfxMarkData::Shape)(s.toInt());
}

QString MarkShapeBar::shapeToId(GfxMarkData::Shape s) {
  return QString::number(int(s));
}


