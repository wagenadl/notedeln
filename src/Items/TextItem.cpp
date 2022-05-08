// Items/TextItem.cpp - This file is part of NotedELN

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

// TextItem.C

#include "TextItem.h"
#include "TextData.h"
#include "Cursors.h"
#include "Mode.h"
#include "EventView.h"
#include "EntryScene.h"
#include "SearchDialog.h"
#include "Style.h"
#include "ResManager.h"
#include "BlockItem.h"
#include "ElnAssert.h"
#include "TeXCodes.h"
#include "Accents.h"
#include "Latin.h"
#include "Digraphs.h"
#include "TextBlockItem.h"
#include "TextItemDoc.h"
#include "LinkHelper.h"
#include "HtmlBuilder.h"
#include "HtmlParser.h"
#include "PageView.h"
#include "Unicode.h"
#include "OneLink.h"

#include <math.h>
#include <QPainter>
#include <QFont>
#include <QKeyEvent>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

#include "LateNoteItem.h" 
#include "LateNoteData.h" 


static bool isLatinLetter(QChar x) {
  return (x>='A' && x<='Z') || (x>='a' && x<='z');
}

static bool isDigit(QChar x) {
  return (x>='0' && x<='9');
}

TextItem::TextItem(TextData *data, Item *parent, bool noFinalize,
		   TextItemDoc *altdoc):
  Item(data, parent) {
  pblock = QPointer<BlockItem>(dynamic_cast<BlockItem*>(parent));
  hasAltDoc = altdoc!=NULL;
  if (altdoc)
    text = altdoc;
  else
    text = TextItemDoc::create(data, this);
  linkHelper = new LinkHelper(this);
  
  mayMark = true;
  mayNote = false;
  mayMove = false;
  lateMarkType = MarkupData::Normal;
  allowParagraphs_ = true;

  if (!altdoc)
    initializeFormat();

  cursor = TextCursor(text);
  
  if (!noFinalize) 
    finalizeConstructor();
  setFlag(ItemIsFocusable);
}

void TextItem::finalizeConstructor(int sheet) {
  foreach (LateNoteData *lnd, data()->children<LateNoteData>())
    if (sheet<0 || lnd->sheet()==sheet)
      create(lnd, this);
  foreach (GfxNoteData *gnd,  data()->children<GfxNoteData>())
    if (!dynamic_cast<LateNoteData *>(gnd)) // ugly, but hey.
      if (sheet<0 || gnd->sheet()==sheet)
	create(gnd, this);

  connect(document(), SIGNAL(contentsChanged(int, int, int)),
	  this, SLOT(docChange()));
  connect(document(), SIGNAL(markupChanged(MarkupData *)),
	  this, SLOT(markupChange(MarkupData *)));
  connect(document(), &TextItemDoc::noLongerEmpty,
          this, &TextItem::redate);
}

void TextItem::redate() {
  BlockItem *bi = ancestralBlock();
  if (bi) {
    EntryScene *es = dynamic_cast<EntryScene *>(bi->baseScene());
    if (es)
      es->redateBlocks();
  }
}  

bool TextItem::allowNotes() const {
  return mayNote;
}

void TextItem::setAllowNotes(bool y) {
  mayNote = y;
}

void TextItem::makeWritable() {
  Item::makeWritable();
  makeWritableNoRecurse();
}

void TextItem::makeWritableNoRecurse() {
  // this ugliness is for the sake of title items that have notes attached
  Item::makeWritableNoRecurse();
  setFlag(ItemIsFocusable);
  setFlag(ItemAcceptsInputMethod);
  text->makeWritable();
  linkHelper->updateAll();
}

void TextItem::setAllowMoves() {
  mayMove = true;
  setAcceptHoverEvents(true);
  setAcceptHoverEvents(true);
}

TextItem::~TextItem() {
}

void TextItem::initializeFormat() {
  setFont(style().font("text-font"));
  setDefaultTextColor(style().color("text-color"));
}

void TextItem::docChange() {
  prepareGeometryChange();
  emit textChanged();
  update();
}

void TextItem::focusInEvent(QFocusEvent *e) {
  QGraphicsItem::focusInEvent(e);
  if (isWritable() && dynamic_cast<LateNoteItem*>(parent())) {
    PageView *pv = EventView::eventView();
    if (pv)
      pv->mode()->enterLateNote();
    else
      qDebug() << "text focusin - no eventview";
  }
}

void TextItem::focusOutEvent(QFocusEvent *e) {
  if (document()->isEmpty()) 
    emit abandoned();

  // drop selection unless it's just the mouse pointer moving out of the window
  if (scene()) {
    QGraphicsItem *fi = scene()->focusItem();
    if (fi != this) {
      cursor.clearSelection();
      update();
      if (dynamic_cast<LateNoteItem*>(parent())) {
        PageView *pv = EventView::eventView();
        if (pv)
          pv->mode()->leaveLateNote();
        else
          qDebug() << "text focusout - no eventview";
      }
    }
  }

  QGraphicsItem::focusOutEvent(e);
}

void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) {
  bool typeOrBrowse = mode()->mode()==Mode::Type
    || mode()->mode()==Mode::Browse;
  if (e->button()==Qt::LeftButton) {
    if (typeOrBrowse) {
      if (!linkHelper->mouseDoubleClick(e))
        // Select word or line or paragraph
        selectWordOrLineOrParagraph(text->find(e->pos()));
    }
    lastClickTime.start();
    lastClickScreenPos = e->screenPos();
  }
  e->accept();
}

void TextItem::handleLeftClick(QGraphicsSceneMouseEvent *e) {
  switch (mode()->mode()) {
  case Mode::Browse:
  case Mode::Type: {
    if (linkHelper->mousePress(e))
      break;
    int pos = text->find(e->pos());
    if (pos>=0 && (!clips() || clipRect().contains(posToPoint(pos)))) {
      cursor.setPosition(pos,
                         e->modifiers() & Qt::ShiftModifier
                         ? TextCursor::KeepAnchor
                         : TextCursor::MoveAnchor); 
      setFocus();
      update();
    }
    if (mayMove) {
      GfxNoteItem *gni = dynamic_cast<GfxNoteItem*>(parent());
      if (gni)
        gni->childMousePress(e->scenePos(), e->button(), e->modifiers());
    }
  } break;
  case Mode::MoveResize:
    if (linkHelper->mousePress(e)) 
      break;
    if (mayMove) {
      GfxNoteItem *gni = dynamic_cast<GfxNoteItem*>(parent());
      if (gni)
        gni->childMousePress(e->scenePos(), e->button(), e->modifiers());
    }
    break;
  case Mode::Highlight:
    attemptMarkup(e->pos(), MarkupData::Emphasize);
    break;
  case Mode::Strikeout:
    attemptMarkup(e->pos(), MarkupData::StrikeThrough);
    break;
  case Mode::Plain:
    attemptMarkup(e->pos(), MarkupData::Normal);
    break;
  default:
    e->ignore();
    break;
  }
}

