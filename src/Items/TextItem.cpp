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
    text = new TextItemDoc(data, this);
  
  mayMark = true;
  mayNote = false;
  mayMove = false;
  lateMarkType = MarkupData::Normal;
  allowParagraphs_ = true;

  boxvis = false;
  
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

  connect(document(), SIGNAL(contentsChange(int, int, int)),
	  this, SLOT(docChange()));
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
  emit textChanged();
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
      TextCursor c(textCursor());
      c.clearSelection();
      setTextCursor(c);
    }
  }

  QGraphicsItem::focusOutEvent(e);
}

void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e) {
  if (e->button()!=Qt::LeftButton)
    ;
  else if (mode()->mode()==Mode::Type)
    ;
  else if (hasFocus())
    clearFocus();
  e->accept();
}

void TextItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << "TI mouse";
  switch (e->button()) {
  case Qt::LeftButton:
    switch (mode()->mode()) {
    case Mode::Type: {
      int pos = text->find(e->pos());
      qDebug() << "TI: mouse" << e->pos() << pos;
      if (pos>=0) {
        TextCursor c = textCursor();
        c.setPosition(pos,
                      e->modifiers() & Qt::ShiftModifier
                      ? TextCursor::KeepAnchor
                      : TextCursor::MoveAnchor);
        setTextCursor(c);
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
    if (mode()->mode()!=Mode::Type && hasFocus())
      clearFocus();
    break;
  case Qt::MiddleButton:
    if (mode()->mode() == Mode::Type) {
      QClipboard *cb = QApplication::clipboard();
      QString txt = cb->text(QClipboard::Selection);
      if (!txt.isEmpty()) {
      	TextCursor c = textCursor();
      	int pos = pointToPos(e->pos());
      	if (pos>=0) 
      	  c.setPosition(pos);
      	c.insertText(txt);
      	setFocus();
      	setTextCursor(c);
      }
    }
  default:
    break;
  }
  e->accept();
}

int TextItem::pointToPos(QPointF p) const {
  return text->find(p);
}

QPointF TextItem::posToPoint(int pos) const {
  return text->locate(pos).center();
}
      

void TextItem::attemptMarkup(QPointF p, MarkupData::Style m) {
  qDebug() << "TextItem::attemptMarkup" << p << m;
  int pos = pointToPos(p);
  qDebug() << "  pos:"<<pos;
  if (pos<0)
    return;
  lateMarkType = m;
  lateMarkStart = pos;
  grabMouse();
}

void TextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *evt) {
  int pos = pointToPos(evt->pos());
  qDebug() << "TextItem::mouseMove" << lateMarkStart << evt->pos() << pos
	   << MarkupData::styleName(lateMarkType);
  if (pos<0)
    return;
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
            MarkupData *md1 = markupAt(e, mdst);
            if (md1)
              md1->setCreated(cre);
          }
          if (mds<s) {
            addMarkup(mdst, mds, s);
            MarkupData *md1 = markupAt(s, mdst);
            if (md1)
              md1->setCreated(cre);
          }
        }
      }
    }
  } else {
    addMarkup(lateMarkType, s, e); // will be auto-merged
  }
  qDebug() << "  -> markings now:";
  foreach (MarkupData *md, data()->children<MarkupData>()) 
    qDebug() << "    " << md->styleName(md->style())
             << md->start() << md->end();
}

void TextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *) {
  ungrabMouse();
  lateMarkType = MarkupData::Normal;
}

