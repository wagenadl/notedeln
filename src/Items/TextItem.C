// TextItem.C

#include "TextItem.H"
#include "TextData.H"
#include "TextMarkings.H"
#include "ModSnooper.H"
#include "PageScene.H"
#include "Style.H"
#include "ResManager.H"
#include "HoverRegion.H"
#include "BlockItem.H"
#include "ResourceMagic.H"

#include <QFont>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QKeyEvent>
#include <QDebug>
#include <QTextBlock>
#include <QTextLayout>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

#include "LateNoteItem.H" 
#include "LateNoteData.H" 

TextItem::TextItem(TextData *data, Item *parent):
  Item(data, parent), data_(data) {
  text = new TextItemText(this);
  foreach (LateNoteData *lnd, data->children<LateNoteData>()) 
    create(lnd, this);

  mayMark = true;
  mayNote = false;
  mayMove = false;
  lateMarkType = MarkupData::Normal;
  allowParagraphs_ = true;

  text->setPlainText(data_->text());  
  markings_ = new TextMarkings(data_, this);

  initializeFormat();

  connect(document(), SIGNAL(contentsChange(int, int, int)),
	  this, SLOT(docChange()));
}

TextData *TextItem::data() {
  return data_;
}

bool TextItem::allowNotes() const {
  return mayNote;
}

void TextItem::setAllowNotes(bool y) {
  mayNote = y;
}

void TextItem::makeWritable() {
  Item::makeWritable();
  text->setTextInteractionFlags(Qt::TextEditorInteraction);
  text->setCursor(QCursor(Qt::IBeamCursor));
  setFlag(ItemIsFocusable);
  setFocusProxy(text);
}

void TextItem::setAllowMoves() {
  mayMove = true;
  setAcceptHoverEvents(true);
  text->  setAcceptHoverEvents(true);
  acceptModifierChanges();  
}

TextItem::~TextItem() {
}

void TextItem::initializeFormat() {
  setFont(QFont(style().string("text-font-family"),
		style().real("text-font-size")));
  setDefaultTextColor(style().color("text-color"));
}

void TextItem::docChange() {
  QString plainText = text->toPlainText();
  if (data_->text() == plainText) {
    // trivial change; this happens if markup changes
    return;
  }
  Q_ASSERT(isWritable());
  data_->setText(plainText);
  emit textChanged();
}

bool TextItem::focusIn(QFocusEvent *) {
  return false;
}

bool TextItem::focusOut(QFocusEvent *) {
  if (document()->isEmpty()) {
    emit abandoned();
  }
  return false;
}

bool TextItem::mousePress(QGraphicsSceneMouseEvent *e) {
  if (isWritable()) {
    if (moveModPressed() && mayMove) {
      bool resize = shouldResize(e->pos());
      GfxNoteItem *gni = dynamic_cast<GfxNoteItem*>(itemParent());
      if (gni)
	gni->childMousePress(e->scenePos(), e->button(), resize);
      e->accept();
      return true;
    } else {
      return false;
    }
  } else {
    if (allowNotes()
	&& modSnooper()->keyboardModifiers()==0
	&& e->button()==Qt::LeftButton) {
      e->accept();
      createNote(e->pos(), true);
      return true;
    } else if (e->button()==Qt::LeftButton
	       && (modSnooper()->keyboardModifiers()
		   & ((Qt::ShiftModifier | Qt::ControlModifier)))) {
      e->accept();
      attemptMarkup(e->pos(), modSnooper()->keyboardModifiers());
      return true;
    } else {
      return false;
    }
  }
}

int TextItem::pointToPos(QPointF p) const {
  p = text->mapFromParent(p);
  QTextDocument *doc = text->document();
  for (QTextBlock b = doc->begin(); b!=doc->end(); b=b.next()) {
    QTextLayout *lay = b.layout();
    if (lay->boundingRect().contains(p)) {
      p -= lay->position();
      int nLines = lay->lineCount();
      for (int i=0; i<nLines; i++) {
	QTextLine line = lay->lineAt(i); // yes, this returns the i-th line
	if (line.rect().contains(p)) 
	  return line.xToCursor(p.x());
      }
      qDebug() << "TextItem: point in block but not in a line!?";
      return -1;
    }
  }
  return -1;
}
      

