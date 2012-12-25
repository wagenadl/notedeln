// Modebar.C

#include "Modebar.H"
#include "ToolItem.H"
#include "Mode.H"
#include "MarkSizeItem.H"
#include "LineWidthItem.H"

Modebar::Modebar(Mode *mode, QGraphicsItem *parent):
  Toolbar(parent), mode(mode) {
  ToolItem *t = new ToolItem();
  t->setSvg(":icons/browse.svg");
  addTool(modeToId(Mode::Browse), t);

  t = new ToolItem();
  t->setSvg(":icons/type.svg");
  addTool(modeToId(Mode::Type), t);

  t = new ToolItem();
  t->setSvg(":icons/move.svg");
  addTool(modeToId(Mode::MoveResize), t);

  MarkSizeItem *mst = new MarkSizeItem(mode->markSize());
  mst->setShape(mode->shape());
  mst->setColor(mode->color());
  addTool(modeToId(Mode::Mark), mst);
  connect(mode, SIGNAL(shapeChanged(GfxMarkData::Shape)),
	  mst, SLOT(setShape(GfxMarkData::Shape)));
  connect(mode, SIGNAL(markSizeChanged(double)),
	  mst, SLOT(setMarkSize(double)));
  connect(mode, SIGNAL(colorChanged(QColor)),
	  mst, SLOT(setColor(QColor)));

  LineWidthItem *lwt = new LineWidthItem(mode->lineWidth());
  connect(mode, SIGNAL(colorChanged(QColor)),
	  lwt, SLOT(setColor(QColor)));
  connect(mode, SIGNAL(lineWidthChanged(double)),
	  lwt, SLOT(setLineWidth(double)));
  addTool(modeToId(Mode::Freehand), lwt);
  
  t = new ToolItem();
  t->setSvg(":icons/note.svg");
  addTool(modeToId(Mode::Annotate), t);

  t = new ToolItem();
  t->setSvg(":icons/highlight.svg");
  addTool(modeToId(Mode::Highlight), t);

  t = new ToolItem();
  t->setSvg(":icons/strikeout.svg");
  addTool(modeToId(Mode::Strikeout), t);

  t = new ToolItem();
  t->setSvg(":icons/plain.svg");
  addTool(modeToId(Mode::Plain), t);
  
  select(modeToId(mode->mode()));
  connect(mode, SIGNAL(modeChanged(Mode::M)), SLOT(updateMode()));
}

Modebar::~Modebar() {
}

void Modebar::updateMode() {
  select(modeToId(mode->mode()));
}

Mode::M Modebar::idToMode(QString s) {
  return Mode::M(s.toInt());
}

QString Modebar::modeToId(Mode::M m) {
  return QString::number(m);
}

void Modebar::doLeftClick(QString id) {
  mode->setMode(idToMode(id));
}
