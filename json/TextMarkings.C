// TextMarkings.C

#include "TextMarkings.H"
#include <QTextCursor>
#include <QDebug>

TextMarkings::TextMarkings(QTextDocument *doc, QObject *parent):
  QObject(parent), doc(doc) {
  connect(doc, SIGNAL(contentsChange(int, int, int)),
	  SLOT(update(int, int, int)));
}

TextMarkings::~TextMarkings() {
}

void TextMarkings::newMark(MarkupData::Style type, int start, int end) {
  QTextCursor c(doc);
  c.beginEditBlock();
  c.setPosition(start);
  c.setPosition(end, QTextCursor::KeepAnchor);
  QTextCharFormat f(c.charFormat());
  switch (type) {
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
  default: 
    break;
  }
  c.setCharFormat(f);
  c.endEditBlock();
  
  Span s(type, start, end);
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ++i) {
    if (s < *i) {
      spans.insert(i, s);
      return;
    }
  }
  spans.append(s);
}

void TextMarkings::update(int pos, int del, int ins) {
  // First round: update every span
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ) {
    if ((*i).update(doc, pos, del, ins))
      i = spans.erase(i);
    else
      ++i;
  }

  // Second round: combine abutting or overlapping of same kind
  bool changed = false;
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ++i) {
    qDebug() << "iloop";
    for (QList<Span>::iterator j=i+1; j!=spans.end(); ) {
      qDebug() << "jloop";
      if ((*j).start<=(*i).end) {
	if ((*j).type==(*i).type) {
	  // merge or subsume!
	  if ((*j).end>(*i).end)
	    (*i).end = (*j).end;
	  j = spans.erase(j);
	  qDebug() << "erased";
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
  
  qDebug() << "Markings updated";
  foreach (Span const &s, spans) 
    qDebug() << "  Span: " << s.type << ": " << s.start << "-" << s.end;
  
} 

TextMarkings::Span::Span(MarkupData::Style t, int s, int e):
  type(t), start(s), end(e) {
}

bool TextMarkings::Span::operator<(TextMarkings::Span const &other) const {
  if (start<other.start)
    return true;
  else if (start>other.start)
    return false;

  if (end<other.end)
    return true;
  else if (end>other.end)
    return false;

  return type<other.type;
}

bool TextMarkings::Span::cut(int pos, int len) {
  if (start>pos) {
    start -= len;
    if (start<pos)
      start = pos;
  }
  if (end>pos) {
    end -= len;
    if (end<pos)
      end = pos;
  }
  return start==end;
}  

bool TextMarkings::Span::update(QTextDocument *doc,
				int pos, int del, int ins) {
  // This now mostly works.
  // It does not know how to merge adjoining markings,
  // and it cannot deal well with nested markings.

  if (del>ins) 
    return cut(pos, del-ins);
  if (ins>del) 
    insert(doc, pos, ins-del);
  return false;
}

void TextMarkings::Span::insert(QTextDocument *doc,
				int pos, int len) {
  if (end>pos || (end==pos && end==start)) {
    end += len;
  } else if (end==pos) {
    /* Make sure that insertion after this span doesn't get style of the
       span.
    */
    QTextCursor c(doc);
    c.beginEditBlock();
    c.setPosition(pos);
    c.setPosition(pos+len, QTextCursor::KeepAnchor);
    QTextCharFormat f = c.charFormat();
    switch (type) {
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
    default:
      break;
    }
    c.setCharFormat(f);
    c.endEditBlock();
  }
  if (start>=pos)
    start += len;
}
