// PageScene.H

#ifndef PAGESCENE_H

#define PAGESCENE_H

#include "BaseScene.H"
#include <QTextCursor>

class PageScene: public BaseScene {
  Q_OBJECT;
public:
  PageScene(class PageData *data, QObject *parent=0);
  virtual void populate();
  virtual ~PageScene();
  void makeWritable();
  void focusEnd(); // may create new text block
  bool isWritable() const;
  void gotoSheet(int i);
signals:
  void nowOnPage(int);
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
  void futileTitleMovement(QTextCursor, int, Qt::KeyboardModifiers);
private:
  void splitTextBlock(int iblock, int pos);
  class TextBlockItem *injectTextBlock(class TextBlockData *, int iblock);
  bool tryToPaste();
  void remap();
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
  void joinTextBlocks(int iblock_pre, int iblock_post);
private:
  QGraphicsTextItem *dateItem;
  QGraphicsTextItem *nOfNItem;
  QGraphicsRectItem *belowItem;
  class QSignalMapper *hChangeMapper;
  class QSignalMapper *vChangeMapper;
  class QSignalMapper *futileMovementMapper;
  class QNetworkAccessManager *networkManager;
private:
  QList<class BlockItem *> blockItems;
  QList<int> sheetNos; // sheet number for each block
  QList<double> topY; // top Y position for each block
  QList<QGraphicsItem *> footnoteGroups; // one for each block
private:
  PageData *data;
  bool writable;
};

#endif
