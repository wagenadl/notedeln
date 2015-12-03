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
