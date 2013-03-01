// FrontScene.C

#include "FrontScene.H"

#include "BookData.H"
#include "Notebook.H"
#include "Style.H"
#include "RoundedRect.H"

#include <math.h>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QCursor>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QPainter>
#include <QTextDocument>
#include <QGraphicsView>

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
  foreach (QGraphicsView *view, views()) {
    QWidget *toplevel = view->window();
    if (toplevel)
      toplevel->setWindowTitle(title->toPlainText()
			       .replace(QRegExp("\\s\\s*"), " ") + " - eln");
  }
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
		   + QString::fromUtf8("‒") +
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
  bool customFront = !img.isNull();
  if (!customFront)
    img = QImage(":/front.jpg");
  
  if (!img.isNull()) {
    if (style.contains("front-recolor")) {
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

  if (customFront) {
    toprect = 0;
    bottomrect = 0;
  } else {
    toprect = new RoundedRect();
    addItem(toprect);
    bottomrect = new RoundedRect();
    addItem(bottomrect);
  }
}

void FrontScene::makeItems() {
  title = addText("title", style.font("front-title-font"));
  title->setDefaultTextColor(style.color("front-title-color"));
  
  author = addText("author", style.font("front-author-font"));
  author->setDefaultTextColor(style.color("front-author-color"));

  address = addText("address", style.font("front-address-font"));
  address->setDefaultTextColor(style.color("front-address-color"));

  dates = addText("dates", style.font("front-dates-font"));
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

  if (toprect) {
    QRectF tr = title->sceneBoundingRect();
    tr |= dates->sceneBoundingRect();
    tr.adjust(-36, -18, 36, 18); // to be improved
    toprect->setPos(tr.topLeft());
    toprect->resize(tr.size());
  }

  if (bottomrect) {
    QRectF br = author->sceneBoundingRect();
    br |= address->sceneBoundingRect();
    br.adjust(-36, -18, 36, 18); // to be improved
    bottomrect->setPos(br.topLeft());
    bottomrect->resize(br.size());
  } 
}

void FrontScene::print(QPrinter *, QPainter *p) {
  render(p);
}
