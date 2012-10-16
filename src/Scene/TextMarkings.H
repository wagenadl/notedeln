// TextMarkings.H

#ifndef TEXTMARKINGS_H

#define TEXTMARKINGS_H

#include <QObject>
#include <QList>
#include <QTextDocument>
#include "MarkupData.H"
#include "TextData.H"

class TextMarkings: public QObject {
  Q_OBJECT;
public:
  TextMarkings(TextData *data, QTextDocument *doc, QObject *parent=0);
  virtual ~TextMarkings();
  void newMark(MarkupData::Style t, int start, int end);
private slots:
  void update(int, int, int);
private:
  void insertMark(MarkupData *);
  void applyMark(MarkupData const *);
private:
  class Span {
  public:
    MarkupData *data;
  public:
    Span(MarkupData *data);
    bool cut(int pos, int len);
    void insert(QTextDocument *doc, int pos, int len);
    bool update(QTextDocument *doc, int pos, int del, int ins);
    // returns true if the span becomes empty
    bool operator<(Span const &other) const;
    void avoidPropagatingStyle(QTextDocument *doc, int pos, int len);
  };
private:
  TextData *data;
  QList<Span> spans; // kept in order of non-decreasing starts
  QTextDocument *doc;
};

#endif
