// TextBlockTextItem.C

#include "TextBlockTextItem.H"
#include "TextBlockItem.H"
#include "TextBlockData.H"
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

TextBlockTextItem::TextBlockTextItem(TextBlockData *data, TextBlockItem *parent):
  QGraphicsTextItem(parent),
  data_(data) {

  if (data_->editable())
    setTextInteractionFlags(Qt::TextEditorInteraction);

  setPlainText(data_->text()->text()); // eventually, we'll need to deal with markup
  
  connect(document(), SIGNAL(contentsChange(int, int, int)),
	  this, SLOT(docChange()));

  markings_ = new TextMarkings(document(), this);

  initializeFormat();
}

TextBlockTextItem::~TextBlockTextItem() {
}

void TextBlockTextItem::initializeFormat() {
  Style const &style(Style::defaultStyle());
  setTextWidth(style["paragraph-width"].toDouble());
  setFont(QFont(style["text-font-family"].toString(),
		style["text-font-size"].toDouble()));
  setDefaultTextColor(QColor(style["text-color"].toString()));

  QTextCursor tc(document());
  QTextBlockFormat fmt = tc.blockFormat();
  fmt.setLineHeight(style["paragraph-line-spacing"].toDouble()*100,
		    QTextBlockFormat::ProportionalHeight);
  fmt.setTextIndent(style["paragraph-indent"].toDouble());
  fmt.setTopMargin(style["paragraph-top-margin"].toDouble());
  fmt.setBottomMargin(style["paragraph-bottom-margin"].toDouble());
  tc.setBlockFormat(fmt);
}

void TextBlockTextItem::docChange() {
  data_->text()->setText(toPlainText());
  emit textChanged();
}

void TextBlockTextItem::focusOutEvent(QFocusEvent *e) {
  QGraphicsTextItem::focusOutEvent(e);
  if (document()->isEmpty()) {
    qDebug() << "Abandoned.";
    emit abandoned();
  }
}

void TextBlockTextItem::keyPressEvent(QKeyEvent *e) {
  bool pass = true;
  switch (e->key()) {
  case Qt::Key_Escape:
    clearFocus();
    pass = false;
    break;
  case Qt::Key_Return: case Qt::Key_Enter:
    QGraphicsTextItem::keyPressEvent(e);
    emit newParagraph();
    pass = false;
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
    }
    break;
  }
  if (pass)
    QGraphicsTextItem::keyPressEvent(e);
}

bool TextBlockTextItem::charBeforeIsLetter(int pos) const {
  return document()->characterAt(pos-1).isLetter();
  // also returns false at start of doc
}

bool TextBlockTextItem::charAfterIsLetter(int pos) const {
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

bool TextBlockTextItem::tryScriptStyles() {
  /* Returns true if we decide to make a superscript or subscript, that is,
     if:
     (1) there is a preceding "^" or "_"
     (2) either:
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
    markings_->newMark(mrk=="^"
		       ? MarkupData::Superscript
		       : MarkupData::Subscript,
		       p0, c.position());
    return true;
  } else {
    return false;
  }
}

bool TextBlockTextItem::trySimpleStyle(QString marker,
				       MarkupData::Style type) {
  /* Returns true if we decide to do italicize/bold/underline, that is, if:
     (1) there is a preceding "/"/"*"/"_"
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
  markings_->newMark(type, m.position(), c.position());
  return true;
}
  
bool TextBlockTextItem::tryToPaste() {
  QClipboard *cb = QApplication::clipboard();
  QMimeData const *md = cb->mimeData(QClipboard::Clipboard);
  QStringList fmt = md->formats();
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
  fmt = md->formats();
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
