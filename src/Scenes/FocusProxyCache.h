// FocusProxyCache.H

#ifndef FOCUSPROXYCACHE_H

#define FOCUSPROXYCACHE_H

#include <QGraphicsItem>
#include <QPointer>
#include <QList>

class FocusProxyCache {
public:
  FocusProxyCache(QGraphicsItem *gi);
  void restore();
private:
  void store(QGraphicsItem *gi);
private:
  QList<QGraphicsItem *> items;
  QList<QGraphicsItem *> proxies;
  QGraphicsItem *oldfocus;
  /* I wanted to use QPointers, but cannot */
};

#endif
