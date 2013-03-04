// Toolbar.C

#include "Toolbar.H"
#include <QDebug>
#include "Assert.H"
#include "ToolItem.H"

#define TOOLGRID 31.0
#define TOOLOFFSET 2

Toolbar::Toolbar(QGraphicsItem *parent): QGraphicsObject(parent) {
  orient = Qt::Vertical;
}

Toolbar::~Toolbar() {
}
Qt::Orientation Toolbar::orientation() const {
  return orient;
}

void Toolbar::setOrientation(Qt::Orientation o) {
  orient = o;
  arrangeTools();
}

void Toolbar::addTool(QString id, ToolItem *item) {
  ASSERT(!tools.contains(id));
  item->setParentItem(this);
  tools[id] = item;
  revmap[item] = id;
  ids.append(id);
  connect(item, SIGNAL(destroyed(QObject*)), SLOT(childGone()));
  connect(item, SIGNAL(leftClick()), SLOT(leftClicked()));
  connect(item, SIGNAL(rightClick()), SLOT(rightClicked()));
  arrangeTools();
}
    
QString Toolbar::selection() const {
  return sel;
}

void Toolbar::select(QString s) {
  if (sel==s)
    return;
  if (!sel.isEmpty())
    tools[sel]->setSelected(false);

  if (tools.contains(s)) {
    sel = s;
    tools[sel]->setSelected(true);
  } else {
    sel = "";
    qDebug() << "Toolbar: selecting nonexistent tool";
  }
   
  emit selectionChanged(sel);
}

void Toolbar::leftClicked() {
  ToolItem *t = dynamic_cast<ToolItem*>(sender());
  if (t && revmap.contains(t)) {
    QString id = revmap[t];
    doLeftClick(id);
    select(id);
  } else {
    qDebug() << "Toolbar: left click on unknown tool";
  }
}

void Toolbar::rightClicked() {
  ToolItem *t = dynamic_cast<ToolItem*>(sender());
  if (t && revmap.contains(t)) {
    QString id = revmap[t];
    doRightClick(id);
    emit rightClick(id);
  } else {
    qDebug() << "Toolbar: right click on unknown tool";
  }
}
void Toolbar::childGone() {
  ToolItem *t = dynamic_cast<ToolItem*>(sender());
  if (t && revmap.contains(t)) {
    QString id = revmap[t];
    if (sel==id)
      sel="";
    revmap.remove(t);
    tools.remove(id);
    ids.removeOne(id);
    arrangeTools();
  }
}

void Toolbar::arrangeTools() {
  double y = TOOLOFFSET;
  double x = TOOLOFFSET;
  foreach (QString id, ids) {
    tools[id]->setPos(x, y);
    if (orient==Qt::Horizontal)
      x += TOOLGRID;
    else
      y += TOOLGRID;
  }
}

QRectF Toolbar::boundingRect() const {
  return QRectF();
}

void Toolbar::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) {
}

void Toolbar::doLeftClick(QString) {
}

void Toolbar::doRightClick(QString) {
}

