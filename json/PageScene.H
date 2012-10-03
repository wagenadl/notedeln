// PageScene.H

#ifndef PAGESCENE_H

#define PAGESCENE_H

#include <QGraphicsScene>

class PageScene: public QGraphicsScene {
  Q_OBJECT;
public:
  PageScene(class PageData *data, QObject *parent=0);
  virtual ~PageScene();
  int sheetCount() const;
public slots:
  void nextSheet();
  void previousSheet();
  void gotoSheet(int i);
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
  void positionPgNoItem();
  void positionTitleItem();
private:
  QGraphicsTextItem *titleItem;
  QGraphicsTextItem *dateItem;
  QGraphicsTextItem *pgNoItem;
  QGraphicsTextItem *contdItem;
  QGraphicsLineItem *leftMarginItem;
  QGraphicsLineItem *topMarginItem;
  QList<class BlockItem *> blockItems;
private:
  QList<int> sheetNos; // sheet number for each block
  int nSheets; // number of sheets
  int iSheet; // current sheet (counting from 0)
  PageData *data;
  class Style const &style;
};

#endif
