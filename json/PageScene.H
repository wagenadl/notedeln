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
  void hChanged(int block);
  void vChanged(int block);
  void futileMovement(int block);
  void enterPressed(int block);
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
  int findLastBlockOnSheet(int sheet); // returns -1 if none
  bool inMargin(QPointF); // true if point is in any margin
  bool belowContent(QPointF); // true if point is below any blocks on sheet,
  // whether or not the point is in the margin
  void newTextBlock(int after=-1); // creates a new text block after the
  // given block or at the bottom of this sheet
  void deleteBlock(int blocki); // also deletes data!
private:
  class TitleItem *titleItem;
  QGraphicsTextItem *dateItem;
  QGraphicsTextItem *pgNoItem;
  QGraphicsTextItem *nOfNItem;
  QGraphicsTextItem *contdItem; // from prev page
  QGraphicsTextItem *contItem; // to next page
  QGraphicsLineItem *leftMarginItem;
  QGraphicsLineItem *topMarginItem;
  QGraphicsRectItem *bgItem;
  class QSignalMapper *hChangeMapper;
  class QSignalMapper *vChangeMapper;
  class QSignalMapper *abandonedMapper;
  class QSignalMapper *futileMovementMapper;
  class QSignalMapper *enterPressedMapper;
private:
  QList<class BlockItem *> blockItems;
  QList<int> sheetNos; // sheet number for each block
  QList<double> topY; // top Y position for each block
private:
  int nSheets; // number of sheets
  int iSheet; // current sheet (counting from 0)
  PageData *data;
  class Style const &style;
  MouseMode mode_;
};

#endif
