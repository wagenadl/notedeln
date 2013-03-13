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
  class PageData *data() const;
  void makeWritable();
  void focusEnd(); // may create new text block
  bool isWritable() const;
  void gotoSheet(int i);
  int findBlock(class Item const *) const; // -1 if none
  int findBlock(QPointF scenePos) const; // -1 if none
  void newFootnote(int block, QString tag);
public slots:
  void notifyChildless(class BlockItem *);
signals:
  void nowOnPage(int);
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void keyPressEvent(QKeyEvent *);
  void dropEvent (QGraphicsSceneDragDropEvent *);
  virtual int startPage() const;
private slots:
  void titleEdited();
  void vChanged(int block);
  void noteVChanged(int block);
  void futileMovement(int block);
  void futileTitleMovement(int, Qt::KeyboardModifiers);
  void futileNoteMovement();
private:
  void splitTextBlock(int iblock, int pos);
  class TextBlockItem *injectTextBlock(class TextBlockData *, int iblock);
  bool tryToPaste();
  bool tryMakeNote();
  void remap();
  bool importDroppedOrPasted(QPointF scenePos,
			     class QMimeData const *md,
			     bool dropped);
  bool importDroppedImage(QPointF scenePos, QImage const &img,
			  class QUrl const &source);
  bool importDroppedUrls(QPointF scenePos, QList<class QUrl> const &url,
			 bool dropped);
  bool importDroppedUrl(QPointF scenePos, class QUrl const &url,
			bool dropped);
  bool importDroppedText(QPointF scenePos, QString const &txt);
  bool importDroppedFile(QPointF scenePos, QString const &fn);
  bool importDroppedSvg(QPointF scenePos, class QUrl const &url);
  void makeBackground();
  void makeTitleItem();
  void makeDateItem();
  void makeBlockItems();
  void stackBlocks();
  void restackBlocks(int starti=0, bool preferData=false);
  void restackFootnotes(int sheet);
  void positionTitleItem();
  void positionNofNAndDateItems();
  int findLastBlockOnSheet(int sheet); // returns -1 if none
  bool belowContent(QPointF); // true if point is below any blocks on sheet,
  // whether or not the point is in the margin
  class GfxBlockItem *newGfxBlock(int after=-1);
  class TextBlockItem *newTextBlock(int after=-1, bool evenIfLastEmpty=false);
  // creates a new text block after the
  // given block or at the bottom of this sheet
  class GfxBlockItem *gfxBlockAfter(int iblock);
  void deleteBlock(int blocki); // also deletes data!
  class BlockItem *tryMakeGfxBlock(class BlockData *bd);
  class BlockItem *tryMakeTextBlock(class BlockData *bd);
  void joinTextBlocks(int iblock_pre, int iblock_post);
  int indexOfBlock(class BlockItem *) const; // -1 if none
  void repositionContItem();
  void reshapeBelowItem();
private:
  QGraphicsTextItem *dateItem;
  QGraphicsRectItem *belowItem;
  class QSignalMapper *vChangeMapper;
  class QSignalMapper *noteVChangeMapper;
  class QSignalMapper *futileMovementMapper;
private:
  QList<class BlockItem *> blockItems;
  QList<int> sheetNos; // sheet number for each block
  QList<double> topY; // top Y position for each block
  QList<class FootnoteGroupItem *> footnoteGroups; // one for each block
private:
  PageData *data_;
  bool writable;
  class TitleItem *titleItemX;
};

#endif
