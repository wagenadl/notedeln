// SearchResItem.H

#ifndef SEARCHRESITEM_H

#define SEARCHRESITEM_H

#include "TOCItem.H"
#include "Search.H"

class SearchResItem: public TOCItem {
  Q_OBJECT;
public:
  SearchResItem(class TOCEntry *data, class BaseScene *parent);
  virtual ~SearchResItem();
  DATAACCESS(TOCEntry);
  void reset();
  void addResult(SearchResult const &res, QGraphicsItem *parent);
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
private:
  int refineBreak(QString s, int idx);
  int decentBreak(QString s, int first, int last);
  void fillText(class QTextDocument *doc, SearchResult const &res);
private:
  QList<class QGraphicsTextItem *> items;
  QList<double> ytop;
  QList<int> pgno;
};

#endif
