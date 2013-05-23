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

#include "BaseScene.H"
#include <QTextCursor>

class EntryScene: public BaseScene {
  Q_OBJECT;
public:
  EntryScene(class EntryData *data, QObject *parent=0);
  virtual void populate();
  virtual ~EntryScene();
  class EntryData *data() const;
  void makeWritable();
  void focusEnd(); // may create new text block
  bool isWritable() const;
  void gotoSheet(int i);
  void gotoSheetOfBlock(int blocki);
  int findBlock(class Item const *) const; // -1 if none
  int findBlock(QPointF scenePos) const; // -1 if none
  void newFootnote(int block, QString tag);
  bool dropBelow(QPointF scenePos, class QMimeData const *md);
public slots:
  void notifyChildless(class BlockItem *);
signals:
  void nowOnPage(int);
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void keyPressEvent(QKeyEvent *);
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
  class TableBlockItem *injectTableBlock(class TableBlockData *, int iblock);
  class TextBlockItem *injectTextBlock(class TextBlockData *, int iblock);
  bool tryToPaste();
  bool tryMakeNote();
  void remap();
  bool importDroppedImage(QPointF scenePos, QImage const &img,
			  class QUrl const &source);
  bool importDroppedUrls(QPointF scenePos, QList<class QUrl> const &url);
  bool importDroppedUrl(QPointF scenePos, class QUrl const &url);
  bool importDroppedText(QPointF scenePos, QString const &txt);
  bool importDroppedFile(QPointF scenePos, QString const &fn);
  bool importDroppedSvg(QPointF scenePos, class QUrl const &url);
  void makeBackground();
  void makeTitleItem();
  void makeDateItem();
  void makeBlockItems();
  void positionBlocks();
  void restackBlocks();
  void restackNotes(int sheet);
  void positionTitleItem();
  void positionNofNAndDateItems();
  int findLastBlockOnSheet(int sheet); // returns -1 if none
  bool belowContent(QPointF); // true if point is below any blocks on sheet,
  // whether or not the point is in the margin
  class GfxBlockItem *newGfxBlock(int after=-1);
  class TextBlockItem *newTextBlock(int after=-1, bool evenIfLastEmpty=false);
  // creates a new text block after the
  // given block or at the bottom of this sheet
  class TableBlockItem *newTableBlock(int after=-1);
  class GfxBlockItem *gfxBlockAfter(int iblock);
  void deleteBlock(int blocki); // also deletes data!
  class BlockItem *tryMakeGfxBlock(class BlockData *bd);
  class BlockItem *tryMakeTextBlock(class BlockData *bd);
  class BlockItem *tryMakeTableBlock(class BlockData *bd);
  void joinTextBlocks(int iblock_pre, int iblock_post);
  int indexOfBlock(class BlockItem *) const; // -1 if none
  void repositionContItem();
  void reshapeBelowItem();
private:
  QGraphicsTextItem *dateItem;
  class BelowItem *belowItem;
  class QSignalMapper *vChangeMapper;
  class QSignalMapper *noteVChangeMapper;
  class QSignalMapper *futileMovementMapper;
private:
  QList<class BlockItem *> blockItems;
  QList<class FootnoteGroupItem *> footnoteGroups; // one for each block
private:
  EntryData *data_;
  bool writable;
  class TitleItem *titleItemX;
};

#endif
