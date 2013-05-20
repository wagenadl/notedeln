// LineWidthBar.C

#include "LineWidthBar.H"
#include "LineWidthItem.H"
#include "Mode.H"

static double lineWidths[] = {
  0.5,
  1,
  1.5,
  2,
  3,
  5,
  8,
};
static int nLineWidths = sizeof(lineWidths)/sizeof(*lineWidths);

LineWidthBar::LineWidthBar(Mode *mode, QGraphicsItem *parent):
  Toolbar(parent), mode(mode) {
  for (int i=0; i<nLineWidths; i++) 
    addTool(widthToId(lineWidths[i]), new LineWidthItem(lineWidths[i]));

  mode->setLineWidth(lineWidths[1]);
  select(widthToId(mode->lineWidth()));
  setColor(mode->color());
}

LineWidthBar::~LineWidthBar() {
}

void LineWidthBar::setColor(QColor c) {
  foreach (QGraphicsItem *gi, childItems()) {
    LineWidthItem *i = dynamic_cast<LineWidthItem *>(gi);
    if (i)
      i->setColor(c);
  }
}

void LineWidthBar::doLeftClick(QString id) {
  mode->setLineWidth(idToWidth(id));
}

double LineWidthBar::idToWidth(QString s) {
  return s.toDouble();
}

QString LineWidthBar::widthToId(double w) {
  return QString::number(w, 'f', 2);
}


