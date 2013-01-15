// Message.H

#ifndef MESSAGE_H

#define MESSAGE_H

#include "Item.H"
#include <QMap>
#include <QList>

class Message: public QGraphicsTextItem {
  Q_OBJECT;
public:
  static Message *report(QString message, Item *reporter);
  /*:F report
   *:D Reports a new message
   *:N Do not use the return value to attempt to manipulate the message
       directly. This is not safe because messages are designed to disappear
       automatically after some time.
       The only safe use of the return value is as an argument to the static
       functions replace(), remove(), and removeAfter().
  */
  static Message *replace(Message *toBeReplaced,
			  QString newMessage, Item *reporter);
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
protected:
  void timerEvent(QTimerEvent *);
private:
  Message();
  static QMap<QGraphicsScene *, QList<Message *> > messages;
  static bool contains(Message *);
  /* Caution: use of contains() may not be thread-safe */
  static void reap(Message *);
  void rejuvenate();
  void setMortality(double t_s);
  void stylize(class Style const &);
private:
  int timerID;
};

#endif
