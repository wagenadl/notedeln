// GfxBlockData.H

#ifndef GFXBLOCKDATA_H

#define GFXBLOCKDATA_H

#include "BlockData.H"

class GfxBlockData: public BlockData {
  Q_OBJECT
  Q_PROPERTY(double xref READ xref WRITE setXref)
  Q_PROPERTY(double yref READ yref WRITE setYref)
public:
  GfxBlockData(Data *parent=0);
  virtual ~GfxBlockData();
  double xref() const;
  double yref() const;
  QPointF ref() const;
  void setXref(double);
  void setYref(double);
  void setRef(QPointF);
  QList<class GfxData *> gfx() const;
  virtual bool isEmpty() const;
protected:
  double xref_, yref_;
};

#endif
