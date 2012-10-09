// TextBlockTextItem.C

#include "TextBlockTextItem.H"
#include "TextBlockItem.H"
#include "TextBlockData.H"
#include "Style.H"
#include <QFont>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlockFormat>
#include <QKeyEvent>
#include <QDebug>
#include <QTextBlock>
#include <QTextLayout>

TextBlockTextItem::TextBlockTextItem(TextBlockData *data, TextBlockItem *parent):
  QGraphicsTextItem(parent),
  data_(data) {

  if (data_->editable())
    setTextInteractionFlags(Qt::TextEditorInteraction);

  setPlainText(data_->text()); // eventually, we'll need to deal with markup
  
  connect(document(), SIGNAL(contentsChange(int, int, int)),
	  this, SLOT(docChange()));

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
  switch (e->key()) {
  case Qt::Key_Escape:
    clearFocus();
    break;
  case Qt::Key_Return: case Qt::Key_Enter:
    QGraphicsTextItem::keyPressEvent(e);
    emit newParagraph();
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
  } break;
  default:
    QGraphicsTextItem::keyPressEvent(e);
    break;
  }
}
