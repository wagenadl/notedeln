// TextItem.C

#include "TextItem.H"
#include "TextData.H"
#include "TextMarkings.H"

#include "Style.H"
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

#include "GfxNoteItem.H" // yick

TextItem::TextItem(TextData *data, Item *parent):
  QGraphicsTextItem(gi(parent)),
  Item(data, this),
  data_(data) {
  mayMark = true;
  allowParagraphs_ = true;

  setPlainText(data_->text());  
  markings_ = new TextMarkings(data_, document(), this);

  initializeFormat();

  connect(document(), SIGNAL(contentsChange(int, int, int)),
	  this, SLOT(docChange()));
}

void TextItem::makeWritable() {
  Item::makeWritable();
  setTextInteractionFlags(Qt::TextEditorInteraction);
  setCursor(QCursor(Qt::IBeamCursor));
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
  QString plainText = toPlainText();
  if (data_->text() == plainText) {
    // trivial change; this happens if markup changes
    return;
  }
  Q_ASSERT(isWritable());
  data_->setText(plainText);
  emit textChanged();
}

void TextItem::focusOutEvent(QFocusEvent *e) {
  QGraphicsTextItem::focusOutEvent(e);
  if (document()->isEmpty()) {
    emit abandoned();
  }
}

void TextItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  QGraphicsTextItem::mousePressEvent(e);
  emit mousePress(e->scenePos(), e->button());
}

void TextItem::keyPressEvent(QKeyEvent *e) {
  bool pass = true;
  switch (e->key()) {
  case Qt::Key_Escape:
    clearFocus();
    pass = false;
    break;
  case Qt::Key_Return: case Qt::Key_Enter:
    if (!allowParagraphs_) {
      emit futileMovementKey(textCursor(), e->key(), e->modifiers());
      pass = false;
    }
    break;
  case Qt::Key_Backspace:
    if (textCursor().atStart() && !textCursor().hasSelection()) {
      emit futileMovementKey(textCursor(), e->key(), e->modifiers());
      pass = false;
    }
    break;
  case Qt::Key_Delete:
    if (textCursor().atEnd() && !textCursor().hasSelection()) {
      emit futileMovementKey(textCursor(), e->key(), e->modifiers());
      pass = false;
    }
    break;
  case Qt::Key_Left: case Qt::Key_Up:
  case Qt::Key_Right: case Qt::Key_Down:
  case Qt::Key_PageUp: case Qt::Key_PageDown: {
    QTextCursor pre = textCursor();
    QGraphicsTextItem::keyPressEvent(e);
    QTextCursor post = textCursor();
    if (pre.position() == post.position()) {
      emit futileMovementKey(pre, e->key(), e->modifiers());
    }
    pass = false;
  } break;
  case Qt::Key_V:
    if (e->modifiers() & Qt::ControlModifier) {
      tryToPaste();
      pass = false;
    }
    break;
  default:
    if (e->text()=="/") {
      pass = !trySimpleStyle("/", MarkupData::Italic);
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
  if (pass)
    QGraphicsTextItem::keyPressEvent(e);
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
  QTextCursor m = document()->find(QRegExp("https?://"), c,
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
    addMarkup(MarkupData::URL, m.selectionStart(), endpos);
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
     (5) we do not have a word character after us
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

void TextItem::addMarkup(MarkupData::Style t, int start, int end) {
  markings_->newMark(t, start, end);
}

void TextItem::addMarkup(MarkupData *d) {
  markings_->newMark(d);
}
 
bool TextItem::tryToPaste() {
  qDebug() << "TextItem::tryToPaste";
  QClipboard *cb = QApplication::clipboard();
  QMimeData const *md = cb->mimeData(QClipboard::Clipboard);
  //  QStringList fmt = md->formats();
  qDebug() << "Clipboard: ";
  //foreach (QString s, fmt)
  //  qDebug() << "  Format: " << s;
  if (md->hasText())
    qDebug() << "  Text: " << md->text();
  if (md->hasHtml())
    qDebug() << "  Html: " << md->html();
  if (md->hasUrls())
    qDebug() << "  Urls: " << md->urls();
  if (md->hasImage())
    qDebug() << "  Image!";
  md = cb->mimeData(QClipboard::Selection);
  //  fmt = md->formats();
  qDebug() << "Selection: ";
  //foreach (QString s, fmt)
  //  qDebug() << "  Format: " << s;
  if (md->hasText())
    qDebug() << "  Text: " << md->text();
  if (md->hasHtml())
    qDebug() << "  Html: " << md->html();
  if (md->hasUrls())
    qDebug() << "  Urls: " << md->urls();
  if (md->hasImage())
    qDebug() << "  Image!";

  return false; // will return true if we decide to import
}  

bool TextItem::allowParagraphs() const {
  return allowParagraphs_;
}

void TextItem::setAllowParagraphs(bool yes) {
  allowParagraphs_ = yes;
}

void TextItem::modifierChange(Qt::KeyboardModifiers) {
  // this will only be called if we are writable
  if (moveModPressed())
    setCursor(Qt::SizeAllCursor);
  else
    setCursor(Qt::IBeamCursor);
}

