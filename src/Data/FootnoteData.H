// FootnoteData.H

#ifndef FOOTNOTEDATA_H

#define FOOTNOTEDATA_H

#include "TextBlockData.H"

class FootnoteData: public TextBlockData {
  /* We inherit TextBlockData so that a FootnoteItem can be a BlockItem,
     but we don't actually use the y0 and sheet properties for now.
  */
  Q_OBJECT;
  Q_PROPERTY(QString tag READ tag WRITE setTag)
public:
  FootnoteData(Data *parent=0);
  virtual ~FootnoteData();
  QString tag() const;
  void setTag(QString);
private:
  QString tag_;
};

#endif
