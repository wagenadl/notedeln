// ModSnooper.H

#ifndef MODSNOOPER_H

#define MODSNOOPER_H

#include <QObject>
#include <QApplication>

class ModSnooper: public QObject {
  Q_OBJECT;
public:
  ModSnooper(QObject *parent=0);
  virtual ~ModSnooper();
  void x11Event(XEvent *event); // must be called to maintain state on X11
  void keyPress(int key);   // } must be called to maintain ...
  void keyRelease(int key); // } ... state, except on X11
  bool anyShift() const;
  bool anyControl() const; 
  bool anyAlt() const;
  Qt::KeyboardModifiers keyboardModifiers() const;  
signals:
  void modifiersChanged(Qt::KeyboardModifiers);
private:
  void emitMC();
private:
  int shiftCount;
  int controlCount;
  int altCount;
};

#endif
