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
  Qt::KeyboardModifiers keyboardModifiers() const;
public slots:
  bool nextSheet();
  bool previousSheet();
  void gotoSheet(int i);
signals:
  void modifiersChanged(Qt::KeyboardModifiers);
  void futileMovement(QKeyEvent *);
protected:
  virtual void keyPressEvent(QKeyEvent *);
  virtual void keyReleaseEvent(QKeyEvent *);
protected:
  void keyChange(int key, int delta);
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
  QGraphicsTextItem *titleItem;
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
  class Style const *style;
  Qt::KeyboardModifiers keymods;
};

#endif
