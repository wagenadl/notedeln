// DefaultingQTI.h

#ifndef DEFAULTINGQTI_H

#define DEFAULTINGQTI_H

#include <QGraphicsTextItem>

class DefaultingQTI: public QGraphicsTextItem {
  Q_OBJECT;
public:
  DefaultingQTI(QString dflttxt, QGraphicsItem *parent=0);
  virtual ~DefaultingQTI();
  QRectF inclusiveSceneBoundingRect() const;
  void setFont(QFont);
  void paint(QPainter *painter,
	     const QStyleOptionGraphicsItem *option,
	     QWidget *widget);
public slots:
  void setDefaultText(QString);
protected:
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
private slots:
  void showOrHide();
private:
  void reposition();
private:
  class MyQTI *dtext;
  QString dt;
};

#endif
