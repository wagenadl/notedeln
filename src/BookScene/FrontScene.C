// FrontScene.C

#include "FrontScene.H"

#include "BookData.H"
#include "Notebook.H"
#include "Style.H"

#include <QGraphicsTextItem>
#include <QDebug>

FrontScene::FrontScene(Notebook *book, QObject *parent):
  QGraphicsScene(parent),
  book(book), style(book->style()) {
  makeBackground();
  makeItems();
  rebuild();
}

FrontScene::~FrontScene() {
}

void FrontScene::rebuild() {
  BookData *data = book->bookData();
  title->setPlainText(data->title());
  author->setPlainText("DAW");
  address->setPlainText("Caltech");
  QString dateFmt = style.string("front-date-format");
  if (data->startDate() == data->endDate())
    dates->setHtml(data->startDate().toString(dateFmt));
  else
    dates->setHtml(data->startDate().toString(dateFmt)
		   + "&ndash;" +
		   data->endDate().toString(dateFmt));
  positionItems();
}

void FrontScene::makeBackground() {
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
}

void FrontScene::makeItems() {
  title = addText("title", QFont(style.string("front-font-family"),
				 style.real("front-title-font-size")));
  title->setDefaultTextColor(style.color("front-title-color"));

  author = addText("author", QFont(style.string("front-font-family"),
				 style.real("front-author-font-size")));
  author->setDefaultTextColor(style.color("front-author-color"));

  address = addText("address", QFont(style.string("front-font-family"),
				 style.real("front-address-font-size")));
  address->setDefaultTextColor(style.color("front-address-color"));

  dates = addText("dates", QFont(style.string("front-font-family"),
				 style.real("front-dates-font-size")));
  dates->setDefaultTextColor(style.color("front-dates-color"));
}

static void centerAt(QGraphicsItem *item, double x, double y) {
  QRectF bb = item->boundingRect();
  double x0 = (bb.left() + bb.right()) / 2;
  double y0 = (bb.top() + bb.bottom()) / 2;
  item->setPos(item->pos() + QPointF(x-x0, y-y0));
}
  
void FrontScene::positionItems() {
  double xc = style.real("page-width") / 2;

  centerAt(title, xc, style.real("front-title-y"));
  centerAt(author, xc, style.real("front-author-y"));
  centerAt(address, xc, style.real("front-address-y"));
  centerAt(dates, xc, style.real("front-dates-y"));
}
