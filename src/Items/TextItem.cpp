// Items/TextItem.cpp - This file is part of eln

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

// TextItem.C

#include "TextItem.h"
#include "TextData.h"
#include "Mode.h"
#include "EntryScene.h"
#include "SearchDialog.h"
#include "Style.h"
#include "ResManager.h"
//#include "HoverRegion.h"
#include "BlockItem.h"
#include "ResourceMagic.h"
#include "Assert.h"
#include "TeXCodes.h"
#include "Digraphs.h"
#include "TextBlockItem.h"
#include "Cursors.h"
#include "TextItemDoc.h"
#include "LinkHelper.h"
#include "HtmlBuilder.h"
#include "HtmlParser.h"

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

TextItem::TextItem(TextData *data, Item *parent, bool noFinalize,
		   TextItemDoc *altdoc):
  Item(data, parent) {
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
  text->makeWritable();
  linkHelper->updateAll();
}

void TextItem::setAllowMoves() {
  mayMove = true;
  setAcceptHoverEvents(true);
  setAcceptHoverEvents(true);
  connect(mode(), SIGNAL(modeChanged(Mode::M)), SLOT(modeChange(Mode::M)));
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
    
    if (mode()->mode()!=Mode::Type)
      if (hasFocus())
        clearFocus();
  }
  e->accept();
}

