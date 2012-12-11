// FrontScene.C

#include "FrontScene.H"

#include "BookData.H"
#include "Notebook.H"
#include "Style.H"

#include <math.h>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QCursor>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QPainter>
#include <QTextDocument>

FrontScene::FrontScene(Notebook *book, QObject *parent):
  QGraphicsScene(parent),
  book(book), style(book->style()) {
  makeBackground();
  makeItems();
  rebuild();
  if (book->bookData()->isRecent())
    makeWritable();
}

FrontScene::~FrontScene() {
}

void FrontScene::makeWritable() {
  QList<QGraphicsTextItem *> ll;
  ll << title << author << address;
  foreach (QGraphicsTextItem *ti, ll) {
    ti->setTextInteractionFlags(Qt::TextEditorInteraction);
    ti->setCursor(QCursor(Qt::IBeamCursor));
    ti->setFlag(QGraphicsTextItem::ItemIsFocusable);
    connect(ti->document(), SIGNAL(contentsChange(int, int, int)),
	    this, SLOT(textChange()));
  }
}

void FrontScene::textChange() {
  positionItems();
  BookData *data = book->bookData();
  data->setTitle(title->toPlainText());
  data->setAuthor(author->toPlainText());
  data->setAddress(address->toPlainText());
}

void FrontScene::rebuild() {
  BookData *data = book->bookData();
  title->setPlainText(data->title());
  author->setPlainText(data->author());
  address->setPlainText(data->address());
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
  bg = 0;
  
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

  QImage img(book->filePath("front.jpg"));
  if (!img.isNull()) {
    if (style.contains("front-recolor")) {
      qDebug() << "Recolor";
      QColor c(style.color("front-recolor"));
      unsigned char lookup[3][256];
      qreal cc[3]; c.getRgbF(cc, cc+1, cc+2);
      for (int k=0; k<3; k++) {
        cc[k] = pow(2.0, -4*(cc[k]-.5));
        for (int i=0; i<256; i++) {
          lookup[k][i] = (unsigned char)(255.99*pow((i/255.), cc[k]));
        }
      }

      img = img.convertToFormat(QImage::Format_ARGB32);
      int X = img.width();
      int Y = img.height();
      for (int y=0; y<Y; y++) {
        uchar *ptr = img.scanLine(y);
        for (int x=0; x<X; x++) {
          for (int k=0; k<3; k++) 
            ptr[k] = lookup[k][ptr[k]];
          ptr+=4;
        }
      }
    }
            
    bg = new QGraphicsPixmapItem();
    bg->setPixmap(QPixmap::fromImage(img));
    addItem(bg);
    QTransform t;
    t.scale(style.real("page-width")/img.width(),
	    style.real("page-height")/img.height());
    bg->setTransform(t);
  }
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
  item->setPos(QPointF(x-x0, y-y0));
}
  
void FrontScene::positionItems() {
  double xc = style.real("page-width") / 2;

  title->setTextWidth(-1);
  title->setTextWidth(title->boundingRect().width());
  QTextBlockFormat format;
  format.setAlignment(Qt::AlignHCenter);
  QTextCursor c0 = title->textCursor();
  QTextCursor cursor = title->textCursor();
  cursor.select(QTextCursor::Document);
  cursor.mergeBlockFormat(format);
  title->setTextCursor(cursor);
  title->setTextCursor(c0);
  
  centerAt(title, xc, style.real("front-title-y"));
  centerAt(author, xc, style.real("front-author-y"));
  centerAt(address, xc, style.real("front-address-y"));
  centerAt(dates, xc, style.real("front-dates-y"));
}

void FrontScene::print(QPrinter *, QPainter *p) {
  render(p);
}
