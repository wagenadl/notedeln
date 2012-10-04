// GfxData.H

#ifndef GFXDATA_H

#define GFXDATA_H

#include "Data.H"
#include <QMap>

class GfxData: public Data {
  Q_OBJECT
  Q_PROPERTY(double x READ x WRITE setX)
  Q_PROPERTY(double y READ y WRITE setY)
public:
  GfxData(class GfxBlockData *parent=0);
  ~GfxData();
  static GfxData *create(QString type);
  // read properties
  double x() const;
  double y() const;
  // write properties
  void setX(double);
  void setY(double);
protected:
  double x_;
  double y_;
public: // but only to be used in defs of descendents
  template <class T> class Creator {
  public:
    Creator<T>(QString typ) {
      GfxData::creators()[typ] = &create;
    }
    static GfxData *create() {
      return new T();
    }
  };
private:
  static QMap<QString, GfxData *(*)()> &creators();
};

#endif
