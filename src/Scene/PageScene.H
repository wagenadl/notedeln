// PageScene.H

#ifndef PAGESCENE_H

#define PAGESCENE_H

#include "BaseScene.H"

class PageScene: public BaseScene {
  Q_OBJECT;
public:
  PageScene(class PageData *data, QObject *parent=0);
  virtual void populate();
  virtual ~PageScene();
  void makeWritable();
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void keyPressEvent(QKeyEvent *);
  void dropEvent (QGraphicsSceneDragDropEvent *);
  virtual int startPage() const;
private slots:
  void titleEdited();
  void hChanged(int block);
  void vChanged(int block);
  void futileMovement(int block);
  void enterPressed(int block);
private:
  bool tryToPaste();
  bool importDroppedImage(QPointF scenePos, QImage const &img,
			  class QUrl const *source=0);
  bool importDroppedUrls(QPointF scenePos, QList<class QUrl> const &url);
  bool importDroppedUrl(QPointF scenePos, class QUrl const &url);
  bool importDroppedText(QPointF scenePos, QString const &txt,
			 class QUrl const *source=0);
  bool importDroppedFile(QPointF scenePos, QString const &fn);
  void makeBackground();
  void makeTitleItem();
  void makeDateItem();
  void makeBlockItems();
  void stackBlocks();
  void gotoSheet(int i);
  int restackBlocks(int starti=0); // returns new sheet no of starti item
  void positionTitleItem();
  int findLastBlockOnSheet(int sheet); // returns -1 if none
  bool belowContent(QPointF); // true if point is below any blocks on sheet,
  // whether or not the point is in the margin
  class GfxBlockItem *newGfxBlock();
  class TextBlockItem *newTextBlock(int after=-1, bool evenIfLastEmpty=false);
  // creates a new text block after the
  // given block or at the bottom of this sheet
  void deleteBlock(int blocki); // also deletes data!
  class BlockItem *tryMakeGfxBlock(class BlockData *bd);
  class BlockItem *tryMakeTextBlock(class BlockData *bd);
private:
  QGraphicsTextItem *dateItem;
  QGraphicsTextItem *nOfNItem;
  QGraphicsRectItem *belowItem;
  class QSignalMapper *hChangeMapper;
  class QSignalMapper *vChangeMapper;
  class QSignalMapper *abandonedMapper;
  class QSignalMapper *futileMovementMapper;
  class QSignalMapper *enterPressedMapper;
  class QNetworkAccessManager *networkManager;
private:
  QList<class BlockItem *> blockItems;
  QList<int> sheetNos; // sheet number for each block
  QList<double> topY; // top Y position for each block
private:
  PageData *data;
  bool writable;
};

#endif
