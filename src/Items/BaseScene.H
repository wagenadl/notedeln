// BaseScene.H - Base for TOCScene and PageScene

#ifndef BASESCENE_H

#define BASESCENE_H

#include <QGraphicsScene>

class BaseScene: public QGraphicsScene  {
  Q_OBJECT;
public:
  BaseScene(class Data *data, QObject *parent=0);
  virtual void populate();
  virtual ~BaseScene();
  int sheetCount() const;
  class Style const &style() const;
  virtual bool print(class QPrinter *, class QPainter *,
		     int firstSheet=0, int lastSheet=100000000);
  int currentSheet() const;
  class Mode const *mode() const;
public slots:
  bool nextSheet();
  bool previousSheet();
  virtual void gotoSheet(int i);
  void focusTitle();
protected:
  bool inMargin(QPointF); // true if point is in any margin
  virtual void makeBackground();
  virtual void makeTitleItem();
  virtual void makePgNoItem();
  virtual void makeContdItems();
  virtual void positionPgNoItem();
  virtual void positionTitleItem();
  virtual int startPage() const;
  virtual QString title() const;
  virtual QString pgNoToString(int) const;
protected:
  QGraphicsItem *titleItem;
  QGraphicsTextItem *pgNoItem;
  QGraphicsTextItem *contdItem; // from prev page
  QGraphicsTextItem *contItem; // to next page
  QGraphicsLineItem *leftMarginItem;
  QGraphicsLineItem *topMarginItem;
  QGraphicsRectItem *bgItem;
protected:
  int nSheets; // number of sheets
  int iSheet; // current sheet (counting from 0)
  Data *data;
  Style const *style_;
};

#endif
