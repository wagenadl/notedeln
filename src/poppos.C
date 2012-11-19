
void poppos() {
  // below
  d = p + QPoint(-s.width()/3, dy);
  if (d.x()+s.width()>r.right())
    d.setX(r.right()-s.width());
  if (d.x()<r.left())
    d.setX(r.left());
  ir = QRectF(d, s).intersected(r);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestD = d;
    bestArea = area;
  }

  // above
  d = p + QPoint(-s.width()/3, -dy-s.height());
  if (d.x()+s.width()>r.right())
    d.setX(r.right()-s.width());
  if (d.x()<r.left())
    d.setX(r.left());
  ir = QRectF(d, s).intersected(r);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestD = d;
    beestArea = area;
  }

  // to the right
  d = p + QPoint(dx, -s.height()/3);
  if (d.y()+s.height()>r.bottom())
    d.setY(r.bottom()-s.height());
  if (d.y()<r.top())
    d.setY(r.top());
  ir = QRectF(d, s).intersected(r);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestD = d;
    bestArea = area;
  }

  // to the left
  d = p + QPoint(-dx-s.width(), -s.height()/3);
  if (d.y()+s.height()>r.bottom())
    d.setY(r.bottom()-s.height());
  if (d.y()<r.top())
    d.setY(r.top());
  ir = QRectF(d, s).intersected(r);
  area = ir.width()*ir.height();
  if (area>bestArea) {
    bestD = d;
    bestArea = area;
  }
}

void dedecorate() {
  QWidget *w = new QWidget(0,
			   Qt::FramelessWindowHint); // | Qt::WindowSystemMenuHint);
}

void URLRegion::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
  if (popper) 
    popper->popup();
  else
    popper = new PreviewPopper(resMgr(), refText(), this);
}

void URLRegion::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
  if (popper) 
    delete popper;
  popper = 0;
}
  
