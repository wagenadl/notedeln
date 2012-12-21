// Mode.C

#include "Mode.H"

Mode::Mode(QObject *parent): QObject(parent) {
  m = Browse;
}

Mode::~Mode() {
}

Mode::M Mode::mode() const {
  return m;
}

void Mode::setMode(Mode::M m1) {
  m = m1;
  emit modeChanged(m);
}

