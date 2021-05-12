// App/Modebar.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// Modebar.C

#include "Modebar.h"
#include "ToolItem.h"
#include "Mode.h"
#include "MarkSizeItem.h"
#include "LineWidthItem.h"
/*
#include <QPainter>
#include <QImage>
*/

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
  updateTypeMode();
  connect(mode, &Mode::typeModeChanged, this, &Modebar::updateTypeMode);

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

  /*
  QImage test(64, 64, QImage::Format_RGB32);
  test.fill(QColor(255,255,255));
  QPainter p(&test);
  p.scale(2, 2);
  lwt->paintContents(&p);
  test.save("/tmp/squiggle.png");
  test.fill(QColor(255,255,255));
  lwt->setStraightLineMode(true);
  lwt->paintContents(&p);
  test.save("/tmp/straight.png");
  lwt->setStraightLineMode(false);
  test.fill(QColor(255,255,255));
  lwt->setStraightLineMode(true);
  mst->paintContents(&p);
  test.save("/tmp/mark.png");
  */
    
  lwt->setBalloonHelpText(":mode-freehand");
  connect(mode, SIGNAL(colorChanged(QColor)),
	  lwt, SLOT(setColor(QColor)));
  connect(mode, SIGNAL(lineWidthChanged(double)),
	  lwt, SLOT(setLineWidth(double)));
  addTool(modeToId(Mode::Draw), lwt);
  sketchModeItem = lwt;
  updateDrawMode();
  connect(mode, &Mode::drawModeChanged,
	  this, &Modebar::updateDrawMode);
  
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
}

Modebar::~Modebar() {
}

void Modebar::updateMode() {
  select(modeToId(mode->mode()));
}

void Modebar::updateTypeMode() {
  switch (mode->typeMode()) {
  case Mode::Math:
    typeModeItem->setBalloonHelpText(":mode-type-math");
    typeModeItem->setSvg(":icons/type-math.svg");
    break;
  case Mode::Code:
    typeModeItem->setBalloonHelpText(":mode-type-code");
    typeModeItem->setSvg(":icons/type-code.svg");
    break;
  case Mode::Normal:
    typeModeItem->setBalloonHelpText(":mode-type");
    typeModeItem->setSvg(":icons/type.svg");
    break;
  }
}

void Modebar::updateDrawMode() {
  Mode::DrawM m = mode->drawMode();
  if (m==Mode::Straightline)
    sketchModeItem->setBalloonHelpText(":mode-straightline");
  else
    sketchModeItem->setBalloonHelpText(":mode-freehand");
  sketchModeItem->setStraightLineMode(m==Mode::Straightline);
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
    mode->setTypeMode(mode->typeMode()==Mode::Math
		      ? Mode::Normal : Mode::Math);
  else if (idToMode(id)==Mode::Type && (m & Qt::ControlModifier)) 
    mode->setTypeMode(mode->typeMode()==Mode::Code
		      ? Mode::Normal : Mode::Code);
  else if (idToMode(id)==Mode::Draw && (m & Qt::ShiftModifier)) 
    mode->setDrawMode(mode->drawMode()==Mode::Straightline
		      ? Mode::Freehand : Mode::Straightline);
}

void Modebar::doubleClicked(Qt::KeyboardModifiers m) {
  m |= Qt::ShiftModifier;
  leftClicked(m);
}
