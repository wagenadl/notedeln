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

#include "Modebar.h"
#include "ToolItem.h"
#include "Mode.h"
#include "MarkSizeItem.h"
#include "LineWidthItem.h"

Modebar::Modebar(Mode *mode, QGraphicsItem *parent):
  Toolbar(parent), mode(mode) {
  ToolItem *t = 0;

  t = new ToolItem();
  t->setBalloonHelpText(":mode-browse");
  t->setSvg(":icons/browse.svg");
  addTool(modeToId(Mode::Browse), t);
  addSpace(4);

  typeModeItem = new ToolItem();
  addTool(modeToId(Mode::Type), typeModeItem);
  updateMath();
  connect(mode, SIGNAL(mathModeChanged(bool)), SLOT(updateMath()));

  t = new ToolItem();
  t->setBalloonHelpText(":mode-move");
  t->setSvg(":icons/move.svg");
  addTool(modeToId(Mode::MoveResize), t);

  MarkSizeItem *mst = new MarkSizeItem(mode->markSize());
  mst->setBalloonHelpText(":mode-mark");
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
  lwt->setBalloonHelpText(":mode-freehand");
  connect(mode, SIGNAL(colorChanged(QColor)),
	  lwt, SLOT(setColor(QColor)));
  connect(mode, SIGNAL(lineWidthChanged(double)),
	  lwt, SLOT(setLineWidth(double)));
  addTool(modeToId(Mode::Freehand), lwt);
  sketchModeItem = lwt;
  updateStraightLine();
  connect(mode, SIGNAL(straightLineModeChanged(bool)),
	  SLOT(updateStraightLine()));
  
  t = new ToolItem();
  t->setBalloonHelpText(":mode-annotate");
  t->setSvg(":icons/note.svg");
  addTool(modeToId(Mode::Annotate), t);

  t = new ToolItem();
  t->setBalloonHelpText(":mode-highlight");
  t->setSvg(":icons/highlight.svg");
  addTool(modeToId(Mode::Highlight), t);

  t = new ToolItem();
  t->setBalloonHelpText(":mode-strikeout");
  t->setSvg(":icons/strikeout.svg");
  addTool(modeToId(Mode::Strikeout), t);

  t = new ToolItem();
  t->setBalloonHelpText(":mode-plain");
  t->setSvg(":icons/plain.svg");
  addTool(modeToId(Mode::Plain), t);

  select(modeToId(mode->mode()));
  connect(mode, SIGNAL(modeChanged(Mode::M)), SLOT(updateMode()));

  //  t0->setPos(t0->pos().x(), t0->pos().y()-4);
}

Modebar::~Modebar() {
}

void Modebar::updateMode() {
  select(modeToId(mode->mode()));
}

void Modebar::updateMath() {
  if (mode->isMathMode()) {
    typeModeItem->setBalloonHelpText(":mode-type-math");
    typeModeItem->setSvg(":icons/type-math.svg");
  } else {
    typeModeItem->setBalloonHelpText(":mode-type");
    typeModeItem->setSvg(":icons/type.svg");
  }
}

void Modebar::updateStraightLine() {
  bool m = mode->isStraightLineMode();
  if (m)
    sketchModeItem->setBalloonHelpText(":mode-straightline");
  else
    sketchModeItem->setBalloonHelpText(":mode-freehand");
  sketchModeItem->setStraightLineMode(m);
}


Mode::M Modebar::idToMode(QString s) {
  return Mode::M(s.toInt());
}

QString Modebar::modeToId(Mode::M m) {
  return QString::number(m);
}

void Modebar::doLeftClick(QString id, Qt::KeyboardModifiers m) {
  mode->setMode(idToMode(id));
  if (idToMode(id)==Mode::Type && (m & Qt::ShiftModifier)) 
    mode->setMathMode(!mode->isMathMode());
  else if (idToMode(id)==Mode::Freehand && (m & Qt::ShiftModifier)) 
    mode->setStraightLineMode(!mode->isStraightLineMode());

}