void TextItem::attemptMarkup(QPointF p, Qt::KeyboardModifiers m) {
  qDebug() << "TextItem::attemptMarkup" << p << m;
  int pos = pointToPos(p);
  qDebug() << "  pos:"<<pos;
  if (pos<0)
    return;

  bool shift = m & Qt::ShiftModifier;
  bool ctrl = m & Qt::ControlModifier;
  if (shift && ctrl)
    lateMarkType = MarkupData::Normal;
  else if (shift) 
    lateMarkType = MarkupData::Emphasize;
  else if (ctrl)
    lateMarkType = MarkupData::StrikeThrough;
  else
    return;

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
    foreach (MarkupData *md, data_->children<MarkupData>()) {
      if (md->style()==MarkupData::Emphasize
	  || md->style()==MarkupData::StrikeThrough) {
        int mds = md->start();
        int mde = md->end();
	if (mds<e && mde>s) {
          MarkupData::Style mdst = md->style();
          markings_->deleteMark(md);
          if (mde>e) 
            markings_->newMark(mdst, e, mde);
          if (mds<s) 
            markings_->newMark(mdst, mds, s);
        }
      }
    }
  } else {
    addMarkup(lateMarkType, s, e); // will be auto-merged
  }
  qDebug() << "  -> markings now:";
  foreach (MarkupData *md, data_->children<MarkupData>()) 
    qDebug() << "    " << md->styleName(md->style())
             << md->start() << md->end();
}

void TextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *) {
  ungrabMouse();
  lateMarkType = MarkupData::Normal;
}

bool TextItem::keyPressAsMotion(QKeyEvent *e) {
  switch (e->key()) {
  case Qt::Key_Escape:
    clearFocus();
    return true;
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
  case Qt::Key_Left: case Qt::Key_Up:
  case Qt::Key_Right: case Qt::Key_Down:
  case Qt::Key_PageUp: case Qt::Key_PageDown: {
    QTextCursor pre = textCursor();
    text->internalKeyPressEvent(e);
    QTextCursor post = textCursor();
    if (pre.position() == post.position())
      emit futileMovementKey(e->key(), e->modifiers());
    return true;
  } break;
  }
  return false;
}

bool TextItem::keyPressWithControl(QKeyEvent *e) {
  if (!(e->modifiers() & Qt::ControlModifier))
    return false;
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
  case Qt::Key_Slash:
    toggleSimpleStyle(MarkupData::Italic);
    return true;
  case Qt::Key_8:
    toggleSimpleStyle(MarkupData::Bold);
    return true;
  case Qt::Key_Minus:
    toggleSimpleStyle(MarkupData::Underline);
    return true;
  default:
    return false;
  }
}

bool TextItem::keyPressAsSpecialEvent(QKeyEvent *e) {
  if (e->text()=="*") 
    return trySimpleStyle("*", MarkupData::Bold);
  else if (e->text()=="_") 
    return trySimpleStyle("_", MarkupData::Underline);
  else if (e->text()==".") 
    return tryScriptStyles();
  else if (QString(",; \n").contains(e->text())) 
    return tryAutoLink() && false; // never gobble these keys
  else 
    return false;
}
  
bool TextItem::keyPress(QKeyEvent *e) {
  bool take = keyPressAsMotion(e);
  if (!take)
    take = keyPressWithControl(e);
  if (!take)
    take = keyPressAsSpecialEvent(e);
  return take;
}

bool TextItem::charBeforeIsLetter(int pos) const {
  return document()->characterAt(pos-1).isLetter();
  // also returns false at start of doc
}

bool TextItem::charAfterIsLetter(int pos) const {
  return document()->characterAt(pos).isLetter();
  // also returns false at end of doc
}

static bool containsOnlyDigits(QString s) {
  int N = s.length();
  for (int i=0; i<N; i++)
    if (!s[i].isDigit())
      return false;
  return true;
}

static bool containsOnlyLetters(QString s) {
  int N = s.length();
  for (int i=0; i<N; i++)
    if (!s[i].isLetter())
      return false;
  return true;
}

