// Items/TextItem.H - This file is part of eln

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

// TextItem.H

#ifndef TEXTITEM_H

#define TEXTITEM_H

#include "MarkupData.h"
#include "Item.h"
#include "Mode.h"
#include "TextData.h"
#include "TextCursor.h"
#include <QList>
#include "MarkupEdges.h"

class TextItem: public Item {
  Q_OBJECT;
public:
  TextItem(TextData *data, Item *parent, bool noFinalize=false,
	   class TextItemDoc *altdoc=0);
  // only descendents should set noFinalize=true!
  virtual ~TextItem();
  DATAACCESS(TextData);
  virtual void makeWritable();
  virtual void makeWritableNoRecurse();
  bool allowParagraphs() const;
  void setAllowParagraphs(bool yes);
  void setAllowMoves();
  void addMarkup(MarkupData::Style t, int start, int end);
  void addMarkup(MarkupData *); // we appropriate the data!
  void deleteMarkup(MarkupData *);
  bool allowNotes() const;
  void setAllowNotes(bool y=true);
  TextCursor insertBasicHtml(QString html, int pos);
  // Only <b>, <i>, <sub>, and <sup> tags are supported at present.
  // Returns a text cursor that spans the inserted text.
  int pointToPos(QPointF, bool strict=false) const;
  QPointF posToPoint(int) const;
  //  QRectF fittedRect() const;
  QRectF netBounds() const;
  QRectF clipRect() const; // null if not clipping
  bool clips() const;
  void setClip(QRectF);
  void unclip();
signals:
  void invisibleFocus(QPointF);
  void textChanged();
  //  void widthChanged();
  void abandoned(); // escape pressed while empty
  void futileMovementKey(int, Qt::KeyboardModifiers);
  /* also emitted for enter press or for delete/backspace at end/start */
  void mousePress(QPointF, Qt::MouseButton);
  void refTextChange(QString oldText, QString newText);
  void multicellular(int pos, TextData *td);
public:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
  virtual void keyPressEvent(QKeyEvent *);
  virtual void focusInEvent(QFocusEvent *);
  virtual void focusOutEvent(QFocusEvent *);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
private:
  bool keyPressAsMath(QKeyEvent *);
  bool keyPressAsMotion(QKeyEvent *);
  bool keyPressAsSpecialChar(QKeyEvent *);
  bool keyPressWithControl(QKeyEvent *);
  bool keyPressAsSpecialEvent(QKeyEvent *);
  bool keyPressAsInsertion(QKeyEvent *);
  void handleLeftClick(QGraphicsSceneMouseEvent *);
  void selectWordOrLineOrParagraph(int pos);
protected slots:
  virtual void markupChange(MarkupData *);
  virtual void docChange();
  virtual void modeChange(Mode::M);
public:
  QString markedText(MarkupData *);
  bool tryExplicitLink();
  QString toHtml(int start=0, int end=-1) const;
private:
  void initializeFormat();
  int refineStart(int start, int base);
  int refineEnd(int end, int base);
protected:
  bool tryFootnote();
  virtual bool tryToPaste(bool noparagraphs=false);
  virtual bool tryToCopy() const;
  bool tryScriptStyles(bool onlyIfBalanced=false);
  bool tryTeXCode(bool noX=false, bool onlyAtEndOfWord=false);
  void toggleSimpleStyle(MarkupData::Style type, class TextCursor const &c);
  bool charBeforeIsLetter(int pos) const;
  bool charAfterIsLetter(int pos) const;
  bool shouldResize(QPointF) const;
  void attemptMarkup(QPointF p, MarkupData::Style);
  void updateMarkup(int pos);
  bool muckWithIndentation(class TextBlockItem *p,
			   Qt::KeyboardModifiers mod);
public: // pass-through
  void setFont(QFont f);
  QColor defaultTextColor() const { return text->color(); }
  void setDefaultTextColor(QColor c) { text->setColor(c); }
  TextItemDoc *document() const { return text; }
  void setTextWidth(double d, bool relayout=true);
  double textWidth() const { return text->width(); }
  void setLineHeight(double h);
  double lineHeight() const { return text->lineHeight(); }
  TextCursor textCursor() const { return cursor; }
  void setTextCursor(TextCursor const &c);
public:
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
protected:
  void finalizeConstructor(int sheetOnly=-1);
  bool keyPressAsSimpleStyle(int key, TextCursor const &cursor);
  void tryMove(TextCursor::MoveOperation op, int key,
               Qt::KeyboardModifiers mod);
  virtual void representCursor(QList<TransientMarkup> &) const;
  virtual void representSearchPhrase(QList<TransientMarkup> &) const;
protected:
  bool mayMark;
  bool mayNote;
  bool allowParagraphs_;
  class TextItemDoc *text;
  bool mayMove;
  QPointF cursorPos;
  MarkupData::Style lateMarkType;
  int lateMarkStart;
  QRectF clip_;
  bool hasAltDoc; // i.e., we don't own the doc
  TextCursor cursor;
  class LinkHelper *linkHelper;
  QMap<MarkupData *, QString> reftexts;
  QTime lastClickTime;
  QPoint lastClickScreenPos;
};

#endif
