// TextSplitter.cpp

#include "TextSplitter.h"

TextSplitter::TextSplitter(QString text,  QVector<double> const &charwidths):
  text(text), charwidths(charwidths) {
  bits_ << makeBit(0, text.size(), Type::Any);
  splitAtNewlines(bits_.begin());
}

TextSplitter::Bit TextSplitter::makeBit(int start, int length,
					TextSplitter::Type type) {
  Bit b;
  b.text = text.mid(start, length);
  b.start = start;
  b.length = length;
  b.width = 0;
  b.type = type;
  for (int off=0; off<length; off++)
    b.width += charwidths[start+off];
  return b;
}

void TextSplitter::splitAtNewlines(TextSplitter::Iter it) {
  // Breaks into as many chunks as needed. Each chunk contains either
  // a single newline or some text without newlines.
  Bit &b(*it);
  int off = b.text.indexOf("\n");
  if (off>=0) {
    if (off>0) {
      // create a bit with text before the newline
      bits_.insert(it, makeBit(b.start, off, Type::Paragraph));
    }
    if (off+1 == b.length) {
      // replace old bit with just the newline
      b.text = "\n";
      b.width = 0;
      b.length = 1;
      b.start += off;
      b.type = Type::Newline;
    } else {
      // insert a bit with just the newline
      bits_.insert(it, makeBit(b.start + off, 1, Type::Newline));
      // replace old bit with rest
      b = makeBit(b.start + off + 1, b.length - off - 1, Type::Any);
      // and iterate
      splitAtNewlines(it);
      // (Let's hope compiler optimizes tail-end-recursion or whatever
      // that is called.)
    }
  } else {
    b.type = Type::Paragraph;
  }
}
    
int TextSplitter::splitAtSpace(TextSplitter::Iter it) {
  // Breaks a chunk at the first white space/non white space boundary.
  // Returns number of chunks inserted.
  // If successful, the chunk at IT will be replaced with the last subchunk;
  // the other subchunk will be inserted in the list before it.
  Bit &b(*it);
  if (b.type!=Type::Paragraph)
    return 0;
  int off = b.text.indexOf(' ');
  if (off<0) {
    // no space at all
    if (b.type==Type::Paragraph)
      b.type = Type::Word;
    return 0;
  }
  int n = 0;
  if (off>0) {
    // does not start with space, but contains space
    // insert first word
    bits_.insert(it, makeBit(b.start, off, Type::Word));
    ++n;
  }
  // off points to first space
  // is there any nonspace after that?
  int nof = off;
  while (++nof<b.length)
    if (b.text[nof]!=' ')
      break;
  // now nof points to nonspace or end of text
  if (nof<b.length) {
    // insert space section
    bits_.insert(it, makeBit(b.start + off, nof - off, Type::Space));
    ++n;
    // replace final part
    b = makeBit(b.start + nof, b.length - nof, Type::Paragraph);
  } else {
    // replace with space section
    if (off>0)
      b = makeBit(b.start + off, b.length - off, Type::Space);
    else
      b.type = Type::Space;
  }
  return n;
}

bool TextSplitter::isHyphenSlash(QChar c) {
  static QString chrs("-/–—");
  return chrs.contains(c);
}

int TextSplitter::indexOfHS(QString const &str, int from) {
  int len = str.size();
  for (int idx=from; idx<len; ++idx)
    if (isHyphenSlash(str[idx]))
      return idx;
  return -1;
}

int TextSplitter::splitAtHyphenSlash(TextSplitter::Iter it) {
  Bit &b(*it);
  if (b.type!=Type::Word)
    return 0;
  int off = indexOfHS(b.text);
  if (off==0) {
    // must skip past first group
    while (++off < b.length)
      if (!isHyphenSlash(b.text[off]))
	break;
    // now points to nonhyphenslash or end
    off = indexOfHS(b.text, off);
  }
  if (off<0) {
    // no hyphen or slash at all
    b.type = Type::Atom;
    return 0;
  }
  // now, off will be positive; there will be a positive number of
  // characters before the offset.
  bits_.insert(it, makeBit(b.start, off, Type::Atom));
  int n = 1;
  int nof = off;
  while (++nof < b.length)
    if (!isHyphenSlash(b.text[nof]))
      break;
  if (nof<b.length) {
    // insert bit with hyphens and slashes
    bits_.insert(it, makeBit(b.start + off, nof - off, Type::Hyphens));
    n++;
    b = makeBit(b.start + nof, b.length - nof, Type::Word);
  } else {
    if (off>0)
      b = makeBit(b.start + off, b.length - off, Type::Hyphens);
    else
      b.type = Type::Hyphens;
  }
  return n;
}

TextSplitter::Iter TextSplitter::ensureAtomizedAfter(Iter it) {
  if (atEnd(it))
    return it;
  Iter it1 = it;
  ++it1;
  if (atEnd(it1))
    return it1;
  switch ((*it1).type) {
  case Type::Paragraph:
    splitAtSpace(it1);
    return ensureAtomizedAfter(it);
  case Type::Word:
    splitAtHyphenSlash(it1);
    return ensureAtomizedAfter(it);
  default:
    break;
  }
  return it1;
}

    
