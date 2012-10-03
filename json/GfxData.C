// GfxData.C

#include "GfxData.H"

QMap<QString, GfxData *(*)()> &GfxData::creators() {
  static QMap<QString, GfxData *(*)()> g;
  return g;
}

GfxData *GfxData::create(QString t) {
  if (creators().contains(t))
    return creators()[t]();
  else
    return 0;
}
