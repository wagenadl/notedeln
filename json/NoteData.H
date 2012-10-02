// NoteData.H

#ifndef NOTEDATA_H

#define NOTEDATA_H

#include "Data.H"

class NoteData: public Data {
  Q_OBJECT;
  Q_PROPERTY(double x READ x WRITE setX)
  Q_PROPERTY(double y READ y WRITE setY)
  Q_PROPERTY(QString text READ text WRITE setText)
public:
  // constructor and destructor  
  NoteData(BlockData *parent=0);
  virtual ~NoteData();
  // read properties
  double x() const;
  double y() const;
  QString text() const;
  // write properties
  void setX(double);
  void setY(double);
  void setText(QString const &);
protected:
  double x_;
  double y_;
  QString text_;
};

#endif
