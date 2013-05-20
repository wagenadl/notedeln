// App/Message.H - This file is part of eln

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

// Message.H

#ifndef MESSAGE_H

#define MESSAGE_H

#include <QMap>
#include <QList>
#include <QObject>
#include <QPointer>
#include "Notebook.H"
#include <QGraphicsScene>

class Message: public QObject {
  Q_OBJECT;
public:
  static Message *report(QString message, Data const *reporter);
  /*:F report
   *:D Reports a new message
   *:N Do not use the return value to attempt to manipulate the message
       directly. This is not safe because messages are designed to disappear
       automatically after some time.
       The only safe use of the return value is as an argument to the static
       functions replace(), remove(), and removeAfter().
  */
  static Message *replace(Message *toBeReplaced,
			  QString newMessage, Data const *reporter);
  /*:F replace
   *:D Replaces a previous message.
   *:N It's OK if the previous message no longer exists; a new one will be
       constructed in that case.
   */
  static void remove(Message *toBeRemoved);
  /*:F remove
   *:D Removes a previous message.
   *:N It's OK if the message doesn't exist anymore.
   */
  static void removeAfter(Message *toBeDelayed, double t_s);
  /*:F removeAfter
   *:D Removes a previous message after a set delay.
   *:N It's OK if the message doesn't exist.
   */
  virtual ~Message();
public:
  static void associate(class Notebook const *, class QGraphicsScene *);
  static void disassociate(class Notebook const *, class QGraphicsScene *);
protected:
  void timerEvent(QTimerEvent *);
private:
  Message(QString msg, Notebook /*const*/ *);
  static QMap<Notebook const *, QList<Message *> >&messages();
  static QMap<Notebook const *,
              QList<QPointer<QGraphicsScene> > > &scenes();
  static bool contains(Message *);
  /* Caution: use of contains() may not be thread-safe */
  void rejuvenate();
  void setMortality(double t_s);
  void addToScene(QGraphicsScene *);
  void deleteFromScene(QGraphicsScene *);
  void setHtml(QString);
private:
  QString msg;
  int timerID;
  QPointer<Notebook> book;
  QMap<QGraphicsScene *, QPointer<class MessageObject> > objects;
};

#endif
