// PageData.C

#include "PageData.H"
#include "BlockData.H"
#include "TitleData.H"
#include <QDebug>
#include "Notebook.H"

static Data::Creator<PageData> c("page");

PageData::PageData(Data *parent): Data(parent) {
  nb = 0;
  setType("page");
  startPage_ = 1;
  title_ = new TitleData(this);
  addChild(title_);
  connect(title_, SIGNAL(mod()), SIGNAL(titleMod()));
  maxSheet = 0;
}

PageData::~PageData() {
}


QList<class BlockData *> PageData::blocks() const {
  return children<BlockData>();
}

bool PageData::isEmpty() const {
  return blocks().isEmpty();
}

void PageData::addBlock(BlockData *b) {
  connect(b, SIGNAL(newSheet(int)), SLOT(newSheet()));
  if (b->sheet()>maxSheet) {
    maxSheet = b->sheet();
    emit sheetCountMod();
  }
  addChild(b);
}

bool PageData::deleteBlock(BlockData *b) {
  if (!deleteChild(b))
    return false;

  newSheet();
  return true;
}

void PageData::newSheet() {
  int newMax = 0;
  // I think we can actually assume the last block is on the last page,
  // but I am going to be bloody minded about it.
  foreach (BlockData *b, blocks()) {
    if (b->sheet()>newMax)
      newMax = b->sheet();
  }
  bool doEmit = maxSheet>=0;
  if (newMax != maxSheet) {
    maxSheet = newMax;
    if (doEmit)
      emit sheetCountMod();
  }
}  

void PageData::loadMore(QVariantMap const &src) {
  Data::loadMore(src);
  title_ = firstChild<TitleData>();
  // Any old title has already been destructed by Data's loadChildren()
  Q_ASSERT(title_);
  connect(title_, SIGNAL(mod()), SIGNAL(titleMod()));

  maxSheet = 0;
  foreach (BlockData *b, blocks()) {
    if (b->sheet() > maxSheet)
      maxSheet = b->sheet();
    connect(b, SIGNAL(newSheet(int)), SLOT(newSheet()));
  }
}

TitleData *PageData::title() const {
  return title_;
}

QString PageData::titleText() const {
  return title_->current()->text();
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

void PageData::setBook(Notebook *x) {
  nb = x;
}

Notebook *PageData::book() const {
  return nb;
}

ResourceManager *PageData::resMgr() const {
  if (!this)
    return 0; // for convenience, allow to be called without a page
  
  if (nb) 
    return nb->resMgr(startPage_);
  else
    return 0;
}

PageData const *PageData::page() const {
  return this;
}

PageData *PageData::page() {
  return this;
}

