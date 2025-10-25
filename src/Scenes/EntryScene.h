// Scenes/EntryScene.H - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// EntryScene.H

#ifndef PAGESCENE_H

#define PAGESCENE_H

#include "BaseScene.h"
#include <QTextCursor>
#include <QMap>
#include "CachedEntry.h"
#include "TableData.h"

class EntryScene: public BaseScene {
  Q_OBJECT;
public:
  EntryScene(CachedEntry data, QObject *parent=0);
  virtual void populate() override;
  virtual ~EntryScene();
  class EntryData *data() const;
  void makeWritable();
  void focusEnd(int isheet=-1); // may create new text block
  virtual bool isWritable() const override;
  int findBlock(class Item const *) const; // -1 if none
  int findBlock(class Data const *) const; // -1 if none
  class BlockItem const *findBlockByUUID(QString uuid) const; // -1 if none
  int findBlock(QPointF scenePos, int sheet) const; // -1 if none
  bool focusFootnote(int block, QString tag); // true if found
  void newFootnote(int block, QString tag);
  virtual int startPage() const override;
  class LateNoteItem *createLateNote(QPointF scenePos,
				     int sheet); // create note by dragging
  class LateNoteItem *newLateNote(int sheet,
				  QPointF scenePos1,
				  QPointF scenePos2=QPointF());
  virtual QString title() const override;
  virtual class TitleData *fancyTitle() const override;
  virtual QDate date() const override;
  virtual QString pgNoToString(int) const override;
  void clipPgNoAt(int);
  void unlock();
  QList<class BlockItem const *> blocks() const;
  QList<class FootnoteItem const *> footnotes() const;
public slots:
  void notifyChildless(BlockItem *);
  void redateBlocks();
  void restackBlocks(int start=0);
signals:
  void restacked();
  void sheetRequest(int);
protected:
  bool mousePressEvent(QGraphicsSceneMouseEvent *, SheetScene *) override;
  bool keyPressEvent(QKeyEvent *, SheetScene *) override;
  bool dropEvent(QGraphicsSceneDragDropEvent *, SheetScene *) override;
protected:
  void setSheetCount(int) override;
  void waitForLoadComplete() override;
protected slots:
  void titleEdited() override;
  void vChanged(BlockItem *block);
  void futileMovement(BlockItem *block);
  void focusFirst(int sheet) override;
  void makeUnicellular(TableData *);
  void makeMulticellular(int pos, TextData *);
  void makeMulticellularAndPaste(TextData *, QString);
private:
  class TableBlockItem *doMakeMulticellular(int pos, TextData *);
  void resetCreation();
  void splitTextBlock(int iblock, int pos);
  class TableBlockItem *injectTableBlock(class TableBlockData *, int iblock);
  class TextBlockItem *injectTextBlock(class TextBlockData *, int iblock);
  bool tryToPaste(SheetScene *s);
  bool importDroppedImage(QPointF scenePos, int sheet,
			  QImage const &img, class QUrl const &source);
  bool importDroppedVideo(QPointF scenePos, int sheet,
			  QImage const &img, double dur,
                          class QUrl const &source);
  bool importDroppedUrls(QPointF scenePos, int sheet,
			 QList<class QUrl> const &url,
                         class TextItem *fi=0);
  bool importDroppedUrl(QPointF scenePos, int sheet, class QUrl const &url,
                        class TextItem *fi=0);
  bool importDroppedText(QPointF scenePos, int sheet, QString const &txt,
                         class TextItem **itemReturn=0,
                         int *startReturn=0, int *endReturn=0,
                         class TextItem *fi=0);
  // In the above, fi!=0 indicates Ctrl-V on an existing text item.
  bool importDroppedFile(QPointF scenePos, int sheet, QString const &fn);
  bool importDroppedSvg(QPointF scenePos, int sheet, class QUrl const &url);
  void makeBackground();
  void makeTitleItem();
  void makeDateItem();
  void makeBlockItems();
  void positionBlocks();
  void loadLateNotes();
  void resetSheetCount();
  void positionTitleItem();
  void positionNofNAndDateItems();
  int findLastBlockOnSheet(int sheet); // returns -1 if none
  class TextBlockItem *newTextBlock(int after, bool evenIfLastEmpty=false);
  class GfxBlockItem *newGfxBlock(int after);
  class TableBlockItem *newTableBlock(int after);
  // creates a new text/gfx/table block after the given block
  class TextBlockItem *newTextBlockAt(QPointF pos, int sheet,
				      bool evenIfLastEmpty=false);
  class GfxBlockItem *newGfxBlockAt(QPointF pos, int sheet);
  class TableBlockItem *newTableBlockAt(QPointF pos, int sheet);
  // create a new text/gfx/table block at the given position
  void deleteBlock(int blocki); // also deletes data!
  class BlockItem *tryMakeGfxBlock(class BlockData *bd);
  class BlockItem *tryMakeTextBlock(class BlockData *bd);
  class BlockItem *tryMakeTableBlock(class BlockData *bd);
  void joinTextBlocks(int iblock_pre, int iblock_post, bool forward=false);
  int indexOfBlock(class BlockItem *) const; // -1 if none
  void repositionContItem();
  void reshapeBelowItem();
  int clippedPgNo(int n) const;
  void addUnlockedWarning();
  int lastBlockAbove(QPointF scenepos, int sheet);
  // find the last block with bottom y above scene pos, or -1 if none
  int indexOfBlock(class BlockItem const *) const; // -1 if none
public slots:
  void gotoSheetOfBlock(int);
private:
  QList<QGraphicsTextItem *> dateItems;
private:
  QList<class BlockItem *> blockItems;
  QMap<class BlockItem *, class QGraphicsTextItem *> blockDateItems;
private:
  CachedEntry data_;
  bool writable;
  int firstDisallowedPgNo;
  class QGraphicsTextItem *unlockedItem;
  Item *lateNoteParent;
  QString originatingUUID;
  int originatingPos;
};

#endif
