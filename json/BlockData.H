// BlockData.H

#ifndef BLOCKDATA_H

#define BLOCKDATA_H

#include "Data.H"

class BlockData: public Data {
  Q_OBJECT;
  Q_PROPERTY(double y0 READ y0 WRITE setY0)
  Q_PROPERTY(double h READ h WRITE setH)
public:
  // constructor and destructor  
  BlockData(class PageData *parent=0);
  virtual ~BlockData();
  // read properties
  double y0() const;
  double h() const;
  QList<class NoteData *> const &notes() const;
  // write properties
  void setY0(double);
  void setH(double);
  // other
  void addNote(NoteData *);
protected:
  virtual void loadMore(QVariantMap const &src);
  virtual void saveMore(QVariantMap &dst) const;
protected:
  double y0_;
  double h_;
private:
  QList<class NoteData *> notes_;
};

#endif
