// SplashScene.C

#include "SplashScene.H"
#include "SplashView.H"
#include "Notebook.H"
#include "BookSplashItem.H"
#include "Style.H"

#include <QFileDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsView>
#include <QDebug>
#include <QDir>
#include <QEventLoop>

SplashScene::SplashScene(QObject *parent):
  QGraphicsScene(parent) {
  newRequested = false;
  openRequested = false;
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
  Style const &style(Style::defaultStyle());
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

  QImage img(":/splash.jpg");
  QGraphicsPixmapItem *bg = new QGraphicsPixmapItem();
  bg->setPixmap(QPixmap::fromImage(img));
  addItem(bg);
  QTransform t;
  t.scale(style.real("page-width")/img.width(),
	  style.real("page-height")/img.height());
  bg->setTransform(t);

  QGraphicsTextItem *eln = addText("ELN", style.font("splash-font"));
  centerAt(eln, style.real("page-width")/2, style.real("page-height") - 50);
  QGraphicsTextItem *dw = addText("(C) Daniel Wagenaar 2013",
                                  style.font("splash-small-font"));
  centerAt(dw, style.real("page-width")/2, style.real("page-height") - 20);

}

void SplashScene::makeItems() {
  // add clickable items for recent files, local files, new file, arb. file
  double y = 25;
  double x = 45;
  double DY = 90;
  BookSplashItem *bsi = new BookSplashItem("Open existing notebook...");
  connect(bsi, SIGNAL(leftClick(QString)), SLOT(openExisting()));
  addItem(bsi);
  bsi->setPos(x, y);
  y += DY;

  bsi = new BookSplashItem("Create new notebook...");
  connect(bsi, SIGNAL(leftClick(QString)), SLOT(createNew()));
  addItem(bsi);
  bsi->setPos(x, y);
  y += DY;

  QStringList dirs = localNotebooks();
  QSet<QString> absdirs;
  foreach (QString d, dirs)
    absdirs << QDir::cleanPath(QDir::current().absoluteFilePath(d));

  QStringList recent = RecentBooks::instance()->byDate();
  while (dirs.size()<10 && !recent.isEmpty()) {
    QString r = recent.takeFirst();
    QString r0 = QDir::cleanPath(r);
    if (!absdirs.contains(r)) {
      dirs << r;
      absdirs << r0;
    }
  }

  foreach (QString d, dirs) {
    if (RecentBooks::instance()->contains(d))
      bsi = new BookSplashItem(d, (*RecentBooks::instance())[d]);
    else
      bsi = new BookSplashItem(d, BookInfo(d));
    connect(bsi, SIGNAL(leftClick(QString)), SLOT(openNamed(QString)));
    addItem(bsi);
    bsi->setPos(x, y);
    y += DY;
    if (y+DY >= sceneRect().height())
      break;
  }  
}

QStringList SplashScene::localNotebooks() {
  QDir d(QDir::current());
  QStringList flt; flt << "*.nb";
  return d.entryList(flt, QDir::Dirs, QDir::Name);
}

void SplashScene::createNew() {
  newRequested = true;
  emit done();
}

void SplashScene::openExisting() {
  openRequested = true;
  emit done();
}

void SplashScene::openNamed(QString dir) {
  named = dir;
  emit done();
}

Notebook *SplashScene::openNotebook() {
  SplashScene *ss = new SplashScene();
  QEventLoop el;
  QObject::connect(ss, SIGNAL(done()),
                   &el, SLOT(quit()),
                   Qt::QueuedConnection);

  SplashView *gv = new SplashView();
  gv->setScene(ss);
  connect(gv, SIGNAL(closing()), &el, SLOT(quit()));
  gv->show();
  
  QSizeF size = gv->sizeHint();
  double dpiX = QApplication::desktop()->logicalDpiX();
  double dpiY = QApplication::desktop()->logicalDpiY();
  gv->resize(size.width()*dpiX/72, size.height()*dpiY/72);
 
  el.exec();

  delete gv;

  Notebook *nb = 0;
  if (ss->newRequested) {
    QString fn = QFileDialog::getSaveFileName(0, "Create new notebook...",
                                              "",
                                              "Notebooks (*.nb)");
    if (!fn.isEmpty() && !QDir::current().exists(fn)) 
      nb = Notebook::create(fn);
  } else if (ss->openRequested) {
    QString fn = QFileDialog::getExistingDirectory(0,
                                                   "Open existing notebook...");
    qDebug() << fn;
    if (!fn.isEmpty() && QDir(fn).exists("book.json"))
      nb = Notebook::load(fn);
  } else if (!ss->named.isEmpty()) {
    nb = Notebook::load(ss->named);
  }
  
  delete ss;
  
  return nb;
}
