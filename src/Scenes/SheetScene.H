// SheetScene.H

#ifndef SHEETSCENE_H

#define SHEETSCENE_H

#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QDate>
#include <QPointer>

class SheetScene: public QGraphicsScene {
  Q_OBJECT;
public:
  SheetScene(class Style const &s, class BaseScene *parent);
  virtual ~SheetScene();
  void setDate(QDate const &date);
  void setNOfN(int n, int N, bool alwaysShow=false);
  /* n should count 0..N-1 but is printed as 1..N */
  void setTitle(QString const &title);
  void setFancyTitle(class TitleData *data, int sheet,
		     class QTextDocument *doc=0);
  class TitleItem *fancyTitleItem();
  void setPageNumber(QString n);
  void setContInMargin(bool x=true);
  Style const &style() const { return style_; }
  void setOverlay(QGraphicsObject *);
  void repositionTitle();
  QTextDocument *fancyTitleDocument();
  QGraphicsView *eventView() const;
  /* This is the view from which the current mouse or key event comes */
  void setEventView(QGraphicsView *);
 protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void drawBackground(QPainter *, const QRectF &);
private:
  void makeBackground();
  void repositionDate();
private:
  class BaseScene *base;
  class Style const &style_;
  class QGraphicsTextItem *pgnoItem;
  class QGraphicsItem *titleItem;
  class QGraphicsTextItem *plainTitleItem;
  class TitleItem *fancyTitleItem_;
  class QGraphicsTextItem *contItem;
  class QGraphicsTextItem *contdItem;
  class QGraphicsTextItem *nOfNItem;
  class QGraphicsTextItem *dateItem;
  class QGraphicsLineItem *leftMarginItem;
  class QGraphicsLineItem *topMarginItem;
  //  class QGraphicsRectItem *bgItem;
  bool contInMargin;
  QPointer<QGraphicsObject> overlay;
  QPointer<QGraphicsView> eventView_;
};

#endif
