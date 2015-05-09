// LinkHelper.h

#ifndef LINKHELPER_H

#define LINKHELPER_H

#include <QObject>
#include <QMap>
#include <QPointF>

class LinkHelper: public QObject {
public:
  LinkHelper(class TextItem *parent);
  virtual ~LinkHelper();
  bool mousePress(class QGraphicsSceneMouseEvent *);
  bool mouseDoubleClick(QGraphicsSceneMouseEvent *);
  void mouseMove(class QGraphicsSceneHoverEvent *);
public:
  void updateMarkup(class MarkupData *);
  void newMarkup(MarkupData *);
  void removeMarkup(MarkupData *);
  void updateAll();
private:
  MarkupData *findMarkup(QPointF) const;
  void mouseCore(QPointF);
  void perhapsLeave(MarkupData *);
private:
  TextItem *item;
  QMap<class MarkupData *, class OneLink *> links;
  class OneLink *current;
};

#endif