void TextItem::selectWordOrLineOrParagraph(int pos) {
  if (!cursor.hasSelection()) {
    // Nothing selected => select word
    cursor.selectAround(pos, TextCursor::StartOfWord, TextCursor::EndOfWord);
    setTextCursor(cursor);
    return;
  }

  TextCursor::Range r = cursor.selectedRange();
  if (r.start()==0 && r.end()==text->lastPosition()) {
    // Currently, everything is selected => select nothing
    cursor.clearSelection();
    setTextCursor(cursor);
    return;
  }

  TextCursor c(text, pos);
  c.movePosition(TextCursor::StartOfLine);
  TextCursor d(text, pos);
  d.movePosition(TextCursor::EndOfLine);
  if (r.start()<=c.position() && r.end()>=d.position()) {
    // Currently, line or more is selected => select all
    cursor.selectAround(pos, TextCursor::Start, TextCursor::End);
    setTextCursor(cursor);
    return;
  }

  c.setPosition(pos);
  c.movePosition(TextCursor::StartOfWord);
  d.setPosition(pos);
  d.movePosition(TextCursor::EndOfWord);
  if (r.start()<=c.position() && r.end()>=d.position()) {
    // Currently, word or more is selected => select line
    cursor.selectAround(pos, TextCursor::StartOfLine, TextCursor::EndOfLine);
    setTextCursor(cursor);
    return;
  }

  // Less than word is selected => select word
  cursor.selectAround(pos, TextCursor::StartOfWord, TextCursor::EndOfWord);
  update();
}

void TextItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  e->accept();
  switch (e->button()) {
  case Qt::LeftButton:
    if ((mode()->mode()==Mode::Type || mode()->mode()==Mode::Browse
         || mode()->mode()==Mode::Annotate)
        && lastClickTime.elapsed() < 500
        && (lastClickScreenPos - e->screenPos()).manhattanLength()<5) 
      // Select word or line or paragraph
      selectWordOrLineOrParagraph(text->find(e->pos()));
    else 
      handleLeftClick(e);
    lastClickTime.start();
    lastClickScreenPos = e->screenPos();
    break;
  case Qt::MiddleButton:
    if (mode()->mode()==Mode::Type || mode()->mode()==Mode::Annotate) {
      QClipboard *cb = QApplication::clipboard();
      QString txt = cb->text(QClipboard::Selection);
      if (!txt.isEmpty()) {
      	int pos = pointToPos(e->pos());
      	if (pos>=0) {
      	  cursor.setPosition(pos);
	  cursor.insertText(txt);
	  setFocus();
	}
      }
    }
    break;
  case Qt::RightButton:
    linkHelper->mousePress(e);
  default:
    break;
  }
}

int TextItem::pointToPos(QPointF p, bool strict) const {
  return text->find(p, strict);
}

QPointF TextItem::posToPoint(int pos) const {
  return text->locate(pos);
}
      

void TextItem::attemptMarkup(QPointF p, MarkupData::Style m) {
  int pos = pointToPos(p);
  if (pos<0)
    return;
  lateMarkType = m;
  lateMarkStart = pos;
  grabMouse();
}

void TextItem::representDeadLinks(QList<TransientMarkup> &tmm) {
  // qDebug() << "representdeadlinks";
  for (MarkupData *md: data()->markups()) {
    if (md->style()==MarkupData::Link) {
      // qDebug() << "md link text" << md->text();
      ResManager *resmgr = md->resManager();
      if (!resmgr) {
	// qDebug() << "No resource manager";
	continue;
      }
      Resource *res = resmgr->byTag(md->text());
      if (!res) {
	// qDebug() << "No resource";
	if (!QRegExp("\\d\\d?\\d?\\d?[a-z]?").exactMatch(md->text())) {
	  // qDebug() << "  and not a page";
	  tmm << TransientMarkup(md->start(), md->end(),
				 MarkupData::DeadLink);
	}
      } else if (res->inProgress()) {
	// qDebug() << "  in progress";
	tmm << TransientMarkup(md->start(), md->end(),
			       MarkupData::LoadingLink);
	if (!in_progress_res.contains(res)) {
	  in_progress_res.insert(res);
	  connect(res, SIGNAL(mod()), SLOT(inProgressMod()));
	}
      } else if (res->needsArchive()) {
	// qDebug() << "no archive";
	tmm << TransientMarkup(md->start(), md->end(),
			       MarkupData::DeadLink);
      }
    }
  }
}

void TextItem::representCursor(QList<TransientMarkup> &tmm) const {
  if (cursor.hasSelection())
    tmm << TransientMarkup(cursor.selectionStart(), cursor.selectionEnd(),
                           MarkupData::Selected);
}

void TextItem::representSearchPhrase(QList<TransientMarkup> &tmm) const {
  QString phr = SearchDialog::latestPhrase();
  if (phr.isEmpty())
    return;
  QString txt = text->text();
  int N = phr.length();
  for (int off=txt.indexOf(phr, 0, Qt::CaseInsensitive); off>=0;
       off=txt.indexOf(phr, off+N, Qt::CaseInsensitive)) 
    tmm << TransientMarkup(off, off+N, MarkupData::SearchResult);
}

void TextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *evt) {
  int pos = pointToPos(evt->pos());
  if (pos<0)
    return;
  switch (mode()->mode()) {
  case Mode::Browse:
  case Mode::Type: {
    while (pos>=0) {
      int anc = cursor.hasSelection() ? cursor.anchor() : cursor.position();
      setTextCursor(TextCursor(text, pos, anc));
      QPointF p = posToPoint(pos);
      if (!clips() || clipRect().contains(p))
        break;
      pos -= 1;
    }
    } break;
  case Mode::Highlight:
  case Mode::Strikeout:
  case Mode::Plain: 
    updateMarkup(pos);
    break;
  default:
    break;
  }
}

void TextItem::updateMarkup(int pos) {
  int s, e;
  if (lateMarkStart<pos) {
    s = lateMarkStart;
    e = pos;
  } else {
    s = pos;
    e = lateMarkStart;
  }
  
  if (lateMarkType==MarkupData::Normal) {
    // unmark
    foreach (MarkupData *md, data()->children<MarkupData>()) {
      if (md->isWritable()) {
	int mds = md->start();
	int mde = md->end();
	if (mds<e && mde>s) {
	  MarkupData::Style mdst = md->style();
	  QDateTime cre = md->created();
	  deleteMarkup(md);
	  if (mde>e) {
	    addMarkup(mdst, e, mde);
	    MarkupData *md1 = data()->markupAt(e, mdst);
	    if (md1)
	      md1->setCreated(cre);
	  }
	  if (mds<s) {
	    addMarkup(mdst, mds, s);
	    MarkupData *md1 = data()->markupAt(s, mdst);
	    if (md1)
	      md1->setCreated(cre);
	  }
	}
      }
    }
  } else {
    addMarkup(lateMarkType, s, e); // will be auto-merged
  }
}

void TextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *) {
  ungrabMouse();
  lateMarkType = MarkupData::Normal;
}

