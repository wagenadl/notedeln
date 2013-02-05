// SplashScene.C

#include "SplashScene.H"

SplashScene::SplashScene(QObject *parent):
  QGraphicsScene(parent) {
  makeBackground();
  makeItems();
}

SplashScene::~SplashScene() {
}

static void centerAt(QGraphicsItem *item, double x, double y) {
  QRectF bb = item->boundingRect();
  double x0 = (bb.left() + bb.right()) / 2;
  double y0 = (bb.top() + bb.bottom()) / 2;
  item->setPos(QPointF(x-x0, y-y0));
}

void SplashScene::makeBackground() {
  Style const &style = Style::defaultStyle();
  setSceneRect(0,
	       0,
	       style.real("page-width"),
	       style.real("page-height"));
  
  setBackgroundBrush(QBrush(style.color("border-color")));
  
  addRect(0,
	  0,
	  style.real("page-width"),
	  style.real("page-height"),
	  QPen(Qt::NoPen),
	  QBrush(style.color("background-color")));

  QImage img(":/front.jpg");
  QGraphicsPixmapItem *bg = new QGraphicsPixmapItem();
  bg->setPixmap(QPixmap::fromImage(img));
  addItem(bg);
  QTransform t;
  t.scale(style.real("page-width")/img.width(),
	  style.real("page-height")/img.height());
  bg->setTransform(t);

  QGraphicsRectItem *rc
    = new QGraphicsRectItem(QRectF(.1*style.real("page-width"),
				   .1*style.real("page-width"),
				   .8*style.real("page-width"),
				   style.real("page-height")
				   -.2*style.real("page-width")));
  addItem(rc);
  
  QFont f = style.font("front-title-font");
  f.setPointSize(48);
  QGraphicsTextItem *title = addText("ELN", f);
  centerAt(title, style.real("page-width")/2, 64);

  // add clickable items for recent files, local files, new file, arb. file
}

