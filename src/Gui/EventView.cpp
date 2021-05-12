// Gui/EventView.cpp - This file is part of NotedELN

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

// EventView.cpp

#include "EventView.h"

EventView::EventView(PageView *view) {
  view_list() << QPointer<PageView>(view);
  if (view) {
    view_() = QPointer<PageView>(view);
  }
}

EventView::~EventView() {
  view_list().takeLast();
  if (!view_list().isEmpty() && view_list().last()) {
    view_() = QPointer<PageView>(view_list().last());
  }
}

PageView *EventView::eventView() {
  return view_();
}

QList< QPointer<PageView> > &EventView::view_list() {
  static QList< QPointer<PageView> > ptr;
  return ptr;
}

QPointer<PageView> &EventView::view_() {
  static QPointer<PageView> v;
  return v;
}
