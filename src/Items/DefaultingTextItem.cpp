// DefaultingTextItem.cpp

#include "DefaultingTextItem.h"
#include <QDebug>

DefaultingTextItem::DefaultingTextItem(TextData *data, Item *parent,
				       bool noFinalize, TextItemDoc *altdoc):
  TextItem(data, parent, noFinalize, altdoc) {
  connect(document(), SIGNAL(contentsChanged(int, int, int)),
	  this, SIGNAL(textChanged()));
}

DefaultingTextItem::~DefaultingTextItem() {
}

void DefaultingTextItem::setDefaultText(QString s) {
  dflt_text = s;
  update();
}

void DefaultingTextItem::paint(QPainter *p, const QStyleOptionGraphicsItem *o,
			       QWidget *w) {
  qDebug() << "dfltti" << text->text() << " / " << dflt_text;
  if (text->text().isEmpty()) {
    QPointF xy = text->locate(0);
    MarkupStyles sty;
    sty.add(MarkupData::Italic);
    p->setFont(text->font(sty));
    p->setPen(style().color("default-item-color"));
    p->drawText(xy, dflt_text);
  }
  TextItem::paint(p, o, w);
}
