// MessageObject.H

#ifndef MESSAGEOBJECT_H

#define MESSAGEOBJECT_H

#include <QGraphicsTextItem>

class MessageObject: public QGraphicsTextItem {
public:
  MessageObject(QString msg, class Style const &style);
  virtual ~MessageObject();
};

#endif
