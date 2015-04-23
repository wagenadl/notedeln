// SearchResItem.H

#ifndef SEARCHRESITEM_H

#define SEARCHRESITEM_H

#include "TOCItem.h"
#include "Search.h"

class SearchResItem: public TOCItem {
  Q_OBJECT;
public:
  SearchResItem(class TOCEntry *data, class BaseScene *parent);
  virtual ~SearchResItem();
  DATAACCESS(TOCEntry);
  void reset();
  void addResult(SearchResult const &res, QGraphicsItem *parent);
signals:
  void clicked(int, Qt::KeyboardModifiers, QString); // page number; uuid
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
  QList<QString> uuid;
};

#endif