bool TextItem::keyPressAsMotion(QKeyEvent *e) {
  switch (e->key()) {
  case Qt::Key_Escape: {
    if (cursor.hasSelection()) {
      cursor.clearSelection();
      update();
    } else {
      clearFocus();
      emit futileMovementKey(e->key(), e->modifiers());
    }
  } return true;
  case Qt::Key_Return: case Qt::Key_Enter:
    if (allowParagraphs_ || e->modifiers() & Qt::ControlModifier) {
      /* Note that this code is not executed for tables. */
      cursor.insertText("\n"); 
      ensureCursorVisible();      
    } else {
      emit futileMovementKey(e->key(), e->modifiers());
    }
    return true;
  case Qt::Key_Backspace:
    if (cursor.atStart() && !cursor.hasSelection()) {
      emit futileMovementKey(e->key(), e->modifiers());
    } else {
      if (cursor.hasSelection())
	prepareGeometryChange(); // needed in title item. Unsure why.
      cursor.deletePreviousChar();
      ensureCursorVisible();      
    }
    return true;
  case Qt::Key_Delete:
    if (cursor.atEnd() && !cursor.hasSelection()) {
      emit futileMovementKey(e->key(), e->modifiers());
    } else {
      if (cursor.hasSelection())
	prepareGeometryChange(); // needed in title item. Unsure why.	
      cursor.deleteChar();
      ensureCursorVisible();
    }
    return true;
  case Qt::Key_Left:
    tryMove(e->modifiers() & Qt::ControlModifier
            ? TextCursor::PreviousWord : TextCursor::Left,
            e->key(), e->modifiers());
    return true;
  case Qt::Key_Up:
    if (e->modifiers() & Qt::ControlModifier) {
      if (cursor.atStart() && !(e->modifiers() & Qt::ShiftModifier)) 
        emit futileMovementKey(e->key(), e->modifiers());
      else
        tryMove(TextCursor::Start, e->key(), e->modifiers());
    } else {
      if (text->lineFor(cursor.position())==0
          && !(e->modifiers() & Qt::ShiftModifier))
        emit futileMovementKey(e->key(), e->modifiers());
      else
        tryMove(TextCursor::Up, e->key(), e->modifiers());
    }
    return true;
  case Qt::Key_Right:
    tryMove(e->modifiers() & Qt::ControlModifier
            ? TextCursor::NextWord : TextCursor::Right,
            e->key(), e->modifiers());
    return true;
  case Qt::Key_Down:
    if (e->modifiers() & Qt::ControlModifier) {
      if (!(e->modifiers() & Qt::ShiftModifier)) 
        emit futileMovementKey(e->key(), e->modifiers());
      else
        tryMove(TextCursor::End, e->key(), e->modifiers());
    } else {
      if (text->lineFor(cursor.position())==text->lineStarts().size()-1
          && !(e->modifiers() & Qt::ShiftModifier))	
        emit futileMovementKey(e->key(), e->modifiers());
      else
        tryMove(TextCursor::Down, e->key(), e->modifiers());
    }
    return true;
  case Qt::Key_Home:
    tryMove(TextCursor::StartOfLine, e->key(), e->modifiers());
    return true;
  case Qt::Key_End:
    tryMove(TextCursor::EndOfLine, e->key(), e->modifiers());
    return true;
  }
  return false;
}

void TextItem::tryMove(TextCursor::MoveOperation op,
                       int key,
                       Qt::KeyboardModifiers mod) {
  TextCursor c = textCursor();
  TextCursor::MoveMode mm = mod & Qt::ShiftModifier
    ? TextCursor::KeepAnchor
    : TextCursor::MoveAnchor;
  c.movePosition(op, mm);
  if (c==textCursor() && !(mod & Qt::ShiftModifier)) {
    emit futileMovementKey(key, mod);
  } else {
    setTextCursor(c);
    QPointF p = posToPoint(c.position());
    if (clips() && !clipRect().contains(p)) {
      qDebug() << "emitting invisible focus" << p << clipRect();
      emit invisibleFocus(c, p);
    }
  }
}

bool TextItem::keyPressWithControl(QKeyEvent *e) {
  if (!(e->modifiers() & Qt::ControlModifier))
    return false;
  if (keyPressAsSimpleStyle(e->key(), textCursor()))
    return true;

  if (mode()->typeMode()==Mode::Math)
    tryTeXCode(true);
  
  switch (e->key()) {
  case Qt::Key_V:
    if (!cursor.hasSelection())
      tryToPaste(); // for text only; image pasting is handled in EntryScene
    return true;
  case Qt::Key_C:
    tryToCopy();
    return true;
  case Qt::Key_X:
    if (tryToCopy()) {
      prepareGeometryChange(); // needed in title item. Unsure why.
      cursor.deleteChar();
    }
    return true;
  case Qt::Key_A:
    cursor.setPosition(0);
    cursor.movePosition(TextCursor::End, TextCursor::KeepAnchor);
    ensureCursorVisible();
    update();
    return true;
  case Qt::Key_N:
    tryFootnote(e->modifiers() & Qt::ShiftModifier);
    return true;
  case Qt::Key_L:
    tryExplicitLink();
    return true;
  case Qt::Key_Semicolon:
    unscriptStyles();
    return true;
  case Qt::Key_Period:
    tryScriptStyles();
    return true;
  case Qt::Key_Backslash:
    tryTeXCode();
    return true;
  case Qt::Key_S:
    cursor.clearSelection();
    text->swapCase(cursor.position());
    cursor.movePosition(TextCursor::Right);
    return true;
  case Qt::Key_T:
    cursor.clearSelection();
    text->transposeCharacters(cursor.position());
    cursor.movePosition(TextCursor::Right);
    return true;
  case Qt::Key_K:
    cursor.clearSelection();
    if (e->modifiers() & Qt::ShiftModifier) 
      cursor.movePosition(TextCursor::StartOfLine, TextCursor::KeepAnchor);
    else
      cursor.movePosition(TextCursor::EndOfLine, TextCursor::KeepAnchor);
    if (cursor.hasSelection())
      cursor.deleteChar();
    return true;
  case Qt::Key_D:
    cursor.clearSelection();
    if (e->modifiers() & Qt::ShiftModifier) 
      cursor.movePosition(TextCursor::EndOfWord, TextCursor::KeepAnchor);
    cursor.deleteChar();
    return true;
  default:
    return false;
  }
}

bool TextItem::keyPressAsSimpleStyle(int key, TextCursor const &cursor) {
  switch (key) {
  case Qt::Key_Slash: case Qt::Key_I:
    toggleSimpleStyle(MarkupData::Italic, cursor);
    return true;
  case Qt::Key_8: case Qt::Key_Asterisk: case Qt::Key_Comma: case Qt::Key_B:
    toggleSimpleStyle(MarkupData::Bold, cursor);
    return true;
  case Qt::Key_6: case Qt::Key_AsciiCircum:
    toggleSimpleStyle(MarkupData::Superscript, cursor);
    return true;
  case Qt::Key_Minus: // Underscore and Minus are on the same key
    // on my keyboard, but they generate different codes
    toggleSimpleStyle(MarkupData::Subscript, cursor);
    return true;
  case Qt::Key_Underscore: case Qt::Key_U:
    toggleSimpleStyle(MarkupData::Underline, cursor);
    return true;
  case Qt::Key_1: case Qt::Key_Exclam:
    toggleSimpleStyle(MarkupData::Emphasize, cursor);
    return true;
  case Qt::Key_Equal:
    toggleSimpleStyle(MarkupData::StrikeThrough, cursor);
    return true;
  default:
    return false;
  }
}

