// App/Message.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// Message.C

#include "Message.h"
#include <QDebug>
#include "Assert.h"
#include <QGraphicsScene>
#include "MessageObject.h"


QMap<Notebook const *, QList<Message *> >&Message::messages() {
  static QMap<Notebook const *, QList<Message *> > mm;
  return mm;
}

QMap<Notebook const *, QList<QPointer<QGraphicsScene> > > &Message::scenes() {
  static QMap<Notebook const *, QList<QPointer<QGraphicsScene> > > sc;
  return sc;
}


bool Message::contains(Message *m) {
  for (QMap<Notebook const *, QList<Message *> >::iterator
         i = messages().begin(); i != messages().end(); ++i) 
    if (i.value().contains(m))
      return true;
  return false;
}
  
Message *Message::report(QString msg, Data const *reporter) {
  ASSERT(reporter);
  Notebook *book = reporter->book();
  if (!book) {
    qDebug() << "Message::report without a book:";
    qDebug() << msg; // reasonable fallback?
    return 0;
  }
  Message *m = new Message(msg, book);
  QList<Message *> &lst = messages()[book];
  lst.append(m);
  return m;
}

Message::Message(QString msg, Notebook *book): msg(msg), book(book) {
  timerID = 0;
  foreach (QPointer<QGraphicsScene> scenep, scenes()[book]) 
    if (scenep)
      addToScene(scenep);
}

void Message::deleteFromScene(QGraphicsScene *scene) {
  if (objects.contains(scene)) {
    MessageObject *obj = objects[scene];
    if (obj) {
      scene->removeItem(obj);
      obj->deleteLater();
    }
    objects.remove(scene);
  }
}

void Message::addToScene(QGraphicsScene *scene) {
  if (!book) {
    qDebug() << "Message::addToScene: no book";
    return;
  }
  MessageObject *obj = new MessageObject(msg, book->style());
  scene->addItem(obj);
  objects[scene] = obj;
 
  QRectF sr = scene->sceneRect();
  double y_b = sr.bottom();
  double x_l = sr.left();
  QRectF bb = obj->boundingRect();
  obj->setPos(x_l - bb.left(),
              y_b - bb.bottom());
  // Of course, we need to actually position messages in a neat stack,
  // not smash them on top of each other
}

Message::~Message() {
  foreach (MessageObject *mo, objects)
    mo->deleteLater();
  
  QMap<Notebook const *, QList<Message *> >::iterator i, j;
  for (i = messages().begin(); i != messages().end(); i=j) {
    i.value().removeAll(this);
    if (i.value().isEmpty())
      j = messages().erase(i);
    else
      j = ++i;
  }
}

Message *Message::replace(Message *toBeReplaced,
			  QString newMessage, Data const *reporter) {
  if (contains(toBeReplaced)) {
    toBeReplaced->rejuvenate();
    toBeReplaced->setHtml(newMessage);
    return toBeReplaced;
  } else {
    return report(newMessage, reporter);
  }
}

void Message::setHtml(QString s) {
  foreach (QPointer<MessageObject> obj, objects) {
    if (obj)
      obj->setHtml(s);
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

void Message::associate(Notebook const *nb, QGraphicsScene *scene) {
  QList<QPointer<QGraphicsScene> > &sc(scenes()[nb]);
  foreach (QPointer<QGraphicsScene> p, sc) 
    if (p==scene)
      return; // already contained

  sc.append(scene);
  foreach (Message *m, messages()[nb]) 
    m->addToScene(scene);
}

void Message::disassociate(Notebook const *nb, QGraphicsScene *scene) {
  QList<QPointer<QGraphicsScene> > &sc(scenes()[nb]);
  for (QList<QPointer<QGraphicsScene> >::iterator
         i = sc.begin(); i!=sc.end(); ++i) {
    QPointer<QGraphicsScene> p = *i;
    if (p==scene) {
      sc.erase(i);
      break;
    }
  }

  foreach (Message *m, messages()[nb])
    m->deleteFromScene(scene);
  if (sc.isEmpty())
    scenes().remove(nb);
}
