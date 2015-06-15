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
		     class TextItemDoc *doc=0);
  class TitleItem *fancyTitleItem();
  void setPageNumber(QString n);
  void setContInMargin(bool x=true);
  Style const &style() const { return style_; }
  void setOverlay(QGraphicsObject *);
  void repositionTitle();
  class TextItemDoc *fancyTitleDocument();
  QGraphicsView *eventView() const;
  // This is the view from which the current mouse, key, enter, etc event came
  class PageView *pageView() const;
  // As above, but dynamic_casted to PageView. Can be null!
  void setEventView(QGraphicsView *);
  class Mode *mode() const;
  QGraphicsRectItem *backgroundItem() { return bgItem; }
  QGraphicsRectItem *marginItem() { return margItem; }
signals:
  void leaveTitle();
public:
  void setCursors();
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *);
  virtual void dropEvent(QGraphicsSceneDragDropEvent *);
  virtual void drawBackground(QPainter *, const QRectF &);
private slots:
  void futileTitleMovement(int, Qt::KeyboardModifiers);
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
  class QGraphicsRectItem *bgItem, *margItem;
  bool contInMargin;
  QPointer<QGraphicsObject> overlay;
  QPointer<QGraphicsView> eventView_;
};

#endif