bool TextItem::tryTeXCode(bool noX, bool onlyAtEndOfWord) {
  TextCursor c = cursor;
  if (!c.hasSelection()) {
    TextCursor m = cursor.findBackward(QRegExp("([^A-Za-z])"));
    int start = m.hasSelection() ? m.selectionEnd() : 0;
    if (onlyAtEndOfWord) {
      c.setPosition(start, TextCursor::KeepAnchor);
    } else {
      m.setPosition(start);
      m = m.findForward(QRegExp("([^A-Za-z])"));
      int end = m.hasSelection() ? m.selectionStart() : data()->text().size();
      c.setPosition(start);
      c.setPosition(end, TextCursor::KeepAnchor);
    }
  }
  // got a word
  QString key = c.selectedText();

  if (!TeXCodes::contains(key))
    return false;
  if (noX && TeXCodes::onlyExplicit(key))
    return false;

  if (data()->markupEdgeIn(c.selectionStart(), c.selectionEnd()))
    return false; // don't do it if there is a style split.
  /* This fixes the “x_i” -> “ξ” bug. */

  QString val = TeXCodes::map(key);
  cursor = c;
  cursor.deleteChar(); // delete the word
  if (document()->characterAt(cursor.position()-1)=='\\')
    cursor.deletePreviousChar(); // delete any preceding backslash
  if (val.startsWith("x")) {
    // this is "vec", or "dot", or similar
    if (document()->characterAt(cursor.position()-1).isSpace())
      cursor.deletePreviousChar(); // delete previous space
    cursor.insertText(val.mid(1));
  } else {
    cursor.insertText(val); // insert the replacement code
  }
  return true;
}

bool TextItem::keyPressAsSpecialEvent(QKeyEvent *e) {
  if (e->key()==Qt::Key_Tab || e->key()==Qt::Key_Backtab) {
    TextBlockItem *p = dynamic_cast<TextBlockItem *>(ancestralBlock());
    if (p) 
      if (muckWithIndentation(p, e->modifiers()))
	return true;
  }
  return false;
}

bool TextItem::muckWithIndentation(TextBlockItem *p,
				   Qt::KeyboardModifiers mod) {
  // we are in a text block, so we could fiddle with indentation
  bool hasIndent = p->data()->indented();
  bool hasDedent = p->data()->dedented();
  if (mod & Qt::ControlModifier) {
    p->data()->setDisplayed(!p->data()->displayed());
  } else if (mod & Qt::ShiftModifier) {
    if (hasIndent) 
      p->data()->setIndented(false);
    else if (hasDedent
	     || data()->lineStarts().size()<=data()->paragraphStarts().size())
      p->data()->setIndented(true);
    else
      p->data()->setDedented(true);
  } else if (QRegExp("\\s*").exactMatch(text->text().left(cursor.position()))) {
    if (hasIndent)
      return false; // allow Tab to be inserted at start
    else
      p->data()->setIndented(true);
  } else {
    // no control, no shift, not at start
    if (document()->lineStarts().size()==1) 
      emit multicellular(cursor.position(), data());
    return true; 
  }
  prepareGeometryChange();
  p->muckWithIndentation(cursor);
  update();
  return true;
}

bool TextItem::keyPressAsInsertion(QKeyEvent *e) {
  if (mode()->mode()!=Mode::Type)
    return false;
  QString now = "";
  if (e->modifiers() & Qt::ControlModifier) {
    switch (e->key()) {
    case Qt::Key_2:
      now = QString::fromUtf8("²");
      break;
    case Qt::Key_3:
      now = QString::fromUtf8("³");
      break;
    case Qt::Key_4:
      now = QString::fromUtf8("⁴");
    break;
    case Qt::Key_Enter: case Qt::Key_Return:
      now = QString("\n");
      break;
    case Qt::Key_Space:
      if (e->modifiers() & Qt::ShiftModifier)
	now = QString(QChar(8199)); // figure space
      else
	now = QString(QChar(160)); // unbreakable space
      break;
    default:
      break;
    }
  }
  if (now.isEmpty()) {
    now = e->text();
    if (now[0]<32 || now[0]==127)
      return false; 
  }
  if (!cursor.hasSelection()) {
    cursor.insertText(now);
    if (now=="}" && mode()->typeMode()==Mode::Math) 
      tryScriptStyles(true);
    ensureCursorVisible();
  }
  return true;
}

void TextItem::ensureCursorVisible() {
  QPointF p = posToPoint(cursor.position());
  if (clips() && !clipRect().contains(p)) {
    qDebug() << "emitting invisible focus 2" << p << clipRect();
    emit invisibleFocus(cursor, p);
  }
}  

bool TextItem::keyPressAsBackslash(QKeyEvent *e) {
  // if this keyevent terminates a \tex code.
  if (cursor.hasSelection())
    return false;

  QString txt = e->text();
  if (txt.isEmpty() || txt=="\b" || txt==QChar(127)) // backspace/delete are weird
    return false;

  int pos = cursor.position();
  QString pre = "";
  QString accents = ",.:^_‘’";
  bool hasdig = false;
  bool haslet = false;
  bool hasacc = false;
  while (true) {
    if (--pos < 0)
      return false;
    QChar c = document()->characterAt(pos);
    if (c=='\\') {
      break;
    } else if (isLatinLetter(c)) {
      haslet = true;
      pre = c + pre;
    } else if (isDigit(c)) {
      hasdig = true;
      pre = c + pre;
    } else if (pos>0 && accents.contains(c)
               && document()->characterAt(pos-1)=='\\') {
      hasacc = true;
      pre = c + pre;
      break;
    } else {
      return false;
    }
  }
  if (hasacc && pre.size()!=2)
    return false;
  if (pre.size()<1)
    return false;


  bool subst = hasacc ? true
    : isLatinLetter(txt[0]) ? (hasdig && !haslet)
    : isDigit(txt[0]) ? (haslet && !hasdig)
    : true;

  if (!subst)
    return false;

  
  if (hasacc) {
    if (Accents::contains(pre)) {
      for (int m=0; m<=pre.size(); m++)
        cursor.movePosition(TextCursor::Left, TextCursor::KeepAnchor);
      cursor.deleteChar();
      cursor.insertText(Accents::map(pre));
      ensureCursorVisible();
      return true;
    } else {
      return false;
    }
  } else if (TeXCodes::contains(pre)) {
    for (int m=0; m<=pre.size(); m++)
      cursor.movePosition(TextCursor::Left, TextCursor::KeepAnchor);
    cursor.deleteChar();
    QString sub = TeXCodes::map(pre);
    if (sub.startsWith("x"))
      sub = sub.mid(1);
    cursor.insertText(sub);
    //cursor.insertText(txt);
    ensureCursorVisible();
    return true;
  } else {
    return false;
  }
}

