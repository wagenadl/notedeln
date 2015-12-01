// EventView.cpp

#include "EventView.h"

EventView::EventView(PageView *view) {
  view_list() << QPointer<PageView>(view);
  if (view) {
    mode_() = QPointer<Mode>(view->mode());
    view_() = QPointer<PageView>(view);
  }
}

EventView::~EventView() {
  view_list().takeLast();
  if (!view_list().isEmpty() && view_list().last()) {
    mode_() = QPointer<Mode>(view_list().last()->mode());
    view_() = QPointer<PageView>(view_list().last());
  }
}

PageView *EventView::eventView() {
  return view_();
}

Mode *EventView::mode() {
  return mode_();
}

QList< QPointer<PageView> > &EventView::view_list() {
  static QList< QPointer<PageView> > ptr;
  return ptr;
}

QPointer<Mode> &EventView::mode_() {
  static QPointer<Mode> m;
  return m;
}

QPointer<PageView> &EventView::view_() {
  static QPointer<PageView> v;
  return v;
}
