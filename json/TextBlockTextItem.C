// TextBlockTextItem.C

#include "TextBlockTextItem.H"
#include "TextBlockItem.H"
#include "TextBlockData.H"
#include "Style.H"
#include <QFont>
#include <QTextDocument>

TextBlockTextItem::TextBlockTextItem(TextBlockData *data, TextBlockItem *parent):
  QGraphicsTextItem(parent),
  data_(data) {
  Style const &style(Style::defaultStyle());
  setTextWidth(style["text-width"].toDouble());
  setFont(QFont(style["text-font-family"].toString(),
		style["text-font-size"].toDouble()));
  setDefaultTextColor(QColor(style["text-color"].toString()));
  if (data_->editable())
    setTextInteractionFlags(Qt::TextEditorInteraction);

  setPlainText(data_->text()); // eventually, we'll need to deal with markup
  
  connect(document(), SIGNAL(contentsChange(int, int, int)),
	  this, SLOT(docChange()));
}

TextBlockTextItem::~TextBlockTextItem() {
}

void TextBlockTextItem::docChange() {
  data_->setText(toPlainText());
  emit textChanged();
}
