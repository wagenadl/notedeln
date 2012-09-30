// DataBlock.H

#ifndef DATABLOCK_H

#define DATABLOCK_H

#include "Data.H"
#include "DataNote.H"

class DataBlock: public Data {
public:
  DataBlock(QDomElement defn);
  DataBlock(QString tag, QDomElement parent,  QString type="block");
  virtual ~DataBlock();
  DataNote *addNote(QString tag, double x, double y);
  QList<DataNote const *> notes() const;
private:
  QList<DataNote *> notes_;
};

#endif
