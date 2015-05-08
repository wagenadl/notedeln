// OneLink.h

#ifndef ONELINK_H

#define ONELINK_H

#include <QObject>

class OneLink: public QObject {
public:
  OneLink(MarkupData *md, TextItem *item);
  bool mousePress(QGraphicsSceneMouseEvent *);
  bool mouseDoubleClick(QGraphicsSceneMouseEvent *);
  void enter(QGraphicsSceneMouseEvent *);
  void leave();
  void update();
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
  MarkupData *md; // we do not own
  TextItem *ti; // we do not own
  class PreviewPopper *popper; // we own
  QString lastRef;
  bool lastRefIsNew;
  bool busy;  
};

#endif
