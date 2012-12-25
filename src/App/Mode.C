// Mode.C

#include "Mode.H"

Mode::Mode(QObject *parent): QObject(parent) {
  m = Browse;
  overridden = Browse;
  lw = 2;
  ms = 10;
  shp = GfxMarkData::SolidCircle;
}

Mode::~Mode() {
}

Mode::M Mode::mode() const {
  return m;
}

double Mode::lineWidth() const {
  return lw;
}

QColor Mode::color() const {
  return c;
}

GfxMarkData::Shape Mode::shape() const {
  return shp;
}
double Mode::markSize() const {
  return ms;
}

void Mode::setMode(Mode::M m1) {
  m = m1;
  emit modeChanged(m);
}

void Mode::setLineWidth(double lw1) {
  lw = lw1;
  emit lineWidthChanged(lw);
}

void Mode::setColor(QColor c1) {
  c = c1;
  emit colorChanged(c);
}

void Mode::setShape(GfxMarkData::Shape s1) {
  shp = s1;
  emit shapeChanged(shp);
}

void Mode::setMarkSize(double ms1) {
  ms = ms1;
  emit markSizeChanged(ms);
}

void Mode::temporaryOverride(Mode::M m1) {
  overridden = m;
  setMode(m1);
}

void Mode::temporaryRelease(Mode::M m1) {
  if (m==m1)
    setMode(overridden);
}
