// EventView.h

#ifndef EVENTVIEW_H

#define EVENTVIEW_H

#include <QPointer>
#include "PageView.h"
#include "Mode.h"
#include <QList>

class EventView {
public:
  EventView(PageView *view);
  ~EventView();
  static PageView *eventView();
  static Mode *mode();
private:
  static QList<QPointer<PageView> > &view_list();
  static QPointer<Mode> &mode_();
  static QPointer<PageView> &view_();
};

#endif
