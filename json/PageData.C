// PageData.C

#include "PageData.H"
#include "BlockData.H"
#include "TitleData.H"

PageData::PageData(Data *parent): Data(parent) {
  setType("page");
  startPage_ = 1;
  title_ = new TitleData(this);
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

bool PageData::deleteBlock(BlockData *b) {
  int i = blocks_.indexOf(b);
  if (i<0)
    return false;

  delete blocks_[i];
  blocks_.removeAt(i);
  markModified();
  return true;
}



void PageData::loadMore(QVariantMap const &src) {
  title_->load(src["title"].toMap());
  
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
  dst["title"] = title_->save();
}

TitleData *PageData::title() const {
  return title_;
}

int PageData::startPage() const {
  return startPage_;
}

void PageData::setStartPage(int s) {
  startPage_ = s;
  markModified(InternalMod);
}


