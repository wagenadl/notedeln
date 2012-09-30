// DataNote.H

#ifndef DATANOTE_H

#define DATANOTE_H

#include "DataText.H"

class DataNote: public DataText {
public:
  DataNote(QDomElement defn);
  DataNote(QString id, QDomElement parent, QString type="note");
  double x() const;
  double y() const;
  void setX(double x);
  void setY(double y);
};

#endif
