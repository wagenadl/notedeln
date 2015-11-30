// EventView.cpp

#include "EventView.h"

EventView::EventView(PageView *view) {
  event_view() << QPointer<PageView>(view);
}

EventView::~EventView() {
  event_view().takeLast();
}

PageView *EventView::eventView() {
  if (!event_view().isEmpty())
    return event_view().last();
  else
    return 0;
}

QList< QPointer<PageView> > &EventView::event_view() {
  static QList< QPointer<PageView> > ptr;
  return ptr;
}
