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

void TextMarkings::newMark(TextMarkings::Type type, int start, int end) {
  QTextCursor c(doc);
  c.beginEditBlock();
  c.setPosition(start);
  c.setPosition(end, QTextCursor::KeepAnchor);
  switch (type) {
  case None: {
    QTextCharFormat f;
    c.setCharFormat(f);
  } break;
  case Italic: {
    QTextCharFormat f(c.charFormat());
    f.setFontItalic(true);
    c.setCharFormat(f);
  } break;
  default: {
  } break;
  }
  c.endEditBlock();
  
  Span s(type,start, end);
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); ++i) {
    if (s < *i) {
      spans.insert(i, s);
      return;
    }
  }
  spans.append(s);
}

void TextMarkings::update(int pos, int del, int ins) {
  QList<Span>::iterator next;
  // First round: update every span
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); i=next) {
    next = i;
    ++next;
    if ((*i).update(doc, pos, del, ins))
      spans.erase(i);
  }

  // Second round: combine
  bool changed = false;
  for (QList<Span>::iterator i=spans.begin(); i!=spans.end(); i=next) {
    qDebug() << "iloop";
    next = i;
    ++next;
    QList<Span>::iterator nextj;
    for (QList<Span>::iterator j=next; j!=spans.end(); j=nextj) {
      qDebug() << "jloop";
      nextj = j;
      ++nextj;
      if ((*j).start<=(*i).end) {
	if ((*j).type==(*i).type) {
	 // merge or subsume!
	  if ((*j).end>(*i).end)
	    (*i).end = (*j).end;
	  if (next==j)
	    ++next;
	  spans.erase(j);
	  qDebug() << "erased";
	  changed = true;
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

TextMarkings::Span::Span(TextMarkings::Type t, int s, int e):
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

bool TextMarkings::Span::update(QTextDocument *doc,
				int pos, int del, int ins) {
  // This now mostly works.
  // It does not know how to merge adjoining markings,
  // and it cannot deal well with nested markings.

  if (del>0) {
    if (start>pos) {
      start -= del;
      if (start<pos)
	start = pos;
    }
    if (end>pos) {
      end -= del;
      if (end<pos)
	end = pos;
    }
  }
  if (ins>0) {
    if (end>pos || (end==pos && end==start)) {
      end += ins; }
    else if (end==pos) {
      /* Make sure that insertion after this span doesn't get style of the
	 span.
	 This is not correct for nested spans.
      */
      QTextCursor c(doc);
      c.beginEditBlock();
      c.setPosition(pos);
      c.setPosition(pos+ins, QTextCursor::KeepAnchor);
      QTextCharFormat f = c.charFormat();
      f.setFontItalic(false); // and others
      c.setCharFormat(f);
      c.endEditBlock();
    }
    if (start>=pos)
      start += ins;
  }
  return end==start;
}
