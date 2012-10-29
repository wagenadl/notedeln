// ModSnooper.C

#include "ModSnooper.H"
#include <QDebug>

#if QQ_X11
  #include <X11/Xlib.h>
  #include <X11/XKBlib.h>
  #define XK_MISCELLANY
  #define XK_XKB_KEYS
  #include <X11/keysymdef.h>
#endif

ModSnooper::ModSnooper(QObject *parent): QObject(parent) {
  shiftCount = 0;
  controlCount = 0;
  altCount = 0;
}

ModSnooper::~ModSnooper() {
}

bool ModSnooper::anyShift() const {
  return shiftCount>0;
}

bool ModSnooper::anyControl() const {
  return controlCount>0;
}

bool ModSnooper::anyAlt() const {
  return altCount>0;
}

#ifdef QQ_X11
void ModSnooper::keyPress(int) {
}

void ModSnooper::keyRelease(int) {
}

void ModSnooper::x11Event(XEvent *event) {
  if (event->type==KeyPress || event->type==KeyRelease) {
    XKeyEvent *e = (XKeyEvent*)event;
    int delta = event->type==KeyPress ? 1 : -1;
    int sym = XkbKeycodeToKeysym(e->display, e->keycode, 0, 0);
    switch (sym) {
    case XK_Shift_L: case XK_Shift_R:
      shiftCount += delta;
      if (shiftCount<0)
	shiftCount=0;
      emitMC();
      break;
    case XK_Control_L: case XK_Control_R:
       controlCount += delta;
      if (controlCount<0)
	controlCount=0;
      emitMC();
      break;
    case XK_Meta_L: case XK_Meta_R:
    case XK_Alt_L: case XK_Alt_R:
    case XK_ISO_Level3_Shift: // this is on my right alt...
      altCount += delta;
      if (altCount<0)
	altCount=0;
      emitMC();
      break;
    }
  }
}

#else
// Not X11
void ModSnooper::keyPress(int key) {
  switch (key) {
  case Qt::Key_Alt: case Qt::Key_AltGr:
    altCount = 1;
    emitMC();
    break;
  case Qt::Key_Control:
    ctrlCount = 1;
    emitMC();
    break;
  case Qt::Key_Shift:
    shiftCount = 1;
    emitMC();
    break;
  }
}

void ModSnooper::keyRelease(int key) {
  switch (key) {
  case Qt::Key_Alt: case Qt::Key_AltGr:
    altCount = 0;
    emitMC();
    break;
  case Qt::Key_Control:
    ctrlCount = 0;
    emitMC();
    break;
  case Qt::Key_Shift:
    shiftCount = 0;
    emitMC();
    break;
  }
}

bool x11EventFilter(XEvent *) {
  return false;
}
#endif

Qt::KeyboardModifiers ModSnooper::keyboardModifiers() const {
  Qt::KeyboardModifiers m = 0;
  if (shiftCount>0)
    m |= Qt::ShiftModifier;
  if (controlCount>0)
    m |= Qt::ControlModifier;
  if (altCount>0)
    m |= Qt::AltModifier;
  return m;
}
 
void ModSnooper::emitMC() {
  emit modifiersChanged(keyboardModifiers());
}
