// MarkSizeBar.C

#include "MarkSizeBar.H"
#include "MarkSizeItem.H"
#include "Mode.H"
#include <QDebug>

static double markSizes[] = {
  2,
  3,
  4.5,
  6.5,
  9,
  12.5,
};

static int nMarkSizes = sizeof(markSizes)/sizeof(*markSizes);

MarkSizeBar::MarkSizeBar(Mode *mode, QGraphicsItem *parent):
  Toolbar(parent), mode(mode) {
  for (int i=0; i<nMarkSizes; i++) 
    addTool(sizeToId(markSizes[i]), new MarkSizeItem(markSizes[i]));

  mode->setMarkSize(markSizes[2]);
  select(sizeToId(mode->markSize()));
  setShape(mode->shape());
}

MarkSizeBar::~MarkSizeBar() {
}

void MarkSizeBar::setColor(QColor c) {
  foreach (QGraphicsItem *gi, childItems()) {
    MarkSizeItem *i = dynamic_cast<MarkSizeItem *>(gi);
    if (i)
      i->setColor(c);
  }
}

void MarkSizeBar::setShape(GfxMarkData::Shape s) {
  foreach (QGraphicsItem *gi, childItems()) {
    MarkSizeItem *i = dynamic_cast<MarkSizeItem *>(gi);
    if (i)
      i->setShape(s);
  }
}


void MarkSizeBar::doLeftClick(QString id) {
  qDebug() << "MarkSizeBar" << idToSize(id);
  mode->setMarkSize(idToSize(id));
}

double MarkSizeBar::idToSize(QString s) {
  return s.toDouble();
}

QString MarkSizeBar::sizeToId(double w) {
  return QString::number(w, 'f', 2);
}


