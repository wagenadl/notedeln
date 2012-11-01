// DataTemplates.H

#ifndef DATATEMPLATES_H

#define DATATEMPLATES_H

template <class T> QList<T *> Data::children() const {
  QList<T *> res;
  for (QList<Data*>::const_iterator i=children_.begin();
       i!=children_.end(); ++i) {
    T *c = dynamic_cast<T*>(*i);
    if (c)
      res.append(c);
  }
  return res;
}

template <class T> T *Data::firstChild() const {
  for (QList<Data*>::const_iterator i=children_.begin();
       i!=children_.end(); ++i) {
    T *c = dynamic_cast<T*>(*i);
    if (c)
      return c;
  }
  return 0;
}

template <class T> T *Data::deepCopy(T const *data) {
  T *copy = new T(data->parent());
  copy->load(data->save());
  return copy;
}

#endif
