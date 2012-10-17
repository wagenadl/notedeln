// GfxBlockData.C

#include "GfxBlockData.H"
#include "GfxData.H"
#include <QDebug>

static Data::Creator<GfxBlockData> c("gfxblock");

GfxBlockData::GfxBlockData(class PageData *parent):
  BlockData(parent) {
  setType("gfxblock");
  xref_ = yref_ = 0;
}

GfxBlockData::~GfxBlockData() {
}

QList<class GfxData *> const &GfxBlockData::gfx() const{
  return gfx_;
}

void GfxBlockData::addGfx(GfxData *g) {
  qDebug() << "GBD: addGfx" << g;
  gfx_.append(g);
  g->setParent(this);
  markModified();
}

void GfxBlockData::removeGfx(GfxData *g) {
  if (gfx_.removeOne(g)) {
    delete g;
    markModified();
  }
}

void GfxBlockData::loadMore(QVariantMap const &src) {
  qDebug() << "GfxBlockData::loadMore";
  
  foreach (GfxData *gd, gfx_)
    delete gd;
  gfx_.clear();

  QVariantList gl = src["gfx"].toList();
  foreach (QVariant g, gl) {
    QVariantMap gm = g.toMap();
    Data *g0 = Data::create(gm["typ"].toString());
    if (!g0) 
      qDebug() << "Failed to create data of type " << gm["typ"].toString();
    Q_ASSERT(g0);
    GfxData *gd = dynamic_cast<GfxData*>(g0);
    if (!gd)
      qDebug() << "Failed to cast data of type " << gm["typ"].toString()
	       << "to GfxData";
    Q_ASSERT(gd);
    gd->load(gm);
    gfx_.append(gd);
    gd->setParent(this);
  }
}
  
  
void GfxBlockData::saveMore(QVariantMap &dst) const {
  qDebug() << "GfxBlockData::saveMore";
  QVariantList gl;
  foreach (GfxData *gd, gfx_) {
    QVariantMap g = gd->save();
    gl.append(g);
  }
  dst["gfx"] = gl;
}

double GfxBlockData::xref() const {
  return xref_;
}

double GfxBlockData::yref() const {
  return yref_;
}

QPointF GfxBlockData::ref() const {
  return QPointF(xref_, yref_);
}

void GfxBlockData::setXref(double x) {
  xref_ = x;
  markModified();
}


void GfxBlockData::setYref(double y) {
  yref_ = y;
  markModified();
}


void GfxBlockData::setRef(QPointF xy) {
  xref_ = xy.x();
  yref_ = xy.y();
  markModified();
}
