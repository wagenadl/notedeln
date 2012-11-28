// TextMarkings.C

#include "TextMarkings.H"
#include <QTextCursor>
#include <QDebug>
#include "TextItem.H"
#include "Style.H"
#include "ResManager.H"
#include "HoverRegion.H"

TextMarkings::TextMarkings(TextData *data, TextItem *parent):
  QGraphicsObject(parent), data(data) {
  Q_ASSERT(parent);
  doc = parent->document();
  connect(doc, SIGNAL(contentsChange(int, int, int)),
	  SLOT(update(int, int, int)));
  foreach (MarkupData *m, data->markups()) 
    applyMark(insertMark(m));
}

TextMarkings::~TextMarkings() {
}

TextItem *TextMarkings::parent() const {
  return dynamic_cast<TextItem*>(parentItem());
}

void TextMarkings::applyMark(Span const &span) {
  QTextCursor c(doc);
  c.beginEditBlock();
  c.setPosition(span.data->start());
  c.setPosition(span.data->end(), QTextCursor::KeepAnchor);
  QTextCharFormat f(c.charFormat());
  switch (span.data->style()) {
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
  case MarkupData::Link:
    // f.setForeground(parent()->style().color("url-color"));
    break;
  case MarkupData::FootnoteRef:
    f.setForeground(parent()->style().color("customref-color"));
    break;
  default: 
    break;
  }
  c.setCharFormat(f);
  c.endEditBlock();
}  

TextMarkings::Span &TextMarkings::insertMark(MarkupData *m) {
  if (m->style()==MarkupData::Link)
    regions[m] = new HoverRegion(m, parent(), this);
  Span s(m, this);
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ++i) 
    if (s < *i) 
      return *spans.insert(i, s);
  spans.append(s);
  return spans.last();
}

void TextMarkings::newMark(MarkupData::Style type, int start, int end) {
  MarkupData *md = new MarkupData(start, end, type);
  newMark(md);
}

void TextMarkings::newMark(MarkupData *m) {
  data->addMarkup(m);
  applyMark(insertMark(m));
  update(m->start(), 0, 0); // this should fix overlaps if any
}

void TextMarkings::deleteMark(MarkupData *m) {
  if (regions.contains(m)) {
    delete regions[m];
    regions.remove(m);
  }
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ) {
    if ((*i).data==m) {
      (*i).avoidPropagatingStyle(parent(), m->start(), m->end()-m->start());
      i = spans.erase(i);
    } else {
      ++i;
    }
  }
  data->deleteMarkup(m);
}

void TextMarkings::update(int pos, int del, int ins) {
  // First round: update every span, deleting empty spans
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ) {
    if ((*i).update(parent(), pos, del, ins, this)) {
      MarkupData *m = (*i).data;
      if (regions.contains(m)) {
	delete regions[m];
	regions.remove(m);
      }
      data->deleteMarkup(m); // delete empty one
      i = spans.erase(i);
    } else {
      ++i;
    }
  }

  // Second round: combine abutting or overlapping of same kind
  bool changed = false;
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ++i) {
    for (QList<Span>::iterator j=i+1; j!=spans.end(); ) {
      if ((*j).data->start() > (*i).data->end())
	break;
      MarkupData *mi = (*i).data;
      MarkupData *mj = (*j).data;
      if (mergeable(mi, mj)) {
	// merge or subsume!
	mi->merge(mj);
	if (regions.contains(mj)) {
	  delete regions[mj];
	  regions.remove(mj);
	}
	data->deleteMarkup(mj);
	j = spans.erase(j);
	changed = true;
      } else {
	j++;
      }
    }
  }
  
  // Finally, resort if needed
  if (changed) 
    qSort(spans.begin(), spans.end());

  // and update regions
  qDebug() << "TextMarkings::update -> forgetBounds";
  foreach (HoverRegion *hr, regions)
    hr->forgetBounds();
} 

TextMarkings::Span::Span(MarkupData *data, TextMarkings *tm): data(data) {
  if (tm && (data->style()==MarkupData::FootnoteRef
	     || data->style()==MarkupData::Link))
    refText = tm->parent()->markedText(data);
}

bool TextMarkings::Span::operator<(TextMarkings::Span const &other) const {
  return *data < *(other.data);
}

bool TextMarkings::Span::update(TextItem *item,
				int pos, int del, int ins,
				TextMarkings *markings) {
  if (ins>del && data->end()==pos)
    avoidPropagatingStyle(item, pos, ins-del);
  if (ins>del && data->start()==0 && pos==0)
    avoidPropagatingStyle(item, 0, ins-del);
  
  data->update(pos, del, ins);
  if (data->style()==MarkupData::FootnoteRef) {
    QString newRef = item->markedText(data);
    if (newRef != refText) {
      item->updateRefText(refText, newRef);
      refText = newRef;
    }
  } else if (data->style()==MarkupData::Link) {
    QString newRef = item->markedText(data);
    if (newRef != refText) {
      markings->applyMark(*this);
      refText = newRef;
    }
  }

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
  case MarkupData::Link:
    // f.setForeground(item->defaultTextColor());
    break;
  case MarkupData::FootnoteRef:
    f.setForeground(item->defaultTextColor());
    break;
  default:
    break;
  }
  c.setCharFormat(f);
  c.endEditBlock();
}

QRectF TextMarkings::boundingRect() const {
  return QRectF();
}

void TextMarkings::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {
}