bool TextItem::tryScriptStyles() {
  /* Returns true if we decide to make a superscript or subscript, that is,
     if:
     (1) there is a preceding "^" or "_"
     (2) and either:
         (a) there are only digits between that mark and us
	 (b) there is a minus sign followed by only digits between that
	     mark and us
	 (c) the mark is followed by a "{" and we have a "}" before us
   */
  QTextCursor c = textCursor();
  QTextCursor m = document()->find(QRegExp("\\^|_"),
				   c, QTextDocument::FindBackward);
  if (!m.hasSelection())
    return false; // no "^" or "_"
  QString mrk = m.selectedText();
  int p0 = m.selectionStart();
  m.setPosition(m.selectionEnd());
  m.setPosition(c.position(), QTextCursor::KeepAnchor);
  QString t = m.selectedText();
  if (t.isEmpty())
    return false;

  if (containsOnlyDigits(t) ||
      (t.startsWith("-") && containsOnlyDigits(t.mid(1))) ||
      containsOnlyLetters(t) ||
      (t.startsWith("{") && t.endsWith("}"))) {
    m.setPosition(p0);
    m.movePosition(QTextCursor::Right,
		   QTextCursor::KeepAnchor,
		   (t.startsWith("{") ? 2 : 1));
    m.deleteChar();
    if (t.endsWith("}"))
      c.deletePreviousChar();
    addMarkup(mrk=="^"
	      ? MarkupData::Superscript
	      : MarkupData::Subscript,
	      p0, c.position());
    return true;
  } else {
    return false;
  }
}

bool TextItem::tryAutoLink() {
  /* Returns true if we decide to mark some text as a hyperlink, that is, if
     (1) there is text beginning with "http://", "https://" or "www." before
         us,
     (2) and that text was not preceded by a word character
     (3) and there are no spaces or ";" or "," between it and us.
     Additionally, if the character before us is ".", it will not be part
     of the URL.
  */
  QTextCursor m = ResourceMagic::autoLinkAt(textCursor(), style());
  if (!m.hasSelection())
    return false;
  
  // gotcha
  int start = m.selectionStart();
  int end = m.selectionEnd();
  MarkupData *oldmd = markupAt(start, end, MarkupData::Link);
  if (oldmd) {
    if (oldmd->start()==start && oldmd->end()==end)
      return false; // preexisting, no significant change
    else
      markings_->deleteMark(oldmd);
  }
  addMarkup(MarkupData::Link, start, end);
  return true;
}

void TextItem::toggleSimpleStyle(MarkupData::Style type) {
  QTextCursor c = textCursor();
  MarkupData *oldmd = markupAt(c.position(), type);
  int start=-1;
  int end=-1;
  if (c.hasSelection()) {
    start = c.selectionStart();
    end = c.selectionEnd();
  } else {
    QTextCursor m = document()->find(QRegExp("\\W"), c,
				     QTextDocument::FindBackward);
    start = m.hasSelection() ? m.selectionEnd() : 0;
    m = document()->find(QRegExp("\\W"), c);
    end = m.hasSelection() ? m.selectionStart() : data_->text().size();
  }
  
  if (oldmd && oldmd->start()==start && oldmd->end()==end) 
    markings_->deleteMark(oldmd);
  else if (start<end) 
    addMarkup(type, start, end);
}
  

bool TextItem::trySimpleStyle(QString marker,
			      MarkupData::Style type) {
  /* Returns true if we decide to do italicize/bold/underline, that is, if
     all of the following are true:
     (1) there is a preceding "/" or "*" or "_"
     (2) that mark was not preceded by a word character
     (3) that mark was followed by a word character
     (4) we have a word character before us
     (5) we do not have a word character after us.
     Note: I am not yet able to avoid nasty italicization in
     "/home/wagenaar/foo.bar" or "http://www.site.com/somewhere.html".
  */

  QTextCursor c = textCursor();
  if (c.hasSelection()) 
    return false;
  if (charAfterIsLetter(c.position()))
    return false;
  if (!charBeforeIsLetter(c.position()))
    return false;
  
  QTextCursor m = document()->find(marker, c, QTextDocument::FindBackward);
  if (!m.hasSelection())
    return false; // no marker
  if (charBeforeIsLetter(m.selectionStart()))
    return false;
  if (!charAfterIsLetter(m.selectionEnd()))
    return false;

  m.deleteChar();
  addMarkup(type, m.position(), c.position());
  return true;
}

void TextItem::addMarkup(MarkupData::Style t, int start, int end) {
  markings_->newMark(t, start, end);
}

void TextItem::addMarkup(MarkupData *d) {
  markings_->newMark(d);
}

MarkupData *TextItem::markupAt(int pos, MarkupData::Style typ) {
  return markupAt(pos, pos, typ);
}

MarkupData *TextItem::markupAt(int start, int end, MarkupData::Style typ) {
  foreach (MarkupData *md, data_->children<MarkupData>()) 
    if (md->style()==typ && md->end()>=start && md->start()<=end)
      return md;
  return 0;
}

static bool approvedMark(QString s) {
  QString marks = "*@#%$&+"; // add more later
  return marks.contains(s);
}

