// TableData.C

#include "TableData.H"
#include "Assert.H"

static Data::Creator<TableData> c("table");

TableData::TableData(Data *parent): TextData(parent) {
  setType("table");
  nc = nr = 2;
  // in actual practice, this will usu. be overwritten soon
  lengths.resize(nr*nc);
  starts.resize(nr*nc);
  text_ = "\n\n\n\n\n"; // one more than number of cells
  firstInvalidStart = 0;
}

TableData::~TableData() {
}

bool TableData::isEmpty() const {
  return false;
}

unsigned int TableData::rows() const {
  return nr;
}

unsigned int TableData::columns() const {
  return nc;
}

void TableData::setRows(unsigned int r) {
  nr = r;
  lengths.resize(nr*nc);
  starts.resize(nr*nc);
  firstInvalidStart = 0;
  markModified();
}

void TableData::setColumns(unsigned int c) {
  nc = c;
  lengths.resize(nr*nc);
  starts.resize(nr*nc);
  firstInvalidStart = 0;
  markModified();
}

unsigned int TableData::cellLength(unsigned int r, unsigned int c) const {
  ASSERT(r<nr && c<nc);
  return lengths[rc2index(r, c)];
}

void TableData::setCellLength(unsigned int r, unsigned int c,
			      unsigned int len,
			      bool hushhush) {
  ASSERT(r<nr && c<nc);
  unsigned int idx = rc2index(r, c);
  lengths[idx] = len;
  if (firstInvalidStart > idx)
    firstInvalidStart = idx + 1;
  if (!hushhush)
    markModified();
}

unsigned int TableData::cellStart(unsigned int r, unsigned int c) const {
  ASSERT(r<nr && c<nc);
  unsigned int idx = rc2index(r, c);
  if (firstInvalidStart==0) {
    starts[0] = 1;
    firstInvalidStart = 1;
  }
  while (firstInvalidStart<=idx) {
    starts[firstInvalidStart] = starts[firstInvalidStart-1]
      + lengths[firstInvalidStart-1] + 1;
    firstInvalidStart++;
  }
  return starts[idx];
}

QString TableData::cellContents(unsigned int r, unsigned int c) const {
  ASSERT(r<nr && c<nc);
  return text().mid(cellStart(r, c), cellLength(r, c));
}

void TableData::loadMore(QVariantMap const &src) {
  lengths.clear();
  foreach (QVariant v, src["len"].toList())
    lengths.append(v.toInt());
  starts.resize(lengths.size());
  firstInvalidStart = 0;
}

void TableData::saveMore(QVariantMap &dst) const {
  QVariantList xl;
  foreach (unsigned int l, lengths)
    xl.append(QVariant(l));
  dst["len"] = QVariant(xl);
}

