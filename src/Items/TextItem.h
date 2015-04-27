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
  void insertBasicHtml(QString html, int pos);
  // Only <b> and <i> tags are supported at present
  int pointToPos(QPointF) const;
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
  void widthChanged();
  void abandoned(); // escape pressed while empty
  void futileMovementKey(int, Qt::KeyboardModifiers);
  /* also emitted for enter press or for delete/backspace at end/start */
  void mousePress(QPointF, Qt::MouseButton);
  void refTextChange(QString oldText, QString newText);
  void multicellular(int pos, TextData *td);
protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
  void mousePressEvent(QGraphicsSceneMouseEvent *);
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
  void keyPressEvent(QKeyEvent *);
  void focusInEvent(QFocusEvent *);
  void focusOutEvent(QFocusEvent *);
  void hoverMoveEvent(QGraphicsSceneHoverEvent *);
private:
  bool keyPressAsMath(QKeyEvent *);
  bool keyPressAsMotion(QKeyEvent *);
  bool keyPressAsSpecialChar(QKeyEvent *);
  bool keyPressWithControl(QKeyEvent *);
  bool keyPressAsSpecialEvent(QKeyEvent *);
protected slots:
  virtual void docChange();
  virtual void modeChange(Mode::M);
public:
  void updateRefText(QString oldText, QString newText);
  // for use by TextMarkings to signal change of reference text
  QString markedText(MarkupData *);
  void setBoxVisible(bool);
  bool tryExplicitLink();
private:
  void initializeFormat();
  MarkupData *markupAt(int pos, MarkupData::Style type);
  MarkupData *markupAt(int start, int end, MarkupData::Style type);
  int refineStart(int start, int base);
  int refineEnd(int end, int base);
protected:
  bool tryFootnote();
  virtual bool tryToPaste();
  bool tryScriptStyles(TextCursor, bool onlyIfBalanced=false);
  bool tryTeXCode(bool noX=false);
  void toggleSimpleStyle(MarkupData::Style type, class TextCursor const &c);
  bool charBeforeIsLetter(int pos) const;
  bool charAfterIsLetter(int pos) const;
  bool shouldResize(QPointF) const;
  void attemptMarkup(QPointF p, MarkupData::Style);
public: // pass-through
  void setFont(QFont f) { text->setFont(f); }
  QColor defaultTextColor() const { return text->color(); }
  void setDefaultTextColor(QColor c) { text->setColor(c); }
  TextItemDoc *document() const { return text; }
  void setTextWidth(double d) { text->setWidth(d); }
  double textWidth() const { return text->width(); }
  TextCursor textCursor() const { return cursor; }
  void setTextCursor(TextCursor c);
public:
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
protected:
  void finalizeConstructor(int sheetOnly=-1);
  bool keyPressAsSimpleStyle(int key, TextCursor const &cursor);
  void tryMove(TextCursor::MoveOperation op, int key,
               Qt::KeyboardModifiers mod);
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
};

#endif
