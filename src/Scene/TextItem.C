// TextItem.C

#include "TextItem.H"
#include "TextData.H"
#include "TextMarkings.H"
#include "ModSnooper.H"
#include "PageScene.H"
#include "Style.H"
#include "ResourceManager.H"
#include "HoverRegion.H"

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
    } else {
      return false;
    }
  }
}

bool TextItem::keyPress(QKeyEvent *e) {
  bool pass = true;
  switch (e->key()) {
  case Qt::Key_Escape:
    clearFocus();
    pass = false;
    break;
  case Qt::Key_Return: case Qt::Key_Enter:
    if (!allowParagraphs_) {
      emit futileMovementKey(e->key(), e->modifiers());
      pass = false;
    }
    break;
  case Qt::Key_Backspace:
    if (textCursor().atStart() && !textCursor().hasSelection()) {
      emit futileMovementKey(e->key(), e->modifiers());
      pass = false;
    }
    break;
  case Qt::Key_Delete:
    if (textCursor().atEnd() && !textCursor().hasSelection()) {
      emit futileMovementKey(e->key(), e->modifiers());
      pass = false;
    }
    break;
  case Qt::Key_Left: case Qt::Key_Up:
  case Qt::Key_Right: case Qt::Key_Down:
  case Qt::Key_PageUp: case Qt::Key_PageDown: {
    QTextCursor pre = textCursor();
    text->internalKeyPressEvent(e);
    QTextCursor post = textCursor();
    if (pre.position() == post.position()) {
      emit futileMovementKey(e->key(), e->modifiers());
    }
    pass = false;
  } break;
  case Qt::Key_V:
    if (e->modifiers() & Qt::ControlModifier) {
      tryToPaste();
      pass = false;
    }
    break;
  case Qt::Key_N:
    if (e->modifiers() & Qt::ControlModifier) {
      tryFootnote();
      pass = false;
    }
    break;
  case Qt::Key_L:
    if (e->modifiers() & Qt::ControlModifier) {
      tryLink();
      pass = false;
    }
    break;
  case Qt::Key_Slash:
    if (e->modifiers() & Qt::ControlModifier) {
      pass = !trySimpleStyle("/", MarkupData::Italic);
      break;
    } // fall through!
  default:
    if (e->text()=="/") {
      pass = true; // !trySimpleStyle("/", MarkupData::Italic);
    } else if (e->text()=="*") {
      pass = !trySimpleStyle("*", MarkupData::Bold);
    } else if (e->text()=="_") {
      pass = !trySimpleStyle("_", MarkupData::Underline);
    } else if (e->text()==".") {
      pass = !tryScriptStyles();
    } else if (e->text()=="]") {
      pass = !tryCustomRef();
      // should probably offer to create a footnote?
    } else if (QString(",; \n").contains(e->text())) {
      tryURL();
    }
    break;
  }
  return !pass;
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

bool TextItem::tryURL() {
  /* Returns true if we decide to mark some text as a hyperlink, that is, if
     (1) there is text beginning with "http://", "https://" or "www." before
         us,
     (2) and that text was not preceded by a word character
     (3) and there are no spaces or ";" or "," between it and us.
     Additionally, if the character before us is ".", it will not be part
     of the URL.
  */
  QTextCursor c = textCursor();
  QTextCursor m = document()->find(QRegExp("(file|https?)://"), c,
					QTextDocument::FindBackward);
  // look for http or https
  if (m.hasSelection()) {
    QTextCursor url(c);
    url.setPosition(m.selectionStart(), QTextCursor::KeepAnchor);
    if (url.selectedText().contains(QRegExp("[,;\\s]")))
      m.clearSelection();
  }
  if (!m.hasSelection()) {
    // alternatively, look for just plain www
    m = document()->find("www.", c, QTextDocument::FindBackward);
    if (m.hasSelection()) {
      QTextCursor url(c);
      url.setPosition(m.selectionStart(), QTextCursor::KeepAnchor);
      if (url.selectedText().contains(QRegExp("[,;\\s]")))
	m.clearSelection();
    }
  }
  if (m.hasSelection()) {
    // gotcha
    int endpos = c.position();
    if (document()->characterAt(endpos-1)==QChar('.'))
      endpos--;
    MarkupData *md = addMarkup(MarkupData::URL, m.selectionStart(), endpos);
    tryLink(md);
    return true;
  } else {
    return false;
  }
}

bool TextItem::tryCustomRef() {
  /* Returns true if we decide to do a custom reference, that is, if
     (1) there is a preceding "["
     (2) there are no space characters between it and us
     (3) there is not a word character after us
  */
   QTextCursor c = textCursor();
   if (charAfterIsLetter(c.position()))
     return false;
   QTextCursor m = document()->find("[", c,
				    QTextDocument::FindBackward);
   if (!m.hasSelection())
     return false;
   int p0 = m.selectionStart();
   m.setPosition(m.selectionEnd());
   m.setPosition(c.position(), QTextCursor::KeepAnchor);
   QString t = m.selectedText();
   if (t.isEmpty() || t.contains(QRegExp("\\s")))
     return false;

   // gotcha
   m.setPosition(p0);
   m.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
   m.deleteChar(); // remove opening "["
   addMarkup(MarkupData::CustomRef, p0, c.position());
   tryFootnote();
   return true;
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
  if (charAfterIsLetter(c.position()))
    return false;
  if (!charBeforeIsLetter(c.position()))
    return false;
  
  QTextCursor m = document()->find(marker, c, QTextDocument::FindBackward);
  if (!m.hasSelection())
    return false; // no slash
  if (charBeforeIsLetter(m.selectionStart()))
    return false;
  if (!charAfterIsLetter(m.selectionEnd()))
    return false;

  m.deleteChar();
  addMarkup(type, m.position(), c.position());
  return true;
}

MarkupData *TextItem::addMarkup(MarkupData::Style t, int start, int end) {
  return markings_->newMark(t, start, end);
}

void TextItem::addMarkup(MarkupData *d) {
  markings_->newMark(d);
}

MarkupData *TextItem::markupAt(int pos, MarkupData::Style typ) {
  foreach (MarkupData *md, data_->children<MarkupData>()) 
    if (md->style()==typ && md->end()>=pos && md->start()<=pos)
      return md;
  return 0;
}

QString TextItem::markedText(MarkupData *md) {
  Q_ASSERT(md);
  QTextCursor c = textCursor();
  c.setPosition(md->start());
  c.setPosition(md->end(), QTextCursor::KeepAnchor);
  return c.selectedText();
}

bool TextItem::tryLink(MarkupData *md) {
  qDebug() << "Try link";
  Q_ASSERT(pageScene());
  int i = pageScene()->findBlock(this);
  Q_ASSERT(i>=0);

  if (md==0)
    md = markupAt(textCursor().position(), MarkupData::URL);

  if (md) {
    QString txt = markedText(md);
    QUrl url(txt.startsWith("www.") ? ("http://"+txt) : txt);
    if (!url.isValid()) {
      qDebug() << "Invalid url:" << url;
      return false;
    }
    Q_ASSERT(data()->book());
    Q_ASSERT(data()->resMgr());
    connect(data()->resMgr(), SIGNAL(finished(QString)),
	    this, SLOT(linkFinished(QString)),
	    Qt::UniqueConnection);
    data()->resMgr()->link(url);
    return true;
  } else {
    return false;
  }
}

void TextItem::linkFinished(QString resName) {
  if (!data()->resMgr()->contains(resName))
    return; // failed
  QString url = data()->resMgr()->url(resName).toString();
  markings_->foundUrl(url);
}
  
bool TextItem::tryFootnote() {
  Q_ASSERT(pageScene());
  int i = pageScene()->findBlock(this);
  Q_ASSERT(i>=0);

  MarkupData *md = markupAt(textCursor().position(), MarkupData::CustomRef);
  if (md) {
    pageScene()->newFootnote(i, markedText(md));
    return true;
  } else {
    qDebug() << "  No customref found";
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
