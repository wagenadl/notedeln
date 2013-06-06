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

#include <QGraphicsTextItem>
#include <QTextCursor>
#include "MarkupData.H"
#include "Item.H"
#include "TextItemText.H"
#include "Mode.H"
#include "TextData.H"

class TextItem: public Item {
  Q_OBJECT;
public:
  TextItem(TextData *data, Item *parent, bool noFinalize=false);
  // only TableItem should set noFinalize=true!
  ~TextItem();
  DATAACCESS(TextData);
  virtual void makeWritable();
  virtual void makeWritableNoRecurse();
  bool allowParagraphs() const;
  void setAllowParagraphs(bool yes);
  void setAllowMoves();
  void addMarkup(MarkupData::Style t, int start, int end);
  void addMarkup(MarkupData *); // we appropriate the data!
  bool allowNotes() const;
  void setAllowNotes(bool y=true);
  void insertBasicHtml(QString html, int pos);
  // Only <b> and <i> tags are supported at present
  int pointToPos(QPointF) const;
  //  QRectF fittedRect() const;
  QRectF netBounds() const;
signals:
  void textChanged();
  void widthChanged();
  void abandoned(); // escape pressed while empty
  void futileMovementKey(int, Qt::KeyboardModifiers);
  /* also emitted for enter press or for delete/backspace at end/start */
  void mousePress(QPointF, Qt::MouseButton);
  void refTextChange(QString oldText, QString newText);
protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
protected:
  friend class TextItemText;
  virtual bool mousePress(QGraphicsSceneMouseEvent *);
  virtual bool mouseDoubleClick(QGraphicsSceneMouseEvent *);
  virtual bool keyPress(QKeyEvent *);
  virtual bool focusIn(QFocusEvent *);
  virtual bool focusOut(QFocusEvent *);
private:
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
  void hoverMove(QGraphicsSceneHoverEvent *);
  void setBoxVisible(bool);
  bool tryExplicitLink();
private:
  void initializeFormat();
  MarkupData *markupAt(int pos, MarkupData::Style type);
  MarkupData *markupAt(int start, int end, MarkupData::Style type);
protected:
  bool tryFootnote();
  virtual bool tryToPaste();
  bool trySimpleStyle(QString marker, MarkupData::Style type);
  bool tryScriptStyles();
  bool tryAutoLink();
  bool tryTeXCode();
  void toggleSimpleStyle(MarkupData::Style type, class QTextCursor const &c);
  bool charBeforeIsLetter(int pos) const;
  bool charAfterIsLetter(int pos) const;
  bool shouldResize(QPointF) const;
  void attemptMarkup(QPointF p, MarkupData::Style);
public: // pass-through
  void setFont(QFont f) { text->setFont(f); }
  QColor defaultTextColor() const { return text->defaultTextColor(); }
  void setDefaultTextColor(QColor c) { text->setDefaultTextColor(c); }
  QTextDocument *document() const { return text->document(); }
  void setTextWidth(double d);
  double textWidth() const { return text->textWidth(); }
  QTextCursor textCursor() const { return text->textCursor(); }
  void setTextCursor(QTextCursor c) { text->setTextCursor(c); }
public:
  virtual QRectF boundingRect() const;
  virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
protected:
  void finalizeConstructor();
protected:
  bool mayMark;
  bool mayNote;
  bool allowParagraphs_;
  class TextMarkings *markings_;
  class TextItemText *text;
  bool mayMove;
  QPointF cursorPos;
  MarkupData::Style lateMarkType;
  int lateMarkStart;
};

#endif
