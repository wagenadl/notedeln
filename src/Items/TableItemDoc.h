// TableItemDoc.h

#ifndef TABLEITEMDOC_H

#define TABLEITEMDOC_H

#include "TextItemDoc.h"

class TableItemDoc: public TextItemDoc {
  Q_OBJECT;
public:
  TableItemDoc(class TableData *data, QObject *parent=0);
  virtual ~TableItemDoc() { }
  virtual void relayout(bool preserveWidths=false);
  virtual int firstPosition() const;
  virtual int lastPosition() const;
  virtual int find(QPointF p, bool strict=true) const;
  virtual QString selectedText(int start, int end) const;
  QPointF cellLocation(int r, int c) const;
  QRectF cellBoundingRect(int r, int c) const;
protected:
  void buildLinePos();
protected:
  TableData const *table() const;
  TableData *table();
};

#endif
