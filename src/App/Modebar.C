// Modebar.C

#include "Modebar.H"
#include "ToolItem.H"

Modebar::Modebar(Mode *mode, QGraphicsView *view): Toolbar(view), mode(mode) {
  ToolItem *t = new ToolItem();
  t->setSvg(":icons/browse.svg");
  addTool(modeToString(Mode::Browse), t);

  t = new ToolItem();
  t->setSvg(":icons/type.svg");
  addTool(modeToString(Mode::Type), t);

  select(modeToString(mode->mode()));
}

Modebar::~Modebar() {
}

Mode::M Modebar::stringToMode(QString s) {
  return Mode::M(s.toInt());
}

QString Modebar::modeToString(Mode::M m) {
  return QString::number(m);
}

void Modebar::doLeftClick(QString id) {
  mode->setMode(stringToMode(id));
}
