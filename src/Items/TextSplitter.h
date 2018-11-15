// TextSplitter.h

#ifndef TEXTSPLITTER_H

#define TEXTSPLITTER_H

#include <QString>
#include <QLinkedList>
#include <QVector>

class TextSplitter {
public:
  enum class Type {
    Any,
    Newline,
    Paragraph,
    Space,
    Word,
    Hyphens,
    Atom
  };
  struct Bit {
    QString text; // text of the bit
    double width; // pixel width of the bit
    int length; // length of text in characters
    int start; // offset from start of text
    Type type;
  };
  typedef QLinkedList<Bit> List;
  typedef List::iterator Iter;
public:
  TextSplitter(QString text, QVector<double> const &charwidths);
  List const &bits() const { return bits_; };
  List &bits() { return bits_; };
  void splitAtNewlines(Iter it);
  int splitAtSpace(Iter it);
  int splitAtHyphenSlash(Iter it);
  Iter ensureAtomizedAfter(Iter it); // returns iterator to atomized bit
  bool atEnd(Iter it) const { return it==bits_.end(); }
  bool isType(Iter it, Type t) const { return !atEnd(it) && (*it).type==t; }
  static bool isHyphenSlash(QChar c);
  static int indexOfHS(QString const &str, int from=0);
private:
  Bit makeBit(int start, int length, Type type);
private:
  QString text;
  List bits_;
  QVector<double> charwidths;
  
};

#endif
