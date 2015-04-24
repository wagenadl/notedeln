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
  void setFont(QFont const &f) { font_ = f; }
  QFont font() const { return font_; }
  void setIndent(double pix) { indent_ = pix; }
  double indent() const { return indent_; }
  void setWidth(double pix) { width_ = pix; }
  double width() const { return width_; }
  void setLineHeight(double pix) { lineheight_ = pix; }
  double lineHeight() const { return lineheight_; }
  void setColor(QColor const &c) { color_ = c; }
  QColor color() const { return color_; }
  // Other functions
  QRectF boundingRect() const { return br; }
  void relayout();
  void partialRelayout(int startOffset);
  void render(class QPainter *, QRectF roi=QRectF()) const;
  int find(QPointF) const; // return offset from graphical position
  QRectF locate(int offset) const; // returns a 1-pix wide rectangle
  // at the location of the given offset.
  // The rectangle will be appropriately sized for a cursor.
private:
  TextData *data;
  QFont font_;
  double indent_;
  double width_;
  double lineheight_;
  QColor color_;
  QRectF br; 
};

#endif
