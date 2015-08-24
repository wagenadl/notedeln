// DefaultingQTI.h

#ifndef DEFAULTINGQTI_H

#define DEFAULTINGQTI_H

#include <QGraphicsTextItem>

class DefaultingQTI: public QGraphicsTextItem {
  Q_OBJECT;
public:
  DefaultingQTI(QString dflttxt, QGraphicsItem *parent=0);
  virtual ~DefaultingQTI();
  virtual QRectF boundingRect() const;
  void setFont(QFont);
  void setDefaultText(QString);
  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget);
protected:
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
private:
  QString dt;
};

#endif
