// TableData_HighLevel.cpp
// High-level row and column operations

#include "TableData.h"
#include "MarkupData.h"
#include <QDebug>

//////////////////////////////////////////////////////////////////////
// Utility classes

class CellPos { // a position within a cell
public:
  CellPos(): r(0), c(0), off(0) { }
  CellPos(int pos, TableData const *data) {
    TableCell cel = data->cellAt(pos);
    r = cel.row();
    c = cel.column();
    off = pos - cel.firstPosition();
  }
  CellPos(int r, int c, int off): r(r), c(c), off(off) { }
public:
  int row() const { return r; }
  int column() const { return c; }
  int offset() const { return off; }
  int globalOffset(TableData const *data) const {
    return data->cellStart(r,c) + off;
  }
  QString toString() const {
    return QString("(%1,%2):%3").arg(r).arg(c).arg(off);
  }
public:
  void insertRow(int beforer) {
    if (r>=beforer)
      r++;
  }
  void insertColumn(int beforec) {
    if (c>=beforec)
      c++;
  }
  void deleteRow(int rdel) {
    if (r==rdel) 
      off = 0;
    else if (r>rdel)
      r--;
  }
  void deleteColumn(int cdel) {
    if (c==cdel)
      off = 0;
    else if (c>cdel)
      c--;
  }
private:
  int r, c, off;
};

class MDPos { // two positions within cells
public:
  MDPos() { }
  MDPos(MarkupData const *md, TableData const *data):
    s(CellPos(md->start(), data)),
    e(CellPos(md->end(), data)) { }
public:
  CellPos const &start() const { return s; }
  CellPos const &end() const { return e; }
  QString toString() const { return s.toString() + "-" + e.toString(); }
public:
  void insertRow(int r) { s.insertRow(r); e.insertRow(r); }
  void deleteRow(int r) { s.deleteRow(r); e.deleteRow(r); }
  void insertColumn(int c) { s.insertColumn(c); e.insertColumn(c); }
  void deleteColumn(int c) { s.deleteColumn(c); e.deleteColumn(c); }
private:
  CellPos s, e;
};

class RCPair {
public:
  RCPair(int r=0, int c=0): r(r), c(c) { }
  int row() const { return r; }
  int column() const { return c; }
  bool operator<(RCPair const &rc) const {
    if (r<rc.r)
      return true;
    else if (r>rc.r)
      return false;
    else
      return c<rc.c;
  }
private:
  int r, c;
};
  

//////////////////////////////////////////////////////////////////////
// Utility functions

static QMap<RCPair, QString> allContents(TableData const *data) {
  // Texts of all the cells
  // The RCPairs are actually (r,c) pairs
  QMap<RCPair, QString> txts;
  int R = data->rows();
  int C = data->columns();
  for (int r=0; r<R; r++)
    for (int c=0; c<C; c++)
      txts[RCPair(r,c)] = data->cellContents(r, c);
  return txts;
}

static QMap<MarkupData *, MDPos> allMarkupPos(TableData const *data) {
  // Positions of all markups
  QMap<MarkupData *, MDPos> mdp;
  for (MarkupData *md: data->markups())
    mdp[md] = MDPos(md, data);
  return mdp;
};

static QString buildContents(int R, int C, QMap<RCPair, QString> allcont) {
  QString out = "\n";
  for (int r=0; r<R; r++)
    for (int c=0; c<C; c++)
      out += allcont[RCPair(r, c)] + "\n";
  return out;
}

static void updateMD(MarkupData *md, MDPos const &pos,
		     TableData const *data) {
  md->setStart(pos.start().globalOffset(data));
  md->setEnd(pos.end().globalOffset(data));
}

//////////////////////////////////////////////////////////////////////
// High-level functions

void TableData::insertRow(int beforerow) {
  int R = rows();
  int C = columns();
  if (beforerow<0)
    beforerow = 0;
  if (beforerow>R)
    beforerow = R;

  QMap<RCPair, QString> oldcont = allContents(this);
  QMap<RCPair, QString> newcont;
  for (int r=0; r<R; r++)
    for (int c=0; c<C; c++)
      newcont[RCPair(r>=beforerow ? r+1 : r, c)] = oldcont[RCPair(r, c)];
  
  QMap<MarkupData *, MDPos> allmarks = allMarkupPos(this);
  for (auto &pos: allmarks) 
    pos.insertRow(beforerow);
  
  setRows(R+1);
  setText(buildContents(R+1, C, newcont));
  for (auto *md: markups())
    updateMD(md, allmarks[md], this);
}

void TableData::insertColumn(int beforecol) {
  int R = rows();
  int C = columns();
  if (beforecol<0)
    beforecol = 0;
  if (beforecol>C)
    beforecol = C;

  qDebug() << "old text[" << text() << "]";

  QMap<RCPair, QString> oldcont = allContents(this);
  QMap<RCPair, QString> newcont;
  for (int r=0; r<R; r++)
    for (int c=0; c<C; c++)
      newcont[RCPair(r, c>=beforecol ? c+1 : c)] = oldcont[RCPair(r, c)];
  
  QMap<MarkupData *, MDPos> allmarks = allMarkupPos(this);

  for (auto pos: allmarks) 
    qDebug() << "old pos" << pos.toString();
  
  for (auto &pos: allmarks) 
    pos.insertColumn(beforecol);

  for (auto pos: allmarks) 
    qDebug() << "new pos" << pos.toString();
  
  qDebug() << "new text[" << buildContents(R, C+1, newcont) << "]";

  setColumns(C+1);
  setText(buildContents(R, C+1, newcont));
  for (auto *md: markups())
    updateMD(md, allmarks[md], this);
}

void TableData::deleteRow(int row) {
  int R = rows();
  int C = columns();
  if (row<0 || row>=R || R==1)
    return;

  QMap<RCPair, QString> oldcont = allContents(this);
  QMap<RCPair, QString> newcont;
  for (int r=0; r<R-1; r++)
    for (int c=0; c<C; c++)
      newcont[RCPair(r, c)] = oldcont[RCPair(r>=row ? r+1 : r, c)];
  
  QMap<MarkupData *, MDPos> allmarks = allMarkupPos(this);
  for (auto &pos: allmarks) 
    pos.deleteRow(row);
  
  setRows(R-1);
  setText(buildContents(R-1, C, newcont));
  for (auto *md: markups()) {
    updateMD(md, allmarks[md], this);
    if (md->start()>=md->end())
      deleteMarkup(md);
  }
}

void TableData::deleteColumn(int col) {
  int R = rows();
  int C = columns();
  if (col<0 || col>=C || C==1)
    return;

  qDebug() << "old text[" << text() << "]";

  QMap<RCPair, QString> oldcont = allContents(this);
  QMap<RCPair, QString> newcont;
  for (int r=0; r<R; r++)
    for (int c=0; c<C-1; c++)
      newcont[RCPair(r, c)] = oldcont[RCPair(r, c>=col ? c+1 : c)];
  
  QMap<MarkupData *, MDPos> allmarks = allMarkupPos(this);

  for (auto pos: allmarks) 
    qDebug() << "old pos" << pos.toString();

  for (auto &pos: allmarks) 
    pos.deleteColumn(col);
  
  for (auto pos: allmarks) 
    qDebug() << "new pos" << pos.toString();

  qDebug() << "new text[" << buildContents(R, C-1, newcont) << "]";

  setColumns(C-1);
  setText(buildContents(R, C-1, newcont));
  for (auto *md: markups()) {
    updateMD(md, allmarks[md], this);
    if (md->start()>=md->end())
      deleteMarkup(md);
  }
}