bool TextItem::keyPressAsMotion(QKeyEvent *e) {
  switch (e->key()) {
  case Qt::Key_Escape: {
    TextCursor c = textCursor();
    c.clearSelection();
    setTextCursor(c);
    clearFocus();
  } return true;
  case Qt::Key_Return: case Qt::Key_Enter:
    if (!allowParagraphs_) {
      emit futileMovementKey(e->key(), e->modifiers());
      return true;
    } break;
  case Qt::Key_Backspace:
    if (textCursor().atStart() && !textCursor().hasSelection()) {
      emit futileMovementKey(e->key(), e->modifiers());
      return true;
    } break;
  case Qt::Key_Delete:
    if (textCursor().atEnd() && !textCursor().hasSelection()) {
      emit futileMovementKey(e->key(), e->modifiers());
      return true;
    } break;
  case Qt::Key_Left:
    tryMove(TextCursor::Left, e->key(), e->modifiers());
    return true;
  case Qt::Key_Up:
    tryMove(TextCursor::Up, e->key(), e->modifiers());
    return true;
  case Qt::Key_Right:
    tryMove(TextCursor::Right, e->key(), e->modifiers());
    return true;
  case Qt::Key_Down:
    tryMove(TextCursor::Down, e->key(), e->modifiers());
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
  if (c==textCursor())
    emit futileMovementKey(key, mod);
  else
    setTextCursor(c);
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
  case Qt::Key_N:
    tryFootnote();
    return true;
  case Qt::Key_L:
    tryExplicitLink();
    return true;
  case Qt::Key_Period:
    tryScriptStyles(textCursor());
    return true;
  case Qt::Key_Backslash:
    tryTeXCode();
    return true;
  case Qt::Key_2:
    insertBasicHtml(QString::fromUtf8("²"), textCursor().position());
    return true;
  case Qt::Key_3:
    insertBasicHtml(QString::fromUtf8("³"), textCursor().position());
    return true;
  case Qt::Key_4:
    insertBasicHtml(QString::fromUtf8("⁴"), textCursor().position());
    return true;
  case Qt::Key_Space:
    insertBasicHtml(QString::fromUtf8(" "), textCursor().position());
    return true;
  case Qt::Key_Enter: case Qt::Key_Return:
    insertBasicHtml(QString("\n"), textCursor().position());
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
  case Qt::Key_6: // cas Qt::Key_Hat:
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

bool TextItem::tryTeXCode(bool noX) {
  TextCursor c(textCursor());
  if (!c.hasSelection()) {
    TextCursor m = c.findBackward(QRegExp("([^A-Za-z])"));
    int start = m.hasSelection() ? m.selectionEnd() : 0;
    m.setPosition(start);
    m = m.findForward(QRegExp("([^A-Za-z])"));
    int end = m.hasSelection() ? m.selectionStart() : data()->text().size();
    c.setPosition(start);
    c.setPosition(end, TextCursor::KeepAnchor);
  }
  // got a word
  QString key = c.selectedText();
  if (!TeXCodes::contains(key))
    return false;
  if (noX && key.size()==1)
    return false;
  QString val = TeXCodes::map(key);
  c.deleteChar(); // delete the word
  if (document()->characterAt(c.position()-1)=='\\')
    c.deletePreviousChar(); // delete any preceding backslash
  if (val.startsWith("x")) {
    // this is "vec", or "dot", or similar
    if (document()->characterAt(c.position()-1).isSpace())
      c.deletePreviousChar(); // delete previous space
    c.insertText(val.mid(1));
  } else {
    c.insertText(val); // insert the replacement code
  }
  return true;
}

bool TextItem::keyPressAsSpecialEvent(QKeyEvent *e) {
  if (e->key()==Qt::Key_Tab || e->key()==Qt::Key_Backtab) {
    TextCursor tc(textCursor());
    
    TextBlockItem *p = dynamic_cast<TextBlockItem *>(parent());
    if (p) {
      // we are in a text block, so we could fiddle with indentation
      bool hasIndent = p->data()->indented();
      bool hasDedent = p->data()->dedented();
      bool hasShift = e->modifiers() & Qt::ShiftModifier;
      bool hasControl = e->modifiers() & Qt::ControlModifier;
      if (hasControl) {
        p->data()->setDisplayed(!p->data()->displayed());
      } else if (hasShift) {
        if (hasIndent) 
          p->data()->setIndented(false);
        else if (hasDedent)
          p->data()->setIndented(true);
        else
          p->data()->setDedented(true);
      } else if (tc.position()==0) {
	if (hasIndent)
	  return false; // allow Tab to be inserted at start
	else
          p->data()->setIndented(true);
      } else {
	// no control, no shift, not at start
        qDebug() << "Convert to table currently not implemented";
        ASSERT(0);
        /*
	TextItemDoc *doc = document();
	if (doc->blockCount()==1
	    && doc->firstBlock().lineCount()==1
	    && doc->firstBlock().layout()->lineAt(0).naturalTextWidth()
	    < (style().real("page-width")-style().real("margin-left")
	       -style().real("margin-right"))*2.0/3.0) {
	  emit multicellular(tc.position(), data());
	  return true; // do not allow Tab to be inserted
	} else {
	  return false; // allow Tab to be inserted
	}
        */
      }
      p->initializeFormat();
      return true;
    }
  }
  return false;
}

bool TextItem::keyPressAsSpecialChar(QKeyEvent *e) {
  TextCursor c(textCursor());
  QChar charBefore = document()->characterAt(c.position()-1);
  QChar charBefore2 = document()->characterAt(c.position()-2);
  QString charNow = e->text();
  QString digraph = QString(charBefore) + charNow;
  QString trigraph = QString(charBefore2) + digraph;
  if (Digraphs::contains(digraph)) {
    c.deletePreviousChar();
    c.insertText(Digraphs::map(digraph));
    return true;
  } else if (Digraphs::contains(trigraph)) {
    c.deletePreviousChar();
    c.deletePreviousChar();
    c.insertText(Digraphs::map(trigraph));
    return true;
  } else if (Digraphs::contains(charNow)) {
    c.insertText(Digraphs::map(charNow));
    return true;
  } else if (charNow=="\"") {
    if (charBefore.isSpace() || charBefore.isNull()
	|| digraph=="(\"" || digraph=="[\"" || digraph=="{\""
	|| digraph==QString::fromUtf8("‘\"")) 
      c.insertText(QString::fromUtf8("“"));
    else
      c.insertText(QString::fromUtf8("”"));
    return true;
  } else if (digraph==QString::fromUtf8("--")) {
    c.deletePreviousChar();
    if (document()->characterAt(c.position()-1).isDigit()) 
      c.insertText(QString::fromUtf8("‒")); // figure dash
    else 
      c.insertText(QString::fromUtf8("–")); // en dash
    return true;
  } else if (charNow[0].isDigit() && charBefore==QChar('-')
	     && QString(" ([{^_@$/").contains(charBefore2)) {
    c.deletePreviousChar();
    c.insertText(QString::fromUtf8("−")); // replace minus sign
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
      || keyPressAsSpecialEvent(e))
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

bool TextItem::tryScriptStyles(TextCursor c, bool onlyIfBalanced) {
  /* Returns true if we decide to make a superscript or subscript, that is,
     if there is a preceding "^" or "_".
   */
  TextCursor m = c.findBackward(QRegExp("\\^|_"));
  if (!m.hasSelection())
    return false; // no "^" or "_"
  if (m.selectionEnd() == c.position())
    return false; // empty selection

  qDebug() << "tryScriptStyles " << onlyIfBalanced;
  if (onlyIfBalanced) {
    TextCursor scr(m);
    scr.setPosition(c.position(), TextCursor::KeepAnchor);
    if (!balancedBrackets(scr.selectedText()))
      return false;
  }  
  
  QString mrk = m.selectedText();
  m.deleteChar();
  addMarkup(mrk=="^"
	    ? MarkupData::Superscript
	    : MarkupData::Subscript,
	    m.position(), c.position());
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

  MarkupData *oldmd = markupAt(start, type);
  
  if (oldmd && oldmd->start()==start && oldmd->end()==end) {
    deleteMarkup(oldmd);
    if (type==MarkupData::Italic
	&& document()->characterAt(end).unicode()==0x200a) {
      qDebug() << "Italic correction - should eventually go away";
      TextCursor d(c);
      d.setPosition(end);
      d.deleteChar();
    }
  } else if (start<end) {
    addMarkup(type, start, end);
    if (type==MarkupData::Italic) {
      TextCursor d(c);
      d.setPosition(end);
      d.insertText(QString::fromUtf8(" ")); // hair space 0x200a
      qDebug() << "Italic correction - should eventually go away";
    }
  }
}

void TextItem::deleteMarkup(MarkupData *d) {
  int s = d->start();
  int e = d->end();
  data()->deleteMarkup(d);
  text->partialRelayout(s, e);
  update();
}
  
void TextItem::addMarkup(MarkupData::Style t, int start, int end) {
  addMarkup(new MarkupData(start, end, t));
}

void TextItem::addMarkup(MarkupData *d) {
  data()->addMarkup(d);
  text->partialRelayout(d->start(), d->end());
  update();
}

MarkupData *TextItem::markupAt(int pos, MarkupData::Style typ) {
  return markupAt(pos, pos, typ);
}

MarkupData *TextItem::markupAt(int start, int end, MarkupData::Style typ) {
  foreach (MarkupData *md, data()->children<MarkupData>()) 
    if (md->style()==typ && md->end()>=start && md->start()<=end)
      return md;
  return 0;
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
  MarkupData *oldmd = markupAt(start, end, MarkupData::Link);
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
  MarkupData *oldmd = markupAt(c.position(), MarkupData::FootnoteRef);
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
    MarkupData *md = markupAt(start, end, MarkupData::FootnoteRef);
    ASSERT(md);
    bs->newFootnote(i, markedText(md));
    return true;
  } else {
    return false;
  }
}

bool TextItem::tryToPaste() {
  QClipboard *cb = QApplication::clipboard();
  QMimeData const *md = cb->mimeData(QClipboard::Clipboard);
  qDebug() << "TextItem::tryToPaste" << md;
  if (md->hasImage()) {
    return false;
  } else if (md->hasUrls()) {
    return false; // perhaps we should allow URLs, but format specially?
  } else if (md->hasText()) {
    QString txt = md->text();
    TextCursor c = textCursor();
    c.insertText(txt);
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
  e->accept();
}

void TextItem::updateRefText(QString olds, QString news) {
  emit refTextChange(olds, news);
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
  if (boxvis) {
    QPen pen(QColor("#000000"));
    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);
    p->setPen(pen);
    p->drawRect(boundingRect().adjusted(.5, .5, -.5, -.5));
  }
  // render selection box...
  text->render(p);

  if (hasFocus()) {
    QRectF r = text->locate(cursor.position()).adjusted(-10, 0, 10, 0);
    p->setPen(QPen(QColor("red")));
    p->drawText(r, Qt::AlignCenter, "|");
  }
}

void TextItem::setBoxVisible(bool v) {
  boxvis = v;
  update();
}

void TextItem::setTextWidth(double d) {
  text->setWidth(d);
  text->relayout();
  emit widthChanged();
}

void TextItem::insertBasicHtml(QString html, int pos) {
  TextCursor c(document(), pos);
  QRegExp tag("<(.*)>");
  tag.setMinimal(true);
  QList<int> italicStarts;
  QList<int> boldStarts;
  while (!html.isEmpty()) {
    int idx = tag.indexIn(html);
    if (idx>=0) {
      QString cap = tag.cap(1);
      c.insertText(html.left(idx));
      html = html.mid(idx + tag.matchedLength());
      if (cap=="i") 
	italicStarts.append(c.position());
      else if (cap=="b")
	boldStarts.append(c.position());
      else if (cap=="/i" && !italicStarts.isEmpty()) 
	addMarkup(MarkupData::Italic, italicStarts.takeLast(), c.position());
      else if (cap=="/b" && !boldStarts.isEmpty()) 
	addMarkup(MarkupData::Bold, boldStarts.takeLast(), c.position());
    } else {
      c.insertText(html);
      break;
    }
  }
  while (!italicStarts.isEmpty())
    addMarkup(MarkupData::Italic, italicStarts.takeLast(), c.position());
  while (!boldStarts.isEmpty())
    addMarkup(MarkupData::Bold, boldStarts.takeLast(), c.position());
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
  qDebug() << "TI::setClip " << r << boundingRect() << netBounds();
  clip_ = r;
  update();
}

void TextItem::unclip() {
  clip_ = QRectF();
  update();
}

void TextItem::setTextCursor(TextCursor const &tc) {
  cursor = tc;
  text->setSelection(tc);
  setFocus();
  update();
}
