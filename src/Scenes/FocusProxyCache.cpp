// FocusProxyCache.cpp

#include "FocusProxyCache.H"

FocusProxyCache::FocusProxyCache(QGraphicsItem *gi) {
  if (gi->hasFocus())
    oldfocus = gi;
  else
    oldfocus = 0;
  store(gi);
}

void FocusProxyCache::store(QGraphicsItem *gi) {
  QGraphicsItem *pr = gi->focusProxy();
  if (pr) {
    items << gi;
    proxies << pr;
  }
  foreach (QGraphicsItem *c, gi->childItems())
    store(c);
}

void FocusProxyCache::restore() {
  for (int n=0; n<items.size(); n++) 
    if (items[n])
      items[n]->setFocusProxy(proxies[n]);
  if (oldfocus)
    oldfocus->setFocus();
}
