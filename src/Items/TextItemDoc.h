// TextItemDoc.H

#ifndef TEXTITEMDOC_H

#define TEXTITEMDOC_H

#include <QFont>
#include <QColor>
#include <QRectF>

class TextItemDoc {
public:
  TextItemDoc(class TextData *data);
  // Properties
  /* Setting a property does _not_ trigger automatic relayout. You must
     call relayout() directly.
  */
  ~TextItemDoc();
  void setFont(QFont const &f);
  QFont font() const;
  void setIndent(double pix);
  double indent() const;
  void setWidth(double pix);
  double width() const;
  void setLineHeight(double pix);
  double lineHeight() const;
  void setColor(QColor const &c);
  QColor color() const;
  // Other functions
  QRectF boundingRect() const;
  QString text() const;
  QVector<int> lineStarts() const;
  int lineStartFor(int pos) const;
  int lineEndFor(int pos) const;
  void relayout(bool preserveWidths=false);
  void partialRelayout(int startOffset);
  void render(class QPainter *, QRectF roi=QRectF()) const;
  int find(QPointF) const; // return offset from graphical position
  QRectF locate(int offset) const; // returns a 1-pix wide rectangle
  // at the location of the given offset.
  // The rectangle will be appropriately sized for a cursor.
  void insert(int offset, QString text);
  void remove(int offset, int length);
private:
  class TextItemDocData *d;
};

#endif
