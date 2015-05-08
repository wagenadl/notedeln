// LinkHelper.h

#ifndef LINKHELPER_H

#define LINKHELPER_H

class LinkHelper: public QObject {
public:
  LinkHelper(class TextItem *parent);
  virtual ~LinkHelper();
  bool mousePress(QGraphicsSceneMouseEvent *);
  bool mouseDoubleClick(QGraphicsSceneMouseEvent *);
  void mouseMove(QGraphicsSceneMouseEvent *);
private:
  MarkupData *findMarkup(QGraphicsSceneMouseEvent *) const;
  void mouseCore(QGraphicsSceneMouseEvent *);
  void perhapsLeave(MarkupData *);
private:
  TextItem *item;
  QMap<class MarkupData *, class OneLink *> links;
  class OneLink *current;
};

#endif
