// PageScene.H

#ifndef PAGESCENE_H

#define PAGESCENE_H

#include <QGraphicsScene>

class PageScene: public QGraphicsScene {
  Q_OBJECT;
  Q_ENUMS(MouseMode);
  Q_PROPERTY(MouseMode mode READ mode WRITE setMode);
public:
  enum MouseMode {
    ViewMode,
    TypeMode,
    MoveMode,
  };
public:
  PageScene(class PageData *data, QObject *parent=0);
  virtual ~PageScene();
  int sheetCount() const;
  void setMode(MouseMode);
  MouseMode mode() const;
public slots:
  void nextSheet();
  void previousSheet();
  void gotoSheet(int i);
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *);
private slots:
  void titleEdited();
  void titleTextEdited();
private:
  void makeBackground();
  void makeTitleItem();
  void makeDateItem();
  void makePgNoItem();
  void makeContdItem();
  void makeBlockItems();
  void stackBlocks();
  int restackBlocks(int starti=0); // returns new sheet no of starti item
  void positionPgNoItem();
  void positionTitleItem();
private:
  QGraphicsTextItem *titleItem;
  QGraphicsTextItem *dateItem;
  QGraphicsTextItem *pgNoItem;
  QGraphicsTextItem *contdItem; // from prev page
  QGraphicsTextItem *contItem; // to next page
  QGraphicsLineItem *leftMarginItem;
  QGraphicsLineItem *topMarginItem;
  QGraphicsRectItem *bgItem;
  QList<class BlockItem *> blockItems;
private:
  QList<int> sheetNos; // sheet number for each block
  QList<double> topY; // top Y position for each block
  int nSheets; // number of sheets
  int iSheet; // current sheet (counting from 0)
  PageData *data;
  class Style const &style;
  MouseMode mode_;
};

#endif
