// BlockData.H

#ifndef BLOCKDATA_H

#define BLOCKDATA_H

#include "Data.H"

class BlockData: public Data {
  Q_OBJECT;
  Q_PROPERTY(double y0 READ y0 WRITE setY0)
  Q_PROPERTY(int sheet READ sheet WRITE setSheet)
public:
  // constructor and destructor  
  BlockData(Data *parent=0);
  virtual ~BlockData();
  // read properties
  double y0() const;
  int sheet() const;
  QList<class NoteData *> notes() const;
  // write properties
  void setY0(double);
  void setSheet(int);
  // other
  void addNote(NoteData *);
  bool deleteNote(NoteData *);
signals:
  void newSheet(int); // not emitted during loading
private:
  double y0_;
  int sheet_;
};

#endif
