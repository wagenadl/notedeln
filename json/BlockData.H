// BlockData.H

#ifndef BLOCKDATA_H

#define BLOCKDATA_H

class BlockData: public Data {
  Q_OBJECT;
  Q_PROPERTY(QList notes READ notes)
public:
  // constructor and destructor  
  BlockData(class PageData *parent=0);
  virtual ~BlockData();
  // read properties
  QList<class NoteData *> const &notes() const;
  // other
  void addNote(NoteData *);
protected:
  virtual void loadMore(QVariantMap const &src);
  virtual void saveMore(QVariantMap &dst) const;
private:
  QList<class NoteData *> notes_;
};

#endif
