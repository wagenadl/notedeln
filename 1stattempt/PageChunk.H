// PageChunk.H

#ifndef PAGECHUNK_H

#define PAGECHUNK_H

#include <QObject>
#include <QDomElement>

/* A PageChunk should be a QGraphicsItem that contains its children.
   Notably, a TextChunk will be implemented as a QGraphicsItem that contains
   a QGraphicsTextItem. The reason is twofold: First, Notes must be children
   of the appropriate QGraphicsItem and it doesn't make sense to make them
   be children of the QGraphicsTextItem. Second, and more prosaically, I want
   TextChunk to inherit PageChunk in the C++ sense, and that wouldn't work if
   TextChunk *is* a QGraphicsTextItem rather than *contains* a
   QGraphicsTextItem.
*/

class PageChunk: public QGraphicsItem {
  Q_OBJECT;
public:
  PageChunk(QDomElement elt, double y0=0);
  virtual ~PageChunk();
public:
  static PageChunk *readChunk(QDomElement elt);
public:
  QDateTime created() const;
  QDateTime modified() const; // this does *not* include note modifications
  bool editable() const;
  QList<Note const *> notes() const;
  Note *newNote(double x, double yrel);
public slots:
  void childChange();
signals:
  void changed(); // emitted even for note changes
protected:
  void registerMaker(QString typ, PageChunk *(*foo)(QDomElement));
private:
  QDomElement xml;
  QList<Note *> notes_;
  static QMap<QString, PageChunk *(*)(QDomElement)> makers;
  
};

#endif
