// DefaultingTextItem.h

#ifndef DEFAULTINGTEXTITEM_H

#define DEFAULTINGTEXTITEM_H

#include "TextItem.h"
#include <QPainter>
#include "TextItemDoc.h"

class DefaultingTextItem: public TextItem {
public:
  DefaultingTextItem(TextData *data, Item *parent=0,
		     bool noFinalize=false,
		     class TextItemDoc *altdoc=0);
  virtual ~DefaultingTextItem();
  void setDefaultText(QString);
protected:
  virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
signals:
  void textChanged();
private:
  QString dflt_text;
};

#endif
