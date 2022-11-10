// Items/TextSplitter.h - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// TextSplitter.h

#ifndef TEXTSPLITTER_H

#define TEXTSPLITTER_H

#include <list>
#include <QString>
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
  typedef std::list<Bit> List; // this *must* be a linked list, or our
                               // insertion semantics will cause
                               // crashes. QLinkedList has been
                               // abolished.
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
