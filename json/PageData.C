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
  maxSheet = 0;
}

PageData::~PageData() {
}


QList<class BlockData *> const &PageData::blocks() const {
  return blocks_;
}

void PageData::addBlock(BlockData *b) {
  blocks_.append(b);
  b->setParent(this);
  connect(b, SIGNAL(newSheet(int)), SLOT(newSheet()));
  if (b->sheet()>maxSheet) {
    maxSheet = b->sheet();
    emit sheetCountMod();
  }
  markModified();
}

bool PageData::deleteBlock(BlockData *b) {
  int i = blocks_.indexOf(b);
  if (i<0)
    return false;

  delete blocks_[i];
  blocks_.removeAt(i);

  newSheet();

  markModified();
  return true;
}

void PageData::newSheet() {
  int newMax = 0;
  foreach (BlockData *b, blocks_) {
    if (b->sheet()>newMax)
      newMax = b->sheet();
  }
  if (newMax != maxSheet) {
    maxSheet = newMax;
    emit sheetCountMod();
  }
}  

void PageData::loadMore(QVariantMap const &src) {
  title_->load(src["title"].toMap());
  
  foreach (BlockData *bd, blocks_)
    delete bd;
  blocks_.clear();
  maxSheet = 0;

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
    connect(bd, SIGNAL(newSheet(int)), SLOT(newSheet()));
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
  return maxSheet + 1;
}
