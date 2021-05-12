// Scenes/FocusProxyCache.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// FocusProxyCache.cpp

#include "FocusProxyCache.h"

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
