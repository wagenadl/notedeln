// Navbar.C

#include "Navbar.H"
#include "ToolItem.H"

#define NAV_TOC "toc"
#define NAV_PREV "prev"
#define NAV_NEXT "next"
#define NAV_P10 "p10"
#define NAV_N10 "n10"
#define NAV_END "end"

Navbar::Navbar(QGraphicsItem *parent): Toolbar(parent) {
  disableSelect();
  
  ToolItem *t = new ToolItem();
  t->setSvg(":icons/nav-toc.svg");
  addTool(NAV_TOC, t);

  t = new ToolItem();
  t->setSvg(":icons/nav-p10.svg");
  addTool(NAV_P10, t);

  t = new ToolItem();
  t->setSvg(":icons/nav-prev.svg");
  addTool(NAV_PREV, t);

  t = new ToolItem();
  t->setSvg(":icons/nav-next.svg");
  addTool(NAV_NEXT, t);

  t = new ToolItem();
  t->setSvg(":icons/nav-n10.svg");
  addTool(NAV_N10, t);

  t = new ToolItem();
  t->setSvg(":icons/nav-end.svg");
  addTool(NAV_END, t);
}


Navbar::~Navbar() {
}

void Navbar::doLeftClick(QString s) {
  if (s==NAV_TOC)
    emit goTOC();
  else if (s==NAV_P10)
    emit goRelative(-10);
  else if (s==NAV_PREV)
    emit goRelative(-1);
  else if (s==NAV_NEXT)
    emit goRelative(1);
  else if (s==NAV_N10)
    emit goRelative(10);
  else if (s==NAV_END)
    emit goEnd();
}
