// TableData.H

#ifndef TABLEDATA_H

#define TABLEDATA_H

#include "TextData.H"
#include <QVector>

class TableData: public TextData {
  Q_OBJECT;
  Q_PROPERTY(int nr READ rows WRITE setRows)
  Q_PROPERTY(int nc READ columns WRITE setColumns)
public:
  TableData(Data *parent=0);
  virtual ~TableData();
  unsigned int rows() const;
  unsigned int columns() const;
  void setRows(unsigned int r); // invalidates cell lengths
  void setColumns(unsigned int c); // invalidates cell lengths
  unsigned int cellLength(unsigned int r, unsigned int c) const;
  void setCellLength(unsigned int r, unsigned int c, unsigned int len,
		     bool hushhush = false);
  /* Use hushhush to prevent escalation of the modification. This is useful
     because setCellLength will make the data inconsistent until the text
     is updated with setText. So the drill is: *first* call setCellLength,
     with hushhush true, then, AS SOON AS POSSIBLE, call setText to make
     things be consistent again.
     I should probably reconsider this mechanism, and instead implement
     a setCellContents method. Then I could make setText private, because
     setText is dangerous on a table: there is no verification of internal
     consistency.
     On the other hand, TextData has a similar issue with setText and markups,
     and it doesn't enforce anything.
  */
  unsigned int cellStart(unsigned int r, unsigned int c) const;
  QString cellContents(unsigned int r, unsigned int c) const;
  virtual bool isEmpty() const;
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
protected:
  inline unsigned int rc2index(unsigned int r, unsigned int c) const {
    return r*nc+c;
  }
protected:
  unsigned int nr;
  unsigned int nc;
  QVector<unsigned int> lengths;
private:
  mutable QVector<unsigned int> starts;
  mutable unsigned int firstInvalidStart;
};

#endif
