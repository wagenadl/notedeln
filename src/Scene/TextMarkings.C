// TextMarkings.C

#include "TextMarkings.H"
#include <QTextCursor>
#include <QDebug>
#include "TextItem.H"
#include "Style.H"

TextMarkings::TextMarkings(TextData *data, TextItem *parent):
  QObject(parent), data(data) {
  Q_ASSERT(parent);
  doc = parent->document();
  connect(doc, SIGNAL(contentsChange(int, int, int)),
	  SLOT(update(int, int, int)));
  foreach (MarkupData *m, data->markups()) {
    applyMark(m);
    insertMark(m);
  }    
}

TextMarkings::~TextMarkings() {
}

TextItem *TextMarkings::parent() const {
  return dynamic_cast<TextItem*>(QObject::parent());
}

void TextMarkings::applyMark(MarkupData const *data) {
  QTextCursor c(doc);
  c.beginEditBlock();
  c.setPosition(data->start());
  c.setPosition(data->end(), QTextCursor::KeepAnchor);
  QTextCharFormat f(c.charFormat());
  switch (data->style()) {
  case MarkupData::Normal:
    f = QTextCharFormat();
    break;
  case MarkupData::Italic: 
    f.setFontItalic(true);
    break;
  case MarkupData::Bold: 
    f.setFontWeight(QFont::Bold);
    break;
  case MarkupData::Underline: 
    f.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    break;
  case MarkupData::StrikeThrough: 
    f.setFontStrikeOut(true);
    break;
  case MarkupData::Emphasize:
    f.setTextOutline(QPen(QBrush("yellow"), 5)); // or something like that
    break;
  case MarkupData::Superscript:
    f.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    break;
  case MarkupData::Subscript:
    f.setVerticalAlignment(QTextCharFormat::AlignSubScript);
    break;
  case MarkupData::URL:
    f.setForeground(parent()->style().color("url-color"));
    break;
  case MarkupData::CustomRef:
    f.setForeground(parent()->style().color("customref-color"));
    break;
  default: 
    break;
  }
  c.setCharFormat(f);
  c.endEditBlock();
}  

void TextMarkings::insertMark(MarkupData *m) {
  Span s(m);
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ++i) {
    if (s < *i) {
      spans.insert(i, s);
      return;
    }
  }
  spans.append(s);
}
 
void TextMarkings::newMark(MarkupData::Style type, int start, int end) {
  newMark(new MarkupData(start, end, type));
}

void TextMarkings::newMark(MarkupData *m) {
  data->addMarkup(m);
  applyMark(m);
  insertMark(m);
  update(m->start(), 0, 0); // this should fix overlaps if any
}  

void TextMarkings::update(int pos, int del, int ins) {
  // First round: update every span
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ) {
    if ((*i).update(parent(), pos, del, ins)) {
      data->deleteMarkup((*i).data); // delete empty one
      i = spans.erase(i);
    } else {
      ++i;
    }
  }

  // Second round: combine abutting or overlapping of same kind
  bool changed = false;
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ++i) {
    for (QList<Span>::iterator j=i+1; j!=spans.end(); ) {
      if ((*j).data->start()<=(*i).data->end()) {
	if ((*j).data->style()==(*i).data->style()) {
	  // merge or subsume!
	  (*i).data->merge((*j).data);
	  data->deleteMarkup((*j).data);
	  j = spans.erase(j);
	  changed = true;
	} else {
	  j++;
	}
      } else {
	break;
      }
    }
  }
  
  // Finally, resort if needed
  if (changed) 
    qSort(spans.begin(), spans.end());
} 

TextMarkings::Span::Span(MarkupData *data): data(data) {
}

bool TextMarkings::Span::operator<(TextMarkings::Span const &other) const {
  return *data < *(other.data);
}

bool TextMarkings::Span::update(TextItem *item,
				int pos, int del, int ins) {
  if (ins>del && data->end()==pos)
    avoidPropagatingStyle(item, pos, ins);
  
  data->update(pos, del, ins);

  return data->start() == data->end();
}

void TextMarkings::Span::avoidPropagatingStyle(TextItem *item,
					       int pos, int len) {
  QTextCursor c(item->document());
  c.beginEditBlock();
  c.setPosition(pos);
  c.setPosition(pos+len, QTextCursor::KeepAnchor);
  QTextCharFormat f = c.charFormat();
  switch (data->style()) {
  case MarkupData::Italic:
    f.setFontItalic(false);
    break;
  case MarkupData::Bold:
    f.setFontWeight(QFont::Normal);
    break;
  case MarkupData::Underline:
    f.setUnderlineStyle(QTextCharFormat::NoUnderline);
    break;
  case MarkupData::StrikeThrough:
    f.setFontStrikeOut(false);
    break;
  case MarkupData::Emphasize:
    f.setTextOutline(QPen(Qt::NoPen));
    break;
  case MarkupData::Superscript: case MarkupData::Subscript:
    f.setVerticalAlignment(QTextCharFormat::AlignNormal);
    break;
  case MarkupData::URL: case MarkupData::CustomRef:
    f.setForeground(item->defaultTextColor());
    break;
  default:
    break;
  }
  c.setCharFormat(f);
  c.endEditBlock();
}

