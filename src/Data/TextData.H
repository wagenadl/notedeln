// TextData.H

#ifndef TEXTDATA_H

#define TEXTDATA_H

#include "Data.H"
#include "MarkupData.H"

class TextData: public Data {
  Q_OBJECT;
  Q_PROPERTY(QString text READ text WRITE setText)
public:
  TextData(Data *parent=0);
  virtual ~TextData();
   // read properties
  QString text() const;
  // write properties
  void setText(QString const &);
  /* If you change the text in the presence of markups, you are responsible
     for updating the markups */
  // other
  QList<MarkupData *> markups() const;
  MarkupData *addMarkup(int start, int end, MarkupData::Style style);
  void deleteMarkup(MarkupData *);
protected:
  QString text_;
};

#endif
