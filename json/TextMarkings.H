// TextMarkings.H

#ifndef TEXTMARKINGS_H

#define TEXTMARKINGS_H

#include <QObject>
#include <QList>
#include <QTextDocument>

class TextMarkings: public QObject {
  Q_OBJECT;
public:
  enum Type {
    None,
    Italic,
    Emphasize,
    Crossout
  };
public:
  TextMarkings(QTextDocument *doc, QObject *parent=0);
  // of course we should have a TextBlockData object too...
  virtual ~TextMarkings();
  void newMark(Type t, int start, int end);
private slots:
  void update(int, int, int);
private:
  QTextDocument *doc;
  class Span {
  public:
    Type type;
    int start;
    int end;
    Span(Type t=None, int s=0, int e=0);
    bool update(QTextDocument *doc, int pos, int del, int ins);
    // returns true if the span becomes empty
    bool operator<(Span const &other) const;
  };
  QList<Span> spans; // kept in order of non-decreasing starts
};

#endif