int TextItem::substituteInternalScripts(int start, int end) {
  int sumdelta = 0;
  while (true) {
    MarkupData *md = data()->markupAt(start, end, MarkupData::Superscript);
    if (!md)
      break;
    int s = md->start();
    int e = md->end();
    deleteMarkup(md);
    for (int p=e-1; p>=s; --p) {
      TextCursor cur(document(), p);
      cur.insertText("^");
      sumdelta += 1;
      end += 1;
    }
  }
  while (true) {
    MarkupData *md = data()->markupAt(start, end, MarkupData::Subscript);
    if (!md)
      break;
    int s = md->start();
    int e = md->end();
    deleteMarkup(md);
    for (int p=e-1; p>=s; --p) {
      TextCursor cur(document(), p);
      cur.insertText("_");
      sumdelta += 1;
      end += 1;
    }
  }
  for (int pos=start; pos<end; pos++) {
    QChar c = document()->characterAt(pos);
    if ((c=="^" || c=="_") && pos<end-1) {
      QString s = QString(c) + QString(document()->characterAt(pos+1));
      if (Accents::isScript(s)) {
        TextCursor cur(document(), pos+2, pos);
        cur.deleteChar();
        QString sub = Accents::map(s);
        cur.insertText(sub);
        int delta = sub.size() - s.size();
        end += delta;
        sumdelta += delta;
      }
    }
  }
  return sumdelta;
}      

bool TextItem::keyPressAsDigraph(QKeyEvent *e) {
  if (cursor.hasSelection())
    return false;
  QString charNow = e->text();
  if (charNow=="")
    return false;
  QChar charBefore = document()->characterAt(cursor.position()-1);
  QChar charBefore2 = document()->characterAt(cursor.position()-2);
  QString digraph = QString(charBefore) + charNow;
  QString trigraph = QString(charBefore2) + digraph;
  if (Digraphs::contains(digraph)) {
    cursor.deletePreviousChar();
    cursor.insertText(Digraphs::map(digraph));
    return true;
  } else if (Digraphs::contains(trigraph)) {
    cursor.deletePreviousChar();
    cursor.deletePreviousChar();
    cursor.insertText(Digraphs::map(trigraph));
    return true;
  } else if (Digraphs::contains(charNow)) {
    cursor.insertText(Digraphs::map(charNow));
    return true;
  } else if (charNow=="\"") {
    if (charBefore.isSpace() || charBefore.isNull()
	|| digraph=="(\"" || digraph=="[\"" || digraph=="{\""
	|| digraph==QString::fromUtf8("‘\"")) 
      cursor.insertText(QString::fromUtf8("“"));
    else
      cursor.insertText(QString::fromUtf8("”"));
    return true;
  } else if (digraph==QString::fromUtf8("--")) {
    cursor.deletePreviousChar();
    if (document()->characterAt(cursor.position()-1).isDigit()) 
      cursor.insertText(QString::fromUtf8("‒")); // figure dash
    else 
      cursor.insertText(QString::fromUtf8("–")); // en dash
    return true;
  } else if (charNow[0].isDigit() && charBefore==QChar('-')
	     && QString(" ([{^_@$/").contains(charBefore2)) {
    cursor.deletePreviousChar();
    cursor.insertText(QString::fromUtf8("−")); // replace minus sign
    return false; // insert digit as normal
  } else {
    return false;
  }
}

void TextItem::inputMethodEvent(QInputMethodEvent *e) {
  if (mode()->mode()==Mode::Type) 
    cursor.insertText(e->commitString());
  e->accept();
}
  
void TextItem::keyPressEvent(QKeyEvent *e) {
  if (clips() && !clip_.contains(posToPoint(cursor.position()))) {
    qDebug() << "keypress out of rect"<< posToPoint(cursor.position()) << clip_;
    clearFocus();
    return;
  }
  switch (mode()->mode()) {
  case Mode::Browse:
    if (keyPressInBrowseMode(e))
      e->accept();
    else
      Item::keyPressEvent(e);
    break;
  case Mode::Type:
    if (isWritable()) {
      if (keyPressWithControl(e)) {
        e->accept();
      } else {
        bool bs = mode()->typeMode()!=Mode::Code && keyPressAsBackslash(e);
        if ((mode()->typeMode()==Mode::Math && keyPressAsMath(e))
            || (!bs && mode()->typeMode()!=Mode::Code && keyPressAsDigraph(e))
            || keyPressAsMotion(e)
            || keyPressAsSpecialEvent(e)
            || keyPressAsInsertion(e)
            || bs) {
          e->accept();
        } else {
          Item::keyPressEvent(e);
        }
      }
    } else {
      if (keyPressInBrowseMode(e))
	e->accept();
      else
	Item::keyPressEvent(e);
    }      
    break;
  default:
    Item::keyPressEvent(e);
    break;
  }
}

bool TextItem::keyPressInBrowseMode(QKeyEvent *e) {
  bool ctrl = e->modifiers() & Qt::ControlModifier;
  switch (e->key()) {
  case Qt::Key_C:
    if (ctrl) {
      tryToCopy();
      return true;
    }
    break;
  case Qt::Key_A:
    if (ctrl) {
      cursor.movePosition(TextCursor::Start);
      cursor.movePosition(TextCursor::End, TextCursor::KeepAnchor);
      update();
      return true;
    }
    break;
  case Qt::Key_Escape:
    cursor.clearSelection();
    update();
    return true;
  default:
    break;
  }
  return false;
}

bool TextItem::charBeforeIsLetter(int pos) const {
  return document()->characterAt(pos-1).isLetter();
  // also returns false at start of doc
}

bool TextItem::charAfterIsLetter(int pos) const {
  return document()->characterAt(pos).isLetter();
  // also returns false at end of doc
}


static bool balancedBrackets(QString s) {
  static QString brackets = QString::fromUtf8("()<>{}[]{}⁅⁆〈〉⎡⎤⎣⎦❬❭❰❱❲❳❴❵⟦⟧⟨⟩⟪⟫⟬⟭⦃⦄⦇⦈⦉⦊⦋⦌⦍⦎⦏⦐⦑⦒⦓⦔⦕⦖⦗⦘⧼⧽〈〉《》「」『』【】〔〕〖〗〘〙");
 for (int i=0; i<brackets.size(); i+=2)
   if (s.count(brackets[i]) != s.count(brackets[i+1]))
     return false;
 return true;
}

bool TextItem::unscriptStyles() {
  // drop old super/subscript
  cursor.clearSelection();
  
  MarkupData *oldscript = data()->markupAt(cursor.position(),
					  MarkupData::Superscript);
  if (!oldscript)
    oldscript = data()->markupAt(cursor.position(),
				 MarkupData::Subscript);
  if (!oldscript)
    return false;

  MarkupData::Style s = oldscript->style();
  int start = oldscript->start();
  deleteMarkup(oldscript);
  TextCursor c(cursor);
  c.setPosition(start);
  c.insertText(s==MarkupData::Superscript ? "^" : "_");
  cursor.movePosition(TextCursor::Right);
  return true;
}

