// OneLink.h

#ifndef ONELINK_H

#define ONELINK_H

#include "MarkupData.h"
#include "TextItem.h"
#include <QPointer>

class OneLink: public QObject {
public:
  OneLink(MarkupData *md, TextItem *item);
  bool mousePress(QGraphicsSceneMouseEvent *);
  bool mouseDoubleClick(QGraphicsSceneMouseEvent *);
  void enter(QGraphicsSceneMouseEvent *);
  void leave();
  bool isValid() const  { return !md.isNull(); }
private slots:
  void downloadFinished();
protected:
  QString refText() const;
  void openLink();
  void openArchive();
  void getArchiveAndPreview();
  bool hasArchive() const;
  bool hasPreview() const;
  class Resource *resource() const;
  void activate(QGraphicsSceneMouseEvent *);
  void openPage(bool newView=false);
private:
  QPointer<MarkupData> md;
  QPointer<TextItem> ti;
  class PreviewPopper *popper;
  QString lastRef;
  bool lastRefIsNew;
  bool busy;  
};

#endif
