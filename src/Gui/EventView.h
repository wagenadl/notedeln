// EventView.h

#ifndef EVENTVIEW_H

#define EVENTVIEW_H

#include <QPointer>
#include "PageView.h"
#include <QList>

class EventView {
public:
  EventView(PageView *view);
  ~EventView();
  static PageView *eventView();
private:
  static QList<QPointer<PageView> > &event_view();
};

#endif
