// PageData.C

#include "PageData.H"
#include "BlockData.H"
#include "TitleData.H"
#include <QDebug>


static Data::Creator<PageData> c("page");

PageData::PageData(Data *parent): Data(parent) {
  setType("page");
  startPage_ = 1;
  title_ = new TitleData(this);
  connect(title_, SIGNAL(mod()),
	  this, SIGNAL(titleMod()));
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
    QVariantMap bm = b.toMap();
    qDebug() << "PageData: loading block of type " << bm["typ"].toString();
    BlockData *bd = dynamic_cast<BlockData*>(Data::create(bm["typ"].
							  toString()));
    Q_ASSERT(bd);
    bd->load(bm);
    blocks_.append(bd);
    bd->setParent(this);
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

int PageData::sheetCount() const {
  int maxSheet = 0;
  foreach (BlockData *b, blocks_) {
    int s = b->sheet();
    if (s>maxSheet)
      maxSheet = s;
  }
  return maxSheet + 1;
}
