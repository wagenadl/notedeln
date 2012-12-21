// Toolbars.C

#include "Toolbars.H"
#include "Modebar.H"
#include "LineWidthBar.H"
#include "MarkSizeBar.H"
#include "MarkShapeBar.H"
#include "ColorBar.H"

#include <QDebug>

#define CORRESPONDING 1

Toolbars::Toolbars(Mode *mode, QGraphicsItem *parent): QGraphicsObject(parent) {
  orient = Qt::Vertical;

  modes = new Modebar(mode, this);

  mcolors = new ColorBar(mode, false, this);
#if CORRESPONDING
  connect(mode, SIGNAL(markSizeChanged(double)),
	  mcolors, SLOT(setMarkSize(double)));
  connect(mode, SIGNAL(shapeChanged(GfxMarkData::Shape)),
	  mcolors, SLOT(setShape(GfxMarkData::Shape)));
  connect(mode, SIGNAL(colorChanged(QColor)),
	  mcolors, SLOT(setColor(QColor)));
#endif

  lcolors = new ColorBar(mode, true, this);
#if CORRESPONDING
  connect(mode, SIGNAL(lineWidthChanged(double)),
	  lcolors, SLOT(setLineWidth(double)));
  connect(mode, SIGNAL(colorChanged(QColor)),
	  lcolors, SLOT(setColor(QColor)));
#endif
  
  shapes = new MarkShapeBar(mode, this);
#if CORRESPONDING
  connect(mode, SIGNAL(colorChanged(QColor)),
		   shapes, SLOT(setColor(QColor)));
  connect(mode, SIGNAL(markSizeChanged(double)),
		   shapes, SLOT(setMarkSize(double)));
#endif

  sizes = new MarkSizeBar(mode, this);
#if CORRESPONDING
  connect(mode, SIGNAL(colorChanged(QColor)),
	  sizes, SLOT(setColor(QColor)));
  connect(mode, SIGNAL(shapeChanged(GfxMarkData::Shape)),
	  sizes, SLOT(setShape(GfxMarkData::Shape)));
#endif
  
  widths = new LineWidthBar(mode, this);
#if CORRESPONDING
  connect(mode, SIGNAL(colorChanged(QColor)),
	  widths, SLOT(setColor(QColor)));
#endif

  placeChildren();
  
  connect(mode, SIGNAL(modeChanged(Mode::M)),
	  SLOT(setMode(Mode::M)));
  setMode(mode->mode());
}

Toolbars::~Toolbars() {
}

void Toolbars::setOrientation(Qt::Orientation o) {
  orient = o;
  modes->setOrientation(o);
  lcolors->setOrientation(o);
  mcolors->setOrientation(o);
  shapes->setOrientation(o);
  sizes->setOrientation(o);
  widths->setOrientation(o);
  placeChildren();
}

Qt::Orientation Toolbars::orientation() const {
  return orient;
}

void Toolbars::placeChildren() {
  if (orient==Qt::Horizontal)
    qDebug() << "Toolbars: Horizontal not yet supported";
  
  modes->setPos(0, 72);

  lcolors->setPos(modes->pos()
		 + QPointF(0, modes->childrenBoundingRect().height()+10));
  mcolors->setPos(modes->pos()
		 + QPointF(0, modes->childrenBoundingRect().height()+10));
  
  sizes->setPos(mcolors->pos() + QPointF(36, 0));
  shapes->setPos(sizes->pos()
		+ QPointF(0, sizes->childrenBoundingRect().height()+10));

  widths->setPos(lcolors->pos() + QPointF(36, 0));
}

void Toolbars::setMode(Mode::M m) {
  if (m==Mode::Mark) {
    shapes->show();
    sizes->show();
    mcolors->show();
  } else {
    shapes->hide();
    sizes->hide();
    mcolors->hide();
  }

  if (m==Mode::Freehand) {
    widths->show();
    lcolors->show();
  } else {
    widths->hide();
    lcolors->hide();
  }
}
      
