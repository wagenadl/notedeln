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

  setPlainText(data_->text()); // eventually, we'll need to deal with markup
  
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
  data_->setText(toPlainText());
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
      pass = false;
    }
    break;
  default:
    if (e->text()=="/") {
      // Let's find backwards to potentially italicize
      QTextCursor c = textCursor();
      QTextCursor m = document()->find(QRegExp("(?!\\w)/.*\\w(?!\\w)"),
				       c, QTextDocument::FindBackward);
      if (m.hasSelection()) {
	QTextCursor m1 = document()->find(QRegExp("\\W"), c);
	if (m.selectionEnd()>=c.position()
	    && ((m1.hasSelection() && m1.selectionStart()==c.position())
		|| c.atEnd())) {
	  m.beginEditBlock();
	  int p0 = m.selectionStart();
	  m.setPosition(p0);
	  m.deleteChar();
	  int p1 = c.position();
	  m.endEditBlock();
	  markings_->newMark(TextMarkings::Italic, p0, p1);
	  pass = false;
	}
      }
    }
    break;
  }
  if (pass)
    QGraphicsTextItem::keyPressEvent(e);
}