bool TextItem::tryAngleBrackets() {
  cursor.clearSelection();
  if (document()->characterAt(cursor.position()-1) != '>')
    return false;
  TextCursor m = cursor.findBackward("<");
  if (!m.hasSelection())
    return false;
  m.clearSelection();
  for (int k=m.position(); k<cursor.position(); k++)
    if (document()->characterAt(k).isSpace())
      return false;
  m.deleteChar();
  m.insertText("⟨");
  cursor.deletePreviousChar();
  cursor.insertText("⟩");
  return true;
}

bool TextItem::tryScriptStyles(bool onlyifbalanced) {
  /* Returns true if we decide to make a superscript or subscript, that is,
     if there is a preceding "^" or "_".
   */
  tryAngleBrackets();
  cursor.clearSelection();
  
  if (data()->markupAt(cursor.position(),
                       MarkupData::Superscript)
      || data()->markupAt(cursor.position(),
                          MarkupData::Subscript))
    return false;

  bool endswithbrace = document()->characterAt(cursor.position()-1) == '}';
  QString re = "(\\^|_)";
  if (endswithbrace || onlyifbalanced)
    re += "\\{";
      
  TextCursor m = cursor.findBackward(QRegExp(re));
  if (!m.hasSelection()) {
    if (endswithbrace)
      return false;
    re = "(\\^|_)";
    m = cursor.findBackward(QRegExp(re));
    if (!m.hasSelection())
      return false; // no "^" or "_"
  }
  if (m.selectionEnd() == cursor.position())
    return false; // empty target

  QString mrk = m.selectedText();

  if (onlyifbalanced) {
    TextCursor scr(document(), m.selectionStart() + 1);
    scr.setPosition(cursor.position(), TextCursor::KeepAnchor);
    if (!balancedBrackets(scr.selectedText()))
      return false;
  }

  if (endswithbrace) {
    cursor.deletePreviousChar();
    cursor.correctPosition(substituteInternalScripts(m.position(),
                                                     cursor.position()));
  }
  cursor.correctPosition(-m.deleteChar());

  addMarkup(mrk.startsWith("^")
	    ? MarkupData::Superscript
	    : MarkupData::Subscript,
	    m.selectionStart(), cursor.position());
  ensureCursorVisible();
  return true;
}

void TextItem::tryItalicizeAbbreviation(TextCursor const &c) {
  TextCursor w = c;
  w.selectAround(c.position()-2,
		 TextCursor::StartOfWord, TextCursor::EndOfWord);
  QString word = w.selectedText();
  QSet<QString> dict = Latin::abbrev(word);
  int start = -1;
  if (!dict.isEmpty()) {
    int p = w.selectionStart() - 1;
    for (QString s: dict) {
      if (document()->selectedText(p-s.length(), p).toLower() == s) {
	start = p-s.length();
	break;
      }
    }
  }
  if (start>=0) {
    int end = c.position();
    MarkupData *oldmd = data()->markupAt(start, MarkupData::Italic);
    if (oldmd && oldmd->start()==start && oldmd->end()==end) {
      deleteMarkup(oldmd);
    } else if (start<end) {
      addMarkup(MarkupData::Italic, start, end);
    }
  }
}

void TextItem::toggleSimpleStyle(MarkupData::Style type,
                                 TextCursor const &c) {
  int start = -1;
  int end = -1;
  if (c.hasSelection()) {
    start = c.selectionStart();
    end = c.selectionEnd();
  } else {
    int base = c.position();
    start = end = base;
    int di = 1;
    while (Unicode::isCombining(document()->characterAt(start-di)))
      di++;
    if (document()->characterAt(start-di).isDigit()) {
      start -= di;
      while (true) {
        di = 1;
        while (Unicode::isCombining(document()->characterAt(start-di)))
          di++;
        if (!document()->characterAt(start-di).isDigit())
          break;
	start -= di;
      }
      while (document()->characterAt(end).isDigit()) {
        end++;
        while (Unicode::isCombining(document()->characterAt(end)))
          end++;
      }
    } else if (document()->characterAt(start-di).isLetter()) {
      start -= di;
      while (true) {
        di = 1;
        while (Unicode::isCombining(document()->characterAt(start-di)))
          di++;
        if (!document()->characterAt(start-di).isLetter())
          break;
	start -= di;
      }
      while (document()->characterAt(end).isLetter()) {
        end++;
        while (Unicode::isCombining(document()->characterAt(end)))
          end++;
      }
    } else {
      if (type == MarkupData::Italic && document()->characterAt(base-1)=='.')
	tryItalicizeAbbreviation(c);
      return;
    }
    start = refineStart(start, base);
    end = refineEnd(end, base);
  }

  if (type == MarkupData::Italic) {
    // Try latin phrases "in vivo" etc. They are italicized as one.
    QString word = document()->selectedText(start, end);
    QSet<QString> const &dict = Latin::normal(word);
    if (!dict.isEmpty()) {
      for (QString s: dict) {
	if (document()->selectedText(start-1-s.size(), start-1)
	    .toLower() == s) {
	  start -= 1 + s.size();
	  break;
	}
      }
    }
  }
  
  MarkupData *oldmd = data()->markupAt(start, type);
  
  if (oldmd) {
    // touch or overlap
    int ostart = oldmd->start();
    int oend = oldmd->end();
    if (oend>=end) {
      // whole section was marked up, we'll drop it
      deleteMarkup(oldmd);
      if (ostart<start)
        addMarkup(type, ostart, start);
      if (oend>end)
        addMarkup(type, end, oend);
    } else {
      addMarkup(type, start, end);
    }
  } else if (start<end) {
    addMarkup(type, start, end);
  }
}

void TextItem::deleteMarkup(MarkupData *d) {
  int s = d->start();
  int e = d->end();
  linkHelper->removeMarkup(d);
  data()->deleteMarkup(d);
  reftexts.remove(d);
  text->partialRelayout(s, e);
  emit textChanged();
  update();
}
  
MarkupData *TextItem::addMarkup(MarkupData::Style t, int start, int end) {
  MarkupData *md = new MarkupData(start, end, t);
  addMarkup(md);
  return md;
}

void TextItem::addMarkup(MarkupData *d) {
  bool isnew;
  d = data()->mergeMarkup(d, &isnew);
  if (d->style()==MarkupData::FootnoteRef)
    reftexts[d] = d->text();
  if (isnew)
    linkHelper->newMarkup(d);
  text->partialRelayout(d->start(), d->end());
  emit textChanged(); // so that TBI gets size to fit
  update();
}

int TextItem::refineStart(int start, int base) {
  /* Shrinks a region for applysimplestyle to not cross any other style edges
     This function shrinks from the start.
   */
  foreach (MarkupData *md, data()->children<MarkupData>()) {
    int s = md->start();
    int e = md->end();
    if (s>start && s<base)
      start = s;
    if (e>start && e<base)
      start = e;
  }
  return start;
}

int TextItem::refineEnd(int end, int base) {
  /* Shrinks a region for applysimplestyle to not cross any other style edges.
     This function shrinks from the end.
   */
  foreach (MarkupData *md, data()->children<MarkupData>()) {
    int s = md->start();
    int e = md->end();
    if (s>=base && s<end)
      end = s;
    if (e>=base && e<end)
      end = e;
  }
  return end;
}

