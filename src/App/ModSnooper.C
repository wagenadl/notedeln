// ModSnooper.C

#include "ModSnooper.H"
#include <QDebug>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#define XK_MISCELLANY
#define XK_XKB_KEYS
#include <X11/keysymdef.h>

ModSnooper::ModSnooper(int argc, char **argv): QApplication(argc, argv) {
  shiftCount = 0;
  controlCount = 0;
  altCount = 0;
}

ModSnooper::~ModSnooper() {
}

bool ModSnooper::haveState() const {
  return true;
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

bool ModSnooper::x11EventFilter(XEvent *event) {
  if (event->type==KeyPress || event->type==KeyRelease) {
    XKeyEvent *e = (XKeyEvent*)event;
    int delta = event->type==KeyPress ? 1 : -1;
    int sym = XkbKeycodeToKeysym(e->display, e->keycode, 0, 0);
    switch (sym) {
    case XK_Shift_L: case XK_Shift_R:
      shiftCount += delta;
      if (shiftCount<0)
	shiftCount=0;
      break;
    case XK_Control_L: case XK_Control_R:
       controlCount += delta;
      if (controlCount<0)
	controlCount=0;
      break;
    case XK_Meta_L: case XK_Meta_R:
    case XK_Alt_L: case XK_Alt_R:
    case XK_ISO_Level3_Shift: // this is on my right alt...
      altCount += delta;
      if (altCount<0)
	altCount=0;
      break;
    }
  }
  return false;
}

      
ModSnooper *ModSnooper::instance() {
  ModSnooper *ms = dynamic_cast<ModSnooper*>(QApplication::instance());
  return ms;
}
