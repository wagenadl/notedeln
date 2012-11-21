// TextMarkings.H

#ifndef TEXTMARKINGS_H

#define TEXTMARKINGS_H

#include <QGraphicsObject>
#include <QList>
#include <QTextDocument>
#include "MarkupData.H"
#include "TextData.H"

class TextMarkings: public QGraphicsObject {
  Q_OBJECT;
public:
  TextMarkings(TextData *data, class TextItem *parent);
  virtual ~TextMarkings();
  void newMark(MarkupData::Style t, int start, int end);
  void newMark(MarkupData *); // the data object is appropriated into our text
  TextItem *parent() const;
  void foundUrl(QString); // recolors the span if it exists
  QRectF boundingRect() const;
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
private slots:
  void update(int, int, int);
private:
  class Span;
  Span &insertMark(MarkupData *);
  void applyMark(Span const &);
private:
  class Span {
  public:
    MarkupData *data;
    QString refText;
    bool archivedUrl;
  public:
    Span(MarkupData *data, TextMarkings *tm=0);
    bool cut(int pos, int len);
    void insert(TextItem *item, int pos, int len);
    bool update(TextItem *item, int pos, int del, int ins, TextMarkings *);
    // returns true if the span becomes empty
    bool operator<(Span const &other) const;
    void avoidPropagatingStyle(TextItem *item, int pos, int len);
  };
private:
  TextData *data;
  QList<Span> spans; // kept in order of non-decreasing starts
  QMap<class MarkupData *, class HoverRegion *> regions;
  QTextDocument *doc;
};

#endif
