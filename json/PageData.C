// PageData.C

#include "PageData.H"
#include "BlockData.H"

PageData::PageData(Data *parent): Data(parent) {
}

PageData::~PageData() {
}


QList<class BlockData *> const &PageData::blocks() const {
  return blocks_;
}

void PageData::addBlock(BlockData *b) {
  blocks_.append(b);
  b->setParent(this);
  markModified();
}

void PageData::loadMore(QVariantMap const &src) {
  foreach (BlockData *bd, blocks_)
    delete bd;
  blocks_.clear();

  QVariantList bl = src["blocks"].toList();
  foreach (QVariant b, bl) {
    BlockData *bd = new BlockData(this);
    bd->load(b.toMap());
    blocks_.append(bd);
  }
}

void PageData::saveMore(QVariantMap &dst) const {
  QVariantList bl;
  foreach (BlockData *bd, blocks_) {
    QVariantMap b = bd->save();
    bl.append(b);
  }
  dst["blocks"] = bl;
}

