// Scenes/EntryScene.H - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// EntryScene.H

#ifndef PAGESCENE_H

#define PAGESCENE_H

#include "BaseScene.h"
#include <QTextCursor>
#include <QMap>
#include "CachedEntry.h"
#include "Mode.h"

class EntryScene: public BaseScene {
  Q_OBJECT;
public:
  EntryScene(CachedEntry data, QObject *parent=0);
  virtual void populate();
  virtual ~EntryScene();
  class EntryData *data() const;
  void makeWritable();
  void focusEnd(int isheet=-1); // may create new text block
  bool isWritable() const;
  int findBlock(class Item const *) const; // -1 if none
  int findBlock(class Data const *) const; // -1 if none
  class BlockItem const *findBlockByUUID(QString uuid) const; // -1 if none
  int findBlock(QPointF scenePos, int sheet) const; // -1 if none
  void newFootnote(int block, QString tag);
  bool dropBelow(QPointF scenePos, int sheet, class QMimeData const *md);
  virtual int startPage() const;
  class LateNoteItem *createLateNote(QPointF scenePos,
				     int sheet); // create note by dragging
  class LateNoteItem *newLateNote(int sheet,
				  QPointF scenePos1,
				  QPointF scenePos2=QPointF());
  virtual QString title() const;
  virtual class TitleData *fancyTitle() const;
  virtual QDate date() const;
  virtual QString pgNoToString(int) const;
  void clipPgNoAt(int);
  void unlock();
  QList<class BlockItem const *> blocks() const;
  QList<class FootnoteItem const *> footnotes() const;
public slots:
  void notifyChildless(class BlockItem *);
  void redateBlocks();
signals:
  void restacked();
  void sheetRequest(int);
protected:
  bool mousePressEvent(QGraphicsSceneMouseEvent *, SheetScene *);
  bool keyPressEvent(QKeyEvent *, SheetScene *);
  bool dropEvent(QGraphicsSceneDragDropEvent *, SheetScene *);
protected:
  void setSheetCount(int);
protected slots:
  void titleEdited();
  void vChanged(int block);
  void futileMovement(int block);
  void focusFirst(int sheet);
  void makeUnicellular(class TableData *);
  void makeMulticellular(int pos, class TextData *);
private:
  void resetCreation();
  void splitTextBlock(int iblock, int pos);
  class TableBlockItem *injectTableBlock(class TableBlockData *, int iblock);
  class TextBlockItem *injectTextBlock(class TextBlockData *, int iblock);
  bool tryToPaste(SheetScene *s);
  void remap();
  bool importDroppedImage(QPointF scenePos, int sheet,
			  QImage const &img, class QUrl const &source);
  bool importDroppedUrls(QPointF scenePos, int sheet,
			 QList<class QUrl> const &url);
  bool importDroppedUrl(QPointF scenePos, int sheet, class QUrl const &url);
  bool importDroppedText(QPointF scenePos, int sheet, QString const &txt,
                         class TextItem **itemReturn=0,
                         int *startReturn=0, int *endReturn=0);
  bool importDroppedFile(QPointF scenePos, int sheet, QString const &fn);
  bool importDroppedSvg(QPointF scenePos, int sheet, class QUrl const &url);
  void makeBackground();
  void makeTitleItem();
  void makeDateItem();
  void makeBlockItems();
  void positionBlocks();
  void restackBlocks(int start);
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
  void joinTextBlocks(int iblock_pre, int iblock_post);
  int indexOfBlock(class BlockItem *) const; // -1 if none
  void repositionContItem();
  void reshapeBelowItem();
  int clippedPgNo(int n) const;
  void addUnlockedWarning();
  int lastBlockAbove(QPointF scenepos, int sheet);
  // find the last block with bottom y above scene pos, or -1 if none
public slots:
  void gotoSheetOfBlock(int);
private:
  QList<QGraphicsTextItem *> dateItems;
  class QSignalMapper *vChangeMapper;
  class QSignalMapper *futileMovementMapper;
private:
  QList<class BlockItem *> blockItems;
  QMap<class BlockItem *, class QGraphicsTextItem *> blockDateItems;
private:
  CachedEntry data_;
  bool writable;
  int firstDisallowedPgNo;
  class QGraphicsTextItem *unlockedItem;
};

#endif