void TextItem::handleLeftClick(QGraphicsSceneMouseEvent *e) {
  switch (mode()->mode()) {
  case Mode::Browse:
    linkHelper->mousePress(e);
    break;
  case Mode::Type: {
    if (linkHelper->mousePress(e))
      break;
    int pos = text->find(e->pos());
    if (pos>=0) {
      cursor.setPosition(pos,
                         e->modifiers() & Qt::ShiftModifier
                         ? TextCursor::KeepAnchor
                         : TextCursor::MoveAnchor);
      setFocus();
      update();
    }
  } break;
  case Mode::MoveResize:
    if (mayMove) {
      bool resize = shouldResize(e->pos());
      GfxNoteItem *gni = dynamic_cast<GfxNoteItem*>(parent());
      if (gni)
        gni->childMousePress(e->scenePos(), e->button(), resize);
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
  switch (e->button()) {
  case Qt::LeftButton:
    if ((mode()->mode()==Mode::Type || mode()->mode()==Mode::Browse)
        && lastClickTime.elapsed() < 500
        && (lastClickScreenPos - e->screenPos()).manhattanLength()<5) 
      // Select word or line or paragraph
      selectWordOrLineOrParagraph(text->find(e->pos()));
    else 
      handleLeftClick(e);
    lastClickTime.start();
    lastClickScreenPos = e->screenPos();
    if (mode()->mode()!=Mode::Type && hasFocus())
      clearFocus();
    break;
  case Qt::MiddleButton:
    if (mode()->mode() == Mode::Type) {
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
  default:
    break;
  }
  e->accept();
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
  qDebug() << "rSP" << txt << phr;
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
    int anc = cursor.hasSelection() ? cursor.anchor() : cursor.position();
    setTextCursor(TextCursor(text, pos, anc));
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
      if (md->isRecent() && (md->style()==MarkupData::Emphasize
			     || md->style()==MarkupData::StrikeThrough)) {
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
    }
  } return true;
  case Qt::Key_Return: case Qt::Key_Enter:
    if (allowParagraphs_) {
      cursor.insertText("\n");
    } else {
      emit futileMovementKey(e->key(), e->modifiers());
    }
    return true;
  case Qt::Key_Backspace:
    if (cursor.atStart() && !cursor.hasSelection()) {
      emit futileMovementKey(e->key(), e->modifiers());
    } else {
      cursor.deletePreviousChar();
    }
    return true;
  case Qt::Key_Delete:
    if (cursor.atEnd() && !cursor.hasSelection()) {
      emit futileMovementKey(e->key(), e->modifiers());
    } else {
      cursor.deleteChar();
    }
    return true;
  case Qt::Key_Left:
    tryMove(TextCursor::Left, e->key(), e->modifiers());
    return true;
  case Qt::Key_Up:
    if (text->lineFor(cursor.position())==0)
      emit futileMovementKey(e->key(), e->modifiers());
    else
      tryMove(TextCursor::Up, e->key(), e->modifiers());
    return true;
  case Qt::Key_Right:
    tryMove(TextCursor::Right, e->key(), e->modifiers());
    return true;
  case Qt::Key_Down:
    if (text->lineFor(cursor.position())==text->lineStarts().size()-1)
      emit futileMovementKey(e->key(), e->modifiers());
    else
      tryMove(TextCursor::Down, e->key(), e->modifiers());
    return true;
  case Qt::Key_Home:
    tryMove(TextCursor::StartOfLine, e->key(), e->modifiers());
    return true;
  case Qt::Key_End:
    tryMove(TextCursor::EndOfLine, e->key(), e->modifiers());
    return true;
    /*
  case Qt::Key_PageUp: case Qt::Key_PageDown: {
    TextCursor pre = textCursor();
    text->internalKeyPressEvent(e);
    TextCursor post = textCursor();
    if (e->key()==Qt::Key_PageDown) {
      qDebug() << "TextItem::pagedown " << pre.position() << ";" << post.position();
    }
    if (pre.position() == post.position())
      emit futileMovementKey(e->key(), e->modifiers());
    return true;
  } break;
    */
  }
  return false;
}

void TextItem::tryMove(TextCursor::MoveOperation op,
                       int key,
                       Qt::KeyboardModifiers mod) {
  TextCursor c = textCursor();
  TextCursor::MoveMode mm = mod & Qt::ShiftModifier ? TextCursor::KeepAnchor
    : TextCursor::MoveAnchor;
  c.movePosition(op, mm);
  if (c==textCursor()) {
    emit futileMovementKey(key, mod);
    return;
  }

  setTextCursor(c);
  QPointF p = posToPoint(c.position());
  if (!clipRect().contains(p))
    emit invisibleFocus(p);
}

bool TextItem::keyPressWithControl(QKeyEvent *e) {
  if (!(e->modifiers() & Qt::ControlModifier))
    return false;
  if (keyPressAsSimpleStyle(e->key(), textCursor()))
    return true;

  if (mode()->mathMode())
    tryTeXCode(true);
  
  switch (e->key()) {
  case Qt::Key_V:
    tryToPaste();
    return true;
  case Qt::Key_C:
    tryToCopy();
    return true;
  case Qt::Key_X:
    if (tryToCopy())
      cursor.deleteChar();
    return true;
  case Qt::Key_A:
    cursor.setPosition(0);
    cursor.movePosition(TextCursor::End, TextCursor::KeepAnchor);
    return true;
  case Qt::Key_N:
    tryFootnote();
    return true;
  case Qt::Key_L:
    tryExplicitLink();
    return true;
  case Qt::Key_Period:
    tryScriptStyles();
    return true;
  case Qt::Key_Backslash:
    tryTeXCode();
    return true;
  case Qt::Key_2:
    cursor.insertText(QString::fromUtf8("²"));
    return true;
  case Qt::Key_3:
    cursor.insertText(QString::fromUtf8("³"));
    return true;
  case Qt::Key_4:
    cursor.insertText(QString::fromUtf8("⁴"));
    return true;
  case Qt::Key_Space:
    cursor.insertText(QString::fromUtf8(" "));
    return true;
  case Qt::Key_Enter: case Qt::Key_Return:
    cursor.insertText(QString("\n"));
    return true;
  default:
    return false;
  }
}

bool TextItem::keyPressAsSimpleStyle(int key, TextCursor const &cursor) {
  switch (key) {
  case Qt::Key_Slash:
    if (mode()->mathMode())
      tryTeXCode();
    toggleSimpleStyle(MarkupData::Italic, cursor);
    return true;
  case Qt::Key_8: case Qt::Key_Asterisk: case Qt::Key_Comma:
    toggleSimpleStyle(MarkupData::Bold, cursor);
    return true;
  case Qt::Key_6: // case Qt::Key_Hat:
    toggleSimpleStyle(MarkupData::Superscript, cursor);
    return true;
  case Qt::Key_Minus: // Underscore and Minus are on the same key
    // on my keyboard, but they generate different codes
    toggleSimpleStyle(MarkupData::Subscript, cursor);
    return true;
  case Qt::Key_Underscore:
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
  if (noX && key.size()==1)
    return false;
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
    TextBlockItem *p = dynamic_cast<TextBlockItem *>(parent());
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
    else if (hasDedent)
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
  p->initializeFormat();
  text->relayout();
  update();
  return true;
}

bool TextItem::keyPressAsInsertion(QKeyEvent *e) {
  if (mode()->mode()!=Mode::Type)
    return false;
  QString now = e->text();
  if (now.isEmpty() || now[0]<32 || now[0]==127)
    return false;
  cursor.insertText(now);
  return true;
}

bool TextItem::keyPressAsSpecialChar(QKeyEvent *e) {
  QChar charBefore = document()->characterAt(cursor.position()-1);
  QChar charBefore2 = document()->characterAt(cursor.position()-2);
  QString charNow = e->text();
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

  
void TextItem::keyPressEvent(QKeyEvent *e) {
  if (keyPressWithControl(e)
      || keyPressAsSpecialChar(e)
      || (mode()->mathMode() && keyPressAsMath(e))
      || keyPressAsMotion(e)
      || keyPressAsSpecialEvent(e)
      || keyPressAsInsertion(e))
    e->accept();
  else
    e->ignore();
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

bool TextItem::tryScriptStyles(bool onlyIfBalanced) {
  /* Returns true if we decide to make a superscript or subscript, that is,
     if there is a preceding "^" or "_".
   */
  TextCursor m = cursor.findBackward(QRegExp("\\^|_"));
  if (!m.hasSelection())
    return false; // no "^" or "_"
  if (m.selectionEnd() == cursor.position())
    return false; // empty target

  QString mrk = m.selectedText();

  if (onlyIfBalanced) {
    TextCursor scr(m);
    scr.setPosition(cursor.position(), TextCursor::KeepAnchor);
    if (!balancedBrackets(scr.selectedText()))
      return false;
  }  

  cursor.movePosition(TextCursor::Left);
  m.deleteChar();

  addMarkup(mrk=="^"
	    ? MarkupData::Superscript
	    : MarkupData::Subscript,
	    m.selectionStart(), cursor.position());
  return true;
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
    if (document()->characterAt(base-1).unicode() == 0x200a) 
      base--;
    start = end = base;
    if (document()->characterAt(start-1).isDigit()) {
      start--;
      while (document()->characterAt(start-1).isDigit())
	start--;
      while (document()->characterAt(end).isDigit())
	end++;
    } else if (document()->characterAt(start-1).isLetter()) {
      start = base-1;
      while (document()->characterAt(start-1).isLetter())
	start--;
      while (document()->characterAt(end).isLetter())
	end++;
    } else {
      return;
    }
    start = refineStart(start, base);
    end = refineEnd(end, base);
  }

  MarkupData *oldmd = data()->markupAt(start, type);
  
  if (oldmd && oldmd->start()==start && oldmd->end()==end) {
    deleteMarkup(oldmd);
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
  update();
}
  
void TextItem::addMarkup(MarkupData::Style t, int start, int end) {
  addMarkup(new MarkupData(start, end, t));
}

void TextItem::addMarkup(MarkupData *d) {
  bool isnew;
  d = data()->mergeMarkup(d, &isnew);
  if (d->style()==MarkupData::FootnoteRef)
    reftexts[d] = d->text();
  if (isnew)
    linkHelper->newMarkup(d);
  text->partialRelayout(d->start(), d->end());
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

static bool approvedMark(QString s) {
  QString marks = "*@#%$&+"; // add more later
  return marks.contains(s);
}

QString TextItem::markedText(MarkupData *md) {
  ASSERT(md);
  TextCursor c = textCursor();
  c.setPosition(md->start());
  c.setPosition(md->end(), TextCursor::KeepAnchor);
  return c.selectedText();
}

bool TextItem::tryExplicitLink() {
  TextCursor m = ResourceMagic::explicitLinkAt(textCursor(), style());
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

bool TextItem::tryFootnote() {
  BlockItem *anc = ancestralBlock();
  if (!anc) {
    qDebug() << "Cannot add footnote without ancestral block";
    return false;
  }

  EntryScene *bs = dynamic_cast<EntryScene*>(anc->baseScene());
  ASSERT(bs);
  int i = bs->findBlock(anc);
  ASSERT(i>=0);
  
  TextCursor c = textCursor();
  MarkupData *oldmd = data()->markupAt(c.position(), MarkupData::FootnoteRef);
  int start=-1;
  int end=-1;
  bool mayDelete = false;
  if (c.hasSelection()) {
    start = c.selectionStart();
    end = c.selectionEnd();
    mayDelete = true;
  } else {
    TextCursor m = c.findBackward(QRegExp("[^-\\w]"));
    QString mrk = m.selectedText();
    start = m.hasSelection() ? m.selectionEnd() : 0;
    m = c.findForward(QRegExp("[^-\\w]"));
    end = m.hasSelection() ? m.selectionStart() : data()->text().size();
    if (start==end && start>0) 
      if (approvedMark(mrk))
	--start; // markup is a single non-word char like "*".
  }

  if (oldmd && oldmd->start()==start && oldmd->end()==end) {
    if (mayDelete) {
      // delete old mark
      BlockItem *bi = ancestralBlock();
      if (bi) 
	bi->refTextChange(oldmd->text(), ""); // remove any footnotes
      deleteMarkup(oldmd);
    } else {
      return false; // should perhaps give focus to the footnote
    }
    return false;
  } else if (start<end) {
    addMarkup(MarkupData::FootnoteRef, start, end);
    MarkupData *md = data()->markupAt(start, end, MarkupData::FootnoteRef);
    ASSERT(md);
    bs->newFootnote(i, markedText(md));
    return true;
  } else {
    return false;
  }
}

bool TextItem::tryToCopy() const {
  if (!cursor.hasSelection())
    return false;
  TextCursor::Range r = cursor.selectedRange();
  QString html = toHtml(r.start(), r.end());
  qDebug() << html;
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
  if (md->hasImage()) {
    return false;
  } else if (md->hasUrls()) {
    return false; // perhaps we should allow URLs, but format specially?
  } else if (md->hasHtml()) {
    QString txt = md->html();
    if (nonewlines)
      txt.replace("\n", " ");
    if (cursor.hasSelection())
      cursor.deleteChar();
    cursor = insertBasicHtml(txt, cursor.position(),
			     md->hasText() ? md->text() : QString());
    cursor.clearSelection();
    return true;   
  } else if (md->hasText()) {
    QString txt = md->text();
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

bool TextItem::shouldResize(QPointF p) const {
  GfxNoteItem *gni = dynamic_cast<GfxNoteItem*>(parent());
  if (!gni)
    return false;
  double tw = gni->data()->textWidth();
  if (tw<=0)
    tw = netBounds().width();
  bool should = p.x()-netBounds().left() > .75*tw;
  return should;
}
 
void TextItem::modeChange(Mode::M m) {
  Qt::CursorShape cs = defaultCursor();
  switch (m) {
  case Mode::Type:
    if (isWritable())
      cs = Qt::IBeamCursor;
    break;
  case Mode::Annotate:
    cs = Qt::CrossCursor;
    break;
  case Mode::MoveResize:
    if (mayMove) {
      if (shouldResize(cursorPos))
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
  setCursor(Cursors::refined(cs));
}

void TextItem::hoverMoveEvent(QGraphicsSceneHoverEvent *e) {
  cursorPos = e->pos(); // cache for the use of modifierChanged
  modeChange(mode()->mode());
  linkHelper->mouseMove(e);
  e->accept();
}

void TextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  cursorPos = e->pos(); // cache for the use of modifierChanged
  modeChange(mode()->mode());
  linkHelper->mouseMove(e);
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
      emit refTextChange(olds, news);
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
  return text->boundingRect();
}

void TextItem::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) {
  if (!text)
    return;
  
  if (clips())
    p->setClipRect(clip_);
  else
    p->setClipRect(boundingRect());

  QList<TransientMarkup> tmm;
  representCursor(tmm);
  representSearchPhrase(tmm);
  text->render(p, tmm);

  if (hasFocus()) {
    QPointF xy = text->locate(cursor.position());
    p->setFont(style().font("text-font"));
    /* This really ought to match style (e.g., italic and x-script) */
    p->setPen(QPen(QColor("red")));
    p->drawText(xy - QPointF(2, 0), "|");
  }
}

void TextItem::setTextWidth(double d, bool relayout) {
  text->setWidth(d);
  if (relayout) {
    prepareGeometryChange();
    text->relayout();
  }
}

TextCursor TextItem::insertBasicHtml(QString html, int pos, QString ref) {
  HtmlParser p(html);
  TextCursor c(cursor);
  c.setPosition(pos);
  if (ref.isNull() || p.text()==ref) {
    c.insertText(p.text());
    foreach (MarkupData *md, p.markups()) 
      addMarkup(md->style(), md->start()+pos, md->end()+pos);
  } else {
    c.insertText(ref);
  }
  c.setPosition(pos, TextCursor::KeepAnchor);
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
  clip_ = r;
  update();
}

void TextItem::unclip() {
  clip_ = QRectF();
  update();
}

void TextItem::setTextCursor(TextCursor const &tc) {
  cursor = tc;
  if (mode()->mode()==Mode::Type)
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
  text->recalculateCharacterWidths();
}

void TextItem::setFont(QFont f) {
 text->setFont(f);
 text->recalculateCharacterWidths();
}

QString TextItem::toHtml(int start, int end) const {
  HtmlBuilder builder(data(), start, end);
  return builder.toHtml();
}  

