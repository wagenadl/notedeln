// ModSnooper.H

#ifndef MODSNOOPER_H

#define MODSNOOPER_H

#include <QApplication>

class ModSnooper: public QApplication {
public:
  ModSnooper(int argc, char **argv);
  virtual ~ModSnooper();
  virtual bool x11EventFilter(XEvent *event);
  bool haveState() const;
  bool anyShift() const;
  bool anyControl() const; 
  bool anyAlt() const;
  static ModSnooper *instance();
private:
  int shiftCount;
  int controlCount;
  int altCount;
};

#endif
