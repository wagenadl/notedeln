// SimpleNavbar.C

#include "SimpleNavbar.H"
#include "ToolItem.H"
#include "Assert.H"

#include <QGraphicsScene>

#define NAV_PREV "prev"
#define NAV_NEXT "next"

SimpleNavbar::SimpleNavbar(QGraphicsScene *scene): Toolbar(0) {
  //  setOrientation(Qt::Horizontal);
  disableSelect();
  
  ToolItem *t = new ToolItem();
  t->setSvg(":icons/nav-prev.svg");
  addTool(NAV_PREV, t);

  t = new ToolItem();
  t->setSvg(":icons/nav-next.svg");
  addTool(NAV_NEXT, t);

  ASSERT(scene);
  scene->addItem(this);
  setPos(0,
	 scene->sceneRect().height() - childrenBoundingRect().height() - 75);
  setZValue(-1);
}


SimpleNavbar::~SimpleNavbar() {
}

void SimpleNavbar::doLeftClick(QString s) {
  if (s==NAV_PREV)
    emit goRelative(-1);
  else if (s==NAV_NEXT)
    emit goRelative(1);
}