static QString approvedMark(TextCursor m) {
  static QString marks = "*@#%$&+"; // Add more?
  QString out = "";
  int n = m.position();
  TextItemDoc *doc = m.document();
  while (n>0) {
    QChar c = doc->characterAt(--n);
    if (marks.contains(c))
      out += c;
    else
      break;
  }
  return out;
}

static QString substituteMark(QString s) {
  static QMap<QString, QString> map;
  if (map.isEmpty()) {
    map["+"] = QString::fromUtf8("†");
    map["++"] = QString::fromUtf8("‡");
    map["@"] = QString::fromUtf8("¶");
    map["$"] = QString::fromUtf8("§");
    map["#"] = QString::fromUtf8("♯");
  }    
  return map.contains(s) ? map[s] : s;
}

static TextCursor linkAt(TextCursor const &c) {
  if (c.hasSelection())
    return c;

  TextItemDoc *doc = c.document();

  TextCursor e = c.findForward(QRegExp("\\s"));
  int end = e.hasSelection() ? e.selectionStart() : -1;
  if (end<0) {
    e = c;
    e.movePosition(TextCursor::End);
    end = e.position();
  }
  QString endchars = QString::fromUtf8(";:.,)]}’”!?—");
  while (end>0 && endchars.contains(doc->characterAt(end-1)))
    end--;

  TextCursor s = c.findBackward(QRegExp("\\s"));
  int start = s.hasSelection() ? s.selectionEnd() : 0;
  QString startchars = QString::fromUtf8("([{‘“¡¿—");
  while (start<end && startchars.contains(doc->characterAt(start)))
    start++;

  if (start>=end)
    return TextCursor();
    
  // Now, start..end is the area that we will work with
  TextCursor m = c;
  m.setPosition(start);
  m.setPosition(end, TextCursor::KeepAnchor);
  return m;
}
  
bool TextItem::tryExplicitLink() {
  TextCursor m = linkAt(textCursor());
  if (!m.hasSelection())
    return false;
  int start = m.selectionStart();
  int end = m.selectionEnd();
  MarkupData *oldmd = data()->markupAt(start, end, MarkupData::Link);
  if (oldmd) {
    // undo link mark
    deleteMarkup(oldmd);
    // if the old link exactly matches our selection, just drop it;
    // otherwise, replace it.
    if  (oldmd->start()==start && oldmd->end()==end) 
      return false;
  }
  if (end>start) {
    addMarkup(MarkupData::Link, start, end);
    return true;
  } else {
    return false;
  }
}

bool TextItem::tryFootnote(bool del) {
  BlockItem *anc = ancestralBlock();
  if (!anc) {
    /* This happens when trying to add a footnote to a gfxnote or latenote. */
    qDebug() << "Cannot add footnote without ancestral block";
    return false;
  }

  EntryScene *bs = dynamic_cast<EntryScene*>(anc->baseScene());
  if (!bs) {
    /* This happens when trying to add a footnote to a footnote. */
    qDebug() << "Cannot add footnote without scene";
    return false;
  }
  int i = bs->findBlock(anc);
  if (i<0) {
    /* This could happen when trying to add a footnote to a footnote,
       if somehow setBaseScene had been called on that note. */
    qDebug() << "Cannot add footnote if block is not in scene";
    return false;
  }
  
  TextCursor c = textCursor();
  MarkupData *oldmd = data()->markupAt(c.selectionStart(), c.selectionEnd(),
				       MarkupData::FootnoteRef);
  int start=-1;
  int end=-1;
  QString symMark;
  if (c.hasSelection()) {
    start = c.selectionStart();
    end = c.selectionEnd();
  } else {
    TextCursor m = c.findBackward(QRegExp("[^-\\w]"));
    QString mrk = m.selectedText();
    start = m.hasSelection() ? m.selectionEnd() : 0;
    m = c.findForward(QRegExp("[^-\\w]"));
    end = m.hasSelection() ? m.selectionStart() : data()->text().size();
    if (start==end && start>0) {
      symMark = approvedMark(c);
      // symMark is one or more non-word chars like "*".      
      // symMark is empty if it was not an approved mark
      start -= symMark.size();
    }
  }

  if (del) {
    if (oldmd && oldmd->start()==start && oldmd->end()==end) {
      // delete old mark
      QString tag = oldmd->text();
      deleteMarkup(oldmd);
      BlockItem *bi = ancestralBlock();
      bi->refTextChange(tag, ""); // remove footnote or whatever
      return true;
    }
  } else {
    if (oldmd) {
      bs->focusFootnote(i, oldmd->text());
      anc->markPreviousFocus(data()->uuid());
      return true;
    } else if (start<end) {
      if (!symMark.isEmpty()) {
	QString repl = substituteMark(symMark);
	if (repl!=symMark) {
	  cursor.setPosition(start);
	  cursor.setPosition(end, TextCursor::KeepAnchor);
	  cursor.deleteChar();
	  cursor.insertText(repl);
	  end = cursor.position();
	  start = end - repl.length();
	}
	addMarkup(MarkupData::Superscript, start, end);
      }
      MarkupData *md = addMarkup(MarkupData::FootnoteRef, start, end);
      bs->newFootnote(i, md->text());
      anc->markPreviousFocus(data()->uuid());
      return true;
    }
  }
  return false;
}

bool TextItem::tryToCopy() const {
  if (!cursor.hasSelection())
    return false;
  TextCursor::Range r = cursor.selectedRange();
  QString html = toHtml(r.start(), r.end());
  QClipboard *cb = QApplication::clipboard();
  QMimeData *md = new QMimeData();
  md->setText(cursor.selectedText());
  md->setHtml(html);
  cb->setMimeData(md);
  return true;
}

bool TextItem::tryToPaste(bool nonewlines) {
  QClipboard *cb = QApplication::clipboard();
  QMimeData const *md = cb->mimeData(QClipboard::Clipboard);
  if (data()->isEmpty() && md->hasText() && md->text().contains("\t")) {
    // we should become a table item and paste in there.
    emit multicellularpaste(data(), md->text());
    return true;
  }
  if (md->hasHtml()) {
    QString txt = md->html();
    if (cursor.hasSelection())
      cursor.deleteChar();
    cursor = insertBasicHtml(txt, cursor.position(), nonewlines,
			     md->hasText() ? md->text() : QString());
    cursor.clearSelection();
    return true;   
  } else if (md->hasText()) {
    QString txt = md->text();
    txt.replace(QRegExp("[\\x0000-\\x0008\\x000b-\\x001f]"), "");
    txt.replace("\t", " ");
    if (nonewlines)
      txt.replace("\n", " ");
    cursor.insertText(txt);
    return true;
  } else {
    return false;
  }
}  

bool TextItem::allowParagraphs() const {
  return allowParagraphs_;
}

void TextItem::setAllowParagraphs(bool yes) {
  allowParagraphs_ = yes;
}

