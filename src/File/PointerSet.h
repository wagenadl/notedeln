// PointerSet.H

#ifndef POINTERSET_H

#define POINTERSET_H

#include <QObject>
#include <QSet>

class PointerSet: public QObject {
  Q_OBJECT;
public:
  void insert(QObject *);
  bool isEmpty() const;
  template <class T> QList<T*> toList() const {
    QList<T*> lst;
    foreach (QObject *p, data) {
      T *p1 = dynamic_cast<T*>(p);
      if (p1)
        lst.append(p1);
    }
    return lst;
  }
  void clear();
public slots:
  void remove(QObject *);
signals:
  void emptied();
protected:
  QSet<QObject*> data;
};

#endif
