// Mode.H

#ifndef MODE_H

#define MODE_H

#include <QObject>

class Mode: public QObject {
  Q_OBJECT;
public:
  enum M {
    Browse,
    Type,
    MoveResize,
    Mark,
    Freehand,
    Annotate
  };
public:
  Mode(QObject *parent=0);
  virtual ~Mode();
  M mode() const;
public slots:
  void setMode(M m);
signals:
  void modeChanged(M m);
private:
  M m;
};

#endif
