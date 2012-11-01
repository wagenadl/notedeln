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
  bool isEmpty() const;
  QList<MarkupData *> markups() const;
  MarkupData *addMarkup(int start, int end, MarkupData::Style style);
  void deleteMarkup(MarkupData *);
  //  void append(TextData *);
  /*:F append
   *:D Appends the text of another TextData object to our text and imports
       all markups.
   */
  //  TextData *split(int pos);
  /*:F split
   *:D Takes the text from the given position onwards and moves it into a
       new TextData object, taking markups with it.
   *:N The new object is created as a child of the original object's parent;
       use takeChild() to disjoin it if needed.
   */
protected:
  QString text_;
};

#endif
