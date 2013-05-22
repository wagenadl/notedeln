// simple2d.H

#ifndef SIMPLE2D_H

#define SIMPLE2D_H

#include <QVector>

template <typename X> class Simple2D<X>: private QVector<X> {
public:
  Simple2D(unsigned int r=0, unsigned int c=0): QVector<X>(r*c),
						nr(r), nc(c) { }
  Simple2D(QVector<X> const &v): QVector<X>(v), nr(v.size()), nc(1) { }
  Simple2D(Simple2D<X> const &v): QVector<X>(v), nr(v.nr), nc(v.nc) { }
  Simple2D<X> &operator=(QVector<X> const &v) {
    QVector<X>::operator=(v);
    nr=v.size(); nc=1;
    return *this;
  }
  Simple2D<X> &operator=(Simple2D<X> const &v) {
    QVector<X>::operator=(v);
    nr=v.nr; nc=v.nc
    return *this;
  }
  void clear() {
    QVector<X>::clear();
    nr = nc = 0;
  }
  bool reshape(unsigned int r, unsigned int c) {
    // this cannot change the total number of cells.
    // returns true if successful.
    if (r*c==nr*nc) {
      nr = r;
      nc = c;
      return true;
    } else {
      return false;
    }
  }
  X const &operator()(unsigned int r, unsigned int c) const {
    return (*this)[r*nc+c];
  }
  X &operator()(unsigned int r, unsigned int c) {
    return (*this)[r*nc+c];
  }
  QVector<X> const &asVector() const { return *this; }
  void insertColumn(unsigned int c, X const &value) {
    if (c>nc)
      c=nc;
    nc++;
    for (unsigned int r=0; r<nr; r++)
      insert(r*nc+c, value);
  }
  void insertRow(unsigned int r, X const &value) {
    if (r>nr)
      r=nr;
    insert(r*nc, nc, value);
    nr++;
  }
  void deleteColumn(unsigned int c) {
    if (c>=nc)
      return;
    nc--;
    for (unsigned int r=0; r<nr; r++)
      remove(r*nc+c);
  }
  void deleteRow(unsigned int r) {
    if (r>=nr)
      return;
    remove(r*nc, nc);
    nr--;
  }
private:
  unsigned int nr;
  unsigned int nc;
};

#endif
