// BlockData.H

#ifndef BLOCKDATA_H

#define BLOCKDATA_H

class BlockData: public Data {
  Q_OBJECT;
  Q_PROPERTY(QList notes READ notes WRITE setNotes)
public:
  // constructor and destructor  
  BlockData(class PageData *parent=0);
  virtual ~BlockData();
  // read properties
  QList<class NoteData *> const &notes() const;
  // write properties - not for casual use
  void setNotes(QList<class NoteData *> const &);
  // other
  void addNote(NoteData *);
private:
  QList<class NoteData *> notes_;
};

#endif
