// TextBlockData.H

#ifndef TEXTBLOCKDATA_H

#define TEXTBLOCKDATA_H

#include "BlockData.H"
#include "MarkupData.H"

class TextBlockData: public BlockData {
  Q_OBJECT;
  Q_PROPERTY(QString text READ text WRITE setText)
  Q_PROPERTY(QList<int> lineBreaks READ lineBreaks WRITE setLineBreaks)
  /* The lineBreaks property is optional and may be used to mark where
     line breaks should happen. This is useful to prevent reflow of text
     in case font metrics change subtly.
     Line breaks are *not* automatically forgotten when the text is changed,
     so if you use this feature, you are responsible for keeping them up to
     date.
     The numbers are the indices into the text of the starts of each line.
   */
public:
  TextBlockData(class PageData *parent);
  virtual ~TextBlockData();
   // read properties
  QString text() const;
  QList<int> const &lineBreaks() const;
  QList<MarkupData *> const &markups() const;
  // write properties
  void setText(QString const &);
  void setLineBreaks(QList<int> const &);
  // other
  MarkupData *addMarkup(int start, int end, MarkupData::Style style);
  void removeMarkup(MarkupData *); // the markup is destructed as well
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
protected:
  QString text_;
  QList<int> lineBreaks_;
  QList<MarkupData *> markups_;
};

#endif
