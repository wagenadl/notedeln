// App/Modebar.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// Modebar.C

#include "Modebar.H"
#include "ToolItem.H"
#include "Mode.H"
#include "MarkSizeItem.H"
#include "LineWidthItem.H"

Modebar::Modebar(Mode *mode, QGraphicsItem *parent):
  Toolbar(parent), mode(mode) {
  ToolItem *t0 = new ToolItem();
  t0->setBalloonHelpText("In Browse mode, single clicks follow links and content cannot be edited.");
  t0->setSvg(":icons/browse.svg");
  addTool(modeToId(Mode::Browse), t0);

  ToolItem *t = new ToolItem();
  t->setBalloonHelpText("In Type mode, text can be edited.");
  t->setSvg(":icons/type.svg");
  addTool(modeToId(Mode::Type), t);

  t = new ToolItem();
  t->setBalloonHelpText("In Move mode, items on a graphics canvas can be moved around or resized by dragging.");
  t->setSvg(":icons/move.svg");
  addTool(modeToId(Mode::MoveResize), t);

  MarkSizeItem *mst = new MarkSizeItem(mode->markSize());
  mst->setBalloonHelpText("In Mark mode, marks of various sizes and shapes can be added to graphics canvases.");
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
  lwt->setBalloonHelpText("In Line mode, freehand lines can be drawn on graphics canvases.");
  connect(mode, SIGNAL(colorChanged(QColor)),
	  lwt, SLOT(setColor(QColor)));
  connect(mode, SIGNAL(lineWidthChanged(double)),
	  lwt, SLOT(setLineWidth(double)));
  addTool(modeToId(Mode::Freehand), lwt);
  
  t = new ToolItem();
  t->setBalloonHelpText("In Annotate mode, new notes can be added at arbitrary positions on the page. (Edit existing notes using Type mode.)");
t->setSvg(":icons/note.svg");
  addTool(modeToId(Mode::Annotate), t);

  t = new ToolItem();
  t->setBalloonHelpText("In Highlight mode, dragging the mouse over existing text causes it to be highlighted.");
  t->setSvg(":icons/highlight.svg");
  addTool(modeToId(Mode::Highlight), t);

  t = new ToolItem();
  t->setBalloonHelpText("In Strike out mode, dragging the mouse over existing text causes it to be struck out.");
  t->setSvg(":icons/strikeout.svg");
  addTool(modeToId(Mode::Strikeout), t);

  t = new ToolItem();
  t->setBalloonHelpText("In Cleanup mode, dragging the mouse over existing text removes highlighting and strike outs.");
  t->setSvg(":icons/plain.svg");
  addTool(modeToId(Mode::Plain), t);

  t = new ToolItem();
  t->setBalloonHelpText("In Table mode, clicking on the page creates a new table. (Tables can be edited in Type mode. Use Tab and Enter in combination with Shift and Control to navigate tables.)");
  t->setSvg(":icons/table.svg");
  addTool(modeToId(Mode::Table), t);

  select(modeToId(mode->mode()));
  connect(mode, SIGNAL(modeChanged(Mode::M)), SLOT(updateMode()));

  t0->setPos(t0->pos().x(), t0->pos().y()-4);
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
