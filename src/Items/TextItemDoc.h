// TextItemDoc.H

#ifndef TEXTITEMDOC_H

#define TEXTITEMDOC_H

#include <QObject>

#include <QFont>
#include <QColor>
#include <QRectF>

class TextItemDoc: public QObject {
  Q_OBJECT;
public:
  TextItemDoc(class TextData *data, QObject *parent=0);
  // Properties
  /* Setting a property does _not_ trigger automatic relayout. You must
     call relayout() directly.
  */
  ~TextItemDoc();
  void setFont(QFont const &f);
  QFont font() const;
  void setIndent(double pix);
  double indent() const;
  void setLeftMargin(double pix);
  double leftMargin() const;
  void setRightMargin(double pix);
  double rightMargin() const;
  void setWidth(double pix);
  double width() const;
  void setLineHeight(double pix);
  double lineHeight() const;
  void setColor(QColor const &c);
  QColor color() const;
  bool isEmpty() const;
  // Other functions
  QChar characterAt(int pos) const;
  QRectF boundingRect() const;
  QString text() const;
  QVector<int> lineStarts() const;
  int lineStartFor(int pos) const;
  int lineEndFor(int pos) const;
  void relayout(bool preserveWidths=false);
  void partialRelayout(int startOffset, int endOffset);
  void setSelection(class TextCursor const &c);
  void clearSelection();
  void render(class QPainter *, QRectF roi=QRectF()) const;
  int find(QPointF) const; // return offset from graphical position
  QRectF locate(int offset) const; // returns a 1-pix wide rectangle
  // at the location of the given offset.
  // The rectangle will be appropriately sized for a cursor.
  void insert(int offset, QString text);
  void remove(int offset, int length);
  int find(QString) const; // offset or -1
signals:
  void contentsChange(int pos, int nDel, int nIns);
private:
  class TextItemDocData *d;
};

#endif