QString TextItem::markedText(MarkupData *md) {
  Q_ASSERT(md);
  QTextCursor c = textCursor();
  c.setPosition(md->start());
  c.setPosition(md->end(), QTextCursor::KeepAnchor);
  return c.selectedText();
}

bool TextItem::tryExplicitLink() {
  QTextCursor m = ResourceMagic::explicitLinkAt(textCursor(), style());
  if (!m.hasSelection())
    return false;
  int start = m.selectionStart();
  int end = m.selectionEnd();
  MarkupData *oldmd = markupAt(start, end, MarkupData::Link);
  if (oldmd && oldmd->start()==start && oldmd->end()==end) {
    // undo link mark
    markings_->deleteMark(oldmd);
    return false;
  } else if (end>start) {
    addMarkup(MarkupData::Link, start, end);
    return true;
  } else {
    return false;
  }
}

bool TextItem::tryFootnote() {
  Q_ASSERT(pageScene());
  int i = pageScene()->findBlock(this);
  Q_ASSERT(i>=0);
  
  QTextCursor c = textCursor();
  MarkupData *oldmd = markupAt(c.position(), MarkupData::FootnoteRef);
  int start=-1;
  int end=-1;
  bool mayDelete = false;
  if (c.hasSelection()) {
    start = c.selectionStart();
    end = c.selectionEnd();
    mayDelete = true;
  } else {
    QTextCursor m = document()->find(QRegExp("[^-\\w]"), c,
				     QTextDocument::FindBackward);
    QString mrk = m.selectedText();
    start = m.hasSelection() ? m.selectionEnd() : 0;
    m = document()->find(QRegExp("[^-\\w]"), c);
    end = m.hasSelection() ? m.selectionStart() : data_->text().size();
    if (start==end && start>0) 
      if (approvedMark(mrk))
	--start; // markup is a single non-word char like "*".
  }
  if (oldmd && oldmd->start()==start && oldmd->end()==end) {
    if (mayDelete) {
      // delete old mark
      BlockItem *bi = BlockItem::ancestralBlock(this);
      if (bi) 
	bi->refTextChange(oldmd->text(), ""); // remove any footnotes
      markings_->deleteMark(oldmd);
    } else {
      return false; // should perhaps give focus to the footnote
    }
    return false;
  } else if (start<end) {
    addMarkup(MarkupData::FootnoteRef, start, end);
    MarkupData *md = markupAt(start, end, MarkupData::FootnoteRef);
    Q_ASSERT(md);
    pageScene()->newFootnote(i, markedText(md));
    return true;
  } else {
    return false;
  }
}

bool TextItem::tryToPaste() {
  QClipboard *cb = QApplication::clipboard();
  QMimeData const *md = cb->mimeData(QClipboard::Clipboard);
  if (md->hasImage()) {
    return false;
  } else if (md->hasUrls()) {
    return false; // perhaps we should allow URLs, but format specially?
  } else if (md->hasText()) {
    QString txt = md->text();
    QTextCursor c = textCursor();
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
  GfxNoteItem *gni = dynamic_cast<GfxNoteItem*>(itemParent());
  if (!gni)
    return false;
  double tw = gni->data()->textWidth();
  if (tw==0)
    tw = boundingRect().width();
  bool should = p.x()-boundingRect().left() > .75*tw;
  return should;
}
 
void TextItem::modifierChange(Qt::KeyboardModifiers) {
  Qt::CursorShape cs = defaultCursor();
  if (isWritable())
    cs = Qt::IBeamCursor;
  if (moveModPressed() && mayMove) {
    if (shouldResize(cursorPos))
      cs = Qt::SplitHCursor;
    else
      cs = Qt::SizeAllCursor;
  }
  text->setCursor(cs);
}

void TextItem::hoverMove(QGraphicsSceneHoverEvent *e) {
  cursorPos = e->pos(); // cache for the use of modifierChanged
  modifierChange(Qt::KeyboardModifiers());
  e->accept();
}

void TextItem::updateRefText(QString olds, QString news) {
  emit refTextChange(olds, news);
}

QRectF TextItem::boundingRect() const {
  return text->boundingRect();
}
 
void TextItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {
}

void TextItem::setBoxVisible(bool v) {
  text->setBoxVisible(v);
}

void TextItem::setTextWidth(double d) {
  text->setTextWidth(d);
  foreach (QGraphicsItem *i, childItems()) {
    HoverRegion *hr = dynamic_cast<HoverRegion *>(i);
    if (hr)
      hr->forgetBounds();
  }
}

void TextItem::insertBasicHtml(QString html, int pos) {
  QTextCursor c(document());
  c.setPosition(pos);
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
