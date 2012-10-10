// TextData.H

#ifndef TEXTDATA_H

#define TEXTDATA_H

#include "Data.H"

#include <QList>

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
  QList<MarkupData *> const &markups() const;
  MarkupData *addMarkup(int start, int end, MarkupData::Style style);
  void removeMarkup(MarkupData *); // the markup is destructed as well
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
protected:
  QString text_;
  QList<MarkupData *> markups_;
};

#endif
