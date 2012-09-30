// TextChunk.H

#ifndef TEXTCHUNK_H

#define TEXTCHUNK_H

#include "PageChunk.H"

class TextChunk: public PageChunk {
  Q_OBJECT;
public:
  TextChunk(QDomElement elt, double y0=0);
  virtual ~TextChunk();
  QString text() const;
private:
  QGraphicsTextItem *item;
};

#endif
