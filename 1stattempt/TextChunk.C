// TextChunk.C

#include "TextChunk.H"

TextChunk::TextChunk(QDomElement elt, double y0, QObject *parent):
  PageChunk(elt, y0, parent) {
  if (xml.hasAttribute("type")) 
    Q_ASSERT_X(xml.attribute("type") == "text");
  else
    xml.setAttribute("type", "text");
}

TextChunk::~TextChunk() {
}

QString TextChunk::text() const {
  return xml.attribute("text");
}

void TextChunk::setText(QString t) {
  xml.setAttribute("text", t);
  xml.setAttribute("modified", QDateTime::currentDateTime().toString());
  emit changed();
}

double TextChunk::h() const {
  /* Actually, I should implement TextChunk as a GraphicsTextItem */
  
  Style const *s = Style::style();
  QRect r0(0, 0, s.textWidth(), 100*72);
  QFontMetrics fm(s->font());
  QRect r = fm.boundingRect(r0,
			    Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
			    text());
  return r.height();
}
