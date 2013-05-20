// MessageObject.C

#include "MessageObject.H"
#include "Style.H"

MessageObject::MessageObject(QString msg, Style const &):
  QGraphicsTextItem() {
  setHtml(msg);
  // stylize right here
}

MessageObject::~MessageObject() {
}
