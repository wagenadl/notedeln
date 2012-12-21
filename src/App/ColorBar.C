// ColorBar.C

#include "ColorBar.H"
#include "MarkSizeItem.H"
#include "LineWidthItem.H"
#include "Mode.H"
#include <QDebug>

static char const *colors[] = {
  "#000000",
  "#787878",
  "#C7C7C7",
  "#FFFFFF",
  "#EE2200",
  "#EE5F00",
  "#FFD400",
  "#005F00",
  "#00D400",
  "#0000CF",
  "#4C84FF",
  "#C400FF",
};

static int nColors = sizeof(colors)/sizeof(*colors);

ColorBar::ColorBar(Mode *mode, bool asLine, QGraphicsItem *parent):
  Toolbar(parent), mode(mode) {
  for (int i=0; i<nColors; i++) {
    QColor c(colors[i]);
    if (asLine) {
      LineWidthItem *it = new LineWidthItem(4);
      it->setColor(c);
      addTool(colorToId(c), it);
    } else {
      MarkSizeItem *it = new MarkSizeItem(12);
      it->setColor(c);
      addTool(colorToId(c), it);
    }
  }
  mode->setColor(colors[0]);
  select(colorToId(colors[0]));
  setShape(mode->shape());
  setMarkSize(mode->markSize());
  setLineWidth(mode->lineWidth());
}

ColorBar::~ColorBar() {
}

void ColorBar::setLineWidth(double s) {
  foreach (QGraphicsItem *gi, childItems()) {
   LineWidthItem *i = dynamic_cast<LineWidthItem *>(gi);
    if (i)
      i->setLineWidth(s);
  }
}

void ColorBar::setMarkSize(double s) {
  foreach (QGraphicsItem *gi, childItems()) {
    MarkSizeItem *i = dynamic_cast<MarkSizeItem *>(gi);
    if (i)
      i->setMarkSize(s);
  }
}

void ColorBar::setShape(GfxMarkData::Shape s) {
  foreach (QGraphicsItem *gi, childItems()) {
    MarkSizeItem *i = dynamic_cast<MarkSizeItem *>(gi);
    if (i)
      i->setShape(s);
  }
}

void ColorBar::setColor(QColor c) {
  select(colorToId(c));
}

void ColorBar::doLeftClick(QString id) {
  mode->setColor(idToColor(id));
}

QColor ColorBar::idToColor(QString s) {
  return QColor(s);
}

QString ColorBar::colorToId(QColor c) {
  return c.name();
}


