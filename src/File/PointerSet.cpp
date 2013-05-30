// PointerSet.cpp

#include "PointerSet.H"

void PointerSet::insert(QObject *o) {
  data.insert(o);
  connect(o, SIGNAL(destroyed(QObject*)), SLOT(remove(QObject*)));
}

void PointerSet::remove(QObject *o) {
  if (data.remove(o) && isEmpty())
    emit emptied();
}

bool PointerSet::isEmpty() const {
  return data.isEmpty();
}

void PointerSet::clear() {
  if (isEmpty())
    return;
  data.clear();
  emit emptied();
}
