// Message.C

#include "Message.H"
#include <QDebug>
#include "Assert.H"
#include <QGraphicsScene>

QMap<QGraphicsScene const *, QList<Message *> >&Message::messages() {
  static QMap<QGraphicsScene const *, QList<Message *> > mm;
  return mm;
}

bool Message::contains(Message *m) {
  for (QMap<QGraphicsScene const *, QList<Message *> >::iterator i = messages().begin();
       i != messages().end(); ++i) 
    if (i.value().contains(m))
      return true;
  return false;
}
  
Message *Message::report(QString msg, Item *reporter) {
  ASSERT(reporter);
  QGraphicsScene *scene = reporter->scene();
  if (!scene) {
    qDebug() << "Message::report without a scene:";
    qDebug() << msg; // reasonable fallback?
    return 0;
  }
  Message *m = new Message();
  QList<Message *> &lst = messages()[scene];
  lst.append(m);
  m->setHtml(msg);
  scene->addItem(m);
  m->stylize(reporter->style());
  QRectF sr = scene->sceneRect();
  double y_b = sr.bottom();
  double x_l = sr.left();
  for (int n=lst.size()-1; n>=0; --n) {
    QRectF bb = lst[n]->boundingRect();
    lst[n]->setPos(x_l - bb.left(),
		   y_b - bb.bottom());
    y_b -= bb.height();
  }
  return m;
}

Message::Message() {
  timerID = 0;
}

void Message::reap(Message *m) {
  QMap<QGraphicsScene const *, QList<Message *> >::iterator j;
  for (QMap<QGraphicsScene const *, QList<Message *> >::iterator i = messages().begin();
       i != messages().end(); i=j) {
    i.value().removeAll(m);
    if (i.value().isEmpty())
      j = messages().erase(i);
    else
      j = ++i;
  }
}

Message::~Message() {
  reap(this);
}

Message *Message::replace(Message *toBeReplaced,
			  QString newMessage, Item *reporter) {
  if (contains(toBeReplaced)) {
    toBeReplaced->rejuvenate();
    toBeReplaced->setHtml(newMessage);
    return toBeReplaced;
  } else {
    return report(newMessage, reporter);
  }
}

void Message::remove(Message *m) {
  if (contains(m))
    delete m;
}

void Message::removeAfter(Message *m, double t_s) {
  if (!contains(m))
    return;
  m->rejuvenate();
  m->setMortality(t_s);
}

void Message::rejuvenate() {
  if (timerID)
    killTimer(timerID);
  timerID = 0;
}

void Message::setMortality(double t_s) {
  rejuvenate();
  timerID = startTimer(int(t_s * 1000));
}

void Message::timerEvent(QTimerEvent *) {
  deleteLater();
}

void Message::stylize(class Style const &) {
  // set font and transparency here
}
