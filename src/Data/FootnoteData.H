// FootnoteData.H

#ifndef FOOTNOTEDATA_H

#define FOOTNOTEDATA_H

#include "TextData.H"

class FootnoteData: public TextData {
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
