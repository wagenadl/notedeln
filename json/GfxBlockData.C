// GfxBlockData.C

#include "GfxBlockData.H"
#include "GfxData.H"

GfxBlockData::GfxBlockData(class PageData *parent):
  BlockData(parent) {
  setType("gfxblock");
}

GfxBlockData::~GfxBlockData() {
}

QList<class GfxData *> const &GfxBlockData::gfx() const{
  return gfx_;
}

void GfxBlockData::addGfx(GfxData *g) {
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
  foreach (GfxData *gd, gfx_)
    delete gd;
  gfx_.clear();

  QVariantList gl = src["gfx"].toList();
  foreach (QVariant g, gl) {
    QVariantMap gm = g.toMap();
    GfxData *gd = dynamic_cast<GfxData*>(Data::create(gm["typ"].toString()));
    Q_ASSERT(gd);
    gd->load(gm);
    gfx_.append(gd);
    gd->setParent(this);
  }
}
  
  
void GfxBlockData::saveMore(QVariantMap &dst) {
  QVariantList gl;
  foreach (GfxData *gd, gfx_) {
    QVariantMap g = gd->save();
    gl.append(g);
  }
  dst["gfx"] = gl;
}

