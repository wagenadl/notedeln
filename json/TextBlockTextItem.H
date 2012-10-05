// TextBlockTextItem.H

#ifndef TEXTBLOCKTEXTITEM_H

#define TEXTBLOCKTEXTITEM_H

#include <QGraphicsTextItem>

class TextBlockTextItem: public QGraphicsTextItem {
  Q_OBJECT;
public:
  TextBlockTextItem(class TextBlockData *data, class TextBlockItem *parent);
  ~TextBlockTextItem();
signals:
  void textChanged();
private slots:
  void docChange();
private:
  TextBlockData *data_;
};

#endif