Qt::CursorShape TextItem::cursorShape(Qt::KeyboardModifiers m) const {
  Qt::CursorShape cs = defaultCursorShape();
  switch (mode()->mode()) {
  case Mode::Type:
    if (mayMove && (m & Qt::ControlModifier)) {
      GfxNoteItem *gni = dynamic_cast<GfxNoteItem*>(parent());
      if (gni && gni->shouldResize(mapToScene(cursorPos)))
        cs = Qt::SplitHCursor;
      else
        cs = Qt::SizeAllCursor;
    } else if (isWritable()) {
      cs = Qt::IBeamCursor;
    }
    break;
  case Mode::Annotate: case Mode::Mark: case Mode::Draw:
    cs = Qt::CrossCursor;
    break;
  case Mode::MoveResize:
    if (mayMove) {
      GfxNoteItem *gni = dynamic_cast<GfxNoteItem*>(parent());
      if (gni && gni->shouldResize(mapToScene(cursorPos)))
        cs = Qt::SplitHCursor;
      else
        cs = Qt::SizeAllCursor;
    }
    break;
  case Mode::Highlight: case Mode::Strikeout: case Mode::Plain:
    cs = Qt::UpArrowCursor;
    break;
  default:
    break;
  }
  return cs;
}

bool TextItem::changesCursorShape() const {
  return true;
}

void TextItem::hoverMoveEvent(QGraphicsSceneHoverEvent *e) {
  cursorPos = e->pos(); // cache for the use of modifierChanged
  setCursor(Cursors::refined(cursorShape(e->modifiers())));
  linkHelper->mouseMove(e);
  Item::hoverMoveEvent(e);
  e->accept();
}

void TextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  cursorPos = e->pos(); // cache for the use of modifierChanged
  setCursor(Cursors::refined(cursorShape(e->modifiers())));
  linkHelper->mouseMove(e);
  Item::hoverLeaveEvent(e);
  e->accept();
}

void TextItem::markupChange(MarkupData *md) {
  if (!md)
    return;
  switch (md->style()) {
  case MarkupData::FootnoteRef: {
    QString olds = reftexts.contains(md) ? reftexts[md] : "";
    QString news = md->text();
    if (news!=olds) {
      reftexts[md] = news;
      BlockItem *bi = ancestralBlock();
      if (bi)
	bi->refTextChange(olds, news);
    }
  } break;
  case MarkupData::Link:
    linkHelper->updateMarkup(md);
    break;
  default:
    break;
  }
  if (md->isEmpty())
    deleteMarkup(md);
}

QRectF TextItem::boundingRect() const {
  return text->boundingRect().adjusted(-10, 0, 10, 0);
}

void TextItem::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) {
  if (!text)
    return;
  
  if (clips())
    p->setClipRect(clip_);
  else
    p->setClipRect(boundingRect().adjusted(-10,0,10,0));

  QList<TransientMarkup> tmm;
  representCursor(tmm);
  representSearchPhrase(tmm);
  representDeadLinks(tmm);
  text->render(p, tmm);

  if (hasFocus() && mode()->mode()==Mode::Type && isWritable())
    renderCursor(p, cursor.position());
}

void TextItem::renderCursor(QPainter *p, int pos) {
  QPointF xy = text->locate(pos);
  MarkupStyles sty;
  QList<MarkupData::Style> relevantStyles;
  relevantStyles
    << MarkupData::Bold
    << MarkupData::Italic
    << MarkupData::Subscript
    << MarkupData::Superscript;
  foreach (MarkupData::Style s, relevantStyles) {
    MarkupData *md = data()->markupAt(pos, s);
    if (md && md->start()<pos && md->end()>pos)
      sty.add(s);
  }
  xy += QPointF(-2, text->baselineShift(sty));
  if (clips() && !clip_.contains(xy)) {
    qDebug() << "xy" << xy << clip_;
    qDebug() << "NOT Relinquishing focus on redraw: out of rectangle";
    //    clearFocus();
    //    return;
  }
  
  PageView *pv = EventView::eventView();
  if (pv) {
    pv->markCursor(mapToScene(xy), text->font(sty), QColor("red"));
  } else {
    p->setFont(text->font(sty));
    p->setPen(QPen(QColor("red")));
    p->drawText(xy, "|");
  }
}

void TextItem::setTextWidth(double d, bool relayout) {
  text->setWidth(d);
  if (relayout) {
    prepareGeometryChange();
    text->relayout();
  }
}

TextCursor TextItem::insertBasicHtml(QString html, int pos, bool nonewlines,
				     QString ref) {
  HtmlParser p(html);
  TextCursor c(cursor);
  c.setPosition(pos);
  if (ref.isNull() || p.text()==ref) {
    QString txt = p.text();
    txt.replace(QRegExp("[\\x0000-\\x0008\\x000b-\\x001f]"), "");
    txt.replace("\t", " ");
    if (nonewlines)
      txt.replace("\n", " ");
    c.insertText(txt);
    foreach (MarkupData *md, p.markups()) 
      addMarkup(md->style(), md->start()+pos, md->end()+pos);
  } else {
    ref.replace(QRegExp("[\\x0000-\\x0008\\x000b-\\x001f]"), "");
    ref.replace("\t", " ");
    if (nonewlines)
      ref.replace("\n", " ");
    c.insertText(ref);
  }
  c.setPosition(pos, TextCursor::KeepAnchor);
  c.exchangePositionAndAnchor();
  return c;
}

QRectF TextItem::netBounds() const {
  return text->boundingRect();
}

QRectF TextItem::clipRect() const {
  return clip_;
}

bool TextItem::clips() const {
  return !clip_.isNull();
}

void TextItem::setClip(QRectF r) {
  clip_ = r.adjusted(-10, 0, 1000, 0);
  update();
}

void TextItem::unclip() {
  clip_ = QRectF();
  update();
}

void TextItem::setTextCursor(TextCursor const &tc) {
  cursor = tc;
  if (mode()->mode()==Mode::Type || mode()->mode()==Mode::Browse)
    setFocus();
  else
    clearFocus();
  if (cursor.hasSelection()) {
    QClipboard *cb = QApplication::clipboard();
    cb->setText(cursor.selectedText(), QClipboard::Selection);
  }    
  update();
}

void TextItem::setLineHeight(double h) {
  text->setLineHeight(h);
  //text->recalculateCharacterWidths();
  update();
}

void TextItem::setFont(QFont f) {
 text->setFont(f);
 //text->recalculateCharacterWidths();
 update();
}

QString TextItem::toHtml(int start, int end) const {
  HtmlBuilder builder(data(), start, end);
  return builder.toHtml();
}  

void TextItem::setParentBlock(BlockItem *bi) {
  pblock = bi;
}

BlockItem const *TextItem::ancestralBlock() const {
  BlockItem const *bi = pblock;
  if (bi)
    return bi;
  else
    return Item::ancestralBlock();
}

BlockItem *TextItem::ancestralBlock() {
  BlockItem *bi = pblock;
  if (bi)
    return bi;
  else
    return Item::ancestralBlock();
}

void TextItem::inProgressMod() {
  Resource *res = dynamic_cast<Resource *>(sender());
  update();
  if (res && !res->inProgress()) {
    disconnect(res, SIGNAL(mod()), this, SLOT(inProgressMod()));
    in_progress_res.remove(res);
  }
}
