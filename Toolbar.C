// Toolbar.C

#include "Toolbar.H"
#include "Icons.H"
#include <QDebug>

Toolbar::Toolbar(QWidget *parent): QToolBar(parent) {
  add(Act_Type, "type", "Type");
  add(Act_Canvas, "canvas", "Canvas");
  add(Act_Line, "line", "Line");
  add(Act_Arrow, "arrow", "Arrow");
  add(Act_Emph, "emph", "Emphasize");
  add(Act_Strikeout, "strikeout", "Strikeout");
  add(Act_Plain, "plain", "Plain text");
  add(Act_InsertImage, "insert-image", "Insert image");
  add(Act_Screenshot, "camera-photo", "Insert screenshot");
}

Toolbar::~Toolbar() {
}

void Toolbar::add(Toolbar::Action a, QString n, QString l) {
  actions[a] = addAction(Icons::icons().find(n), l);
}

QAction *Toolbar::action(Toolbar::Action a) {
  if (actions.contains(a))
    return actions[a];
  else {
    qDebug() << "Toolbar: No action for " << a;
    return 0;
  }
}
