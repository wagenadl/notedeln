// Note.H

#ifndef NOTE_H

#define NOTE_H

/* A Note represents a sticky note attached to a chunk of text or graphics
   at a later point in time.
*/

class Note: public QGraphicsRectItem {
  Q_OBJECT;
public:
  Note(QDomElement elt, double x0=0, double y0=0, QGraphicsItem *parent=0);
  /* x0 and y0 are where in the parent this Note applies.
     The Note will find an appropriate place in the margin by itself.
   */
  virtual ~Note();
  QDateTime created() const;
  QDateTime modified() const;
  double x0() const;
  double y0() const;
  QString text() const;
signals:
  void changed();
private:
  QDomElement xml;
  QGraphicsItem *dateline;
  QGraphicsTextItem *contents;
  /* Note that a QGraphicsTextItem contains a QTextDocument which can emit
     contentsChanged() signals. */
};

#endif
