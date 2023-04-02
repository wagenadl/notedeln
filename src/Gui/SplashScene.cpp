// App/SplashScene.cpp - This file is part of NotedELN

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

// SplashScene.C

#include "DefaultSize.h"
#include "SplashScene.h"
#include "SplashView.h"
#include "Notebook.h"
#include "BookSplashItem.h"
#include "Style.h"
#include "AlreadyOpen.h"
#include "Version.h"
#include "Translate.h"
#include "CloneBookDialog.h"
#include "NewBookDialog.h"
#include "VersionControl.h"
#include "DefaultLocation.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QGraphicsView>
#include <QDebug>
#include <QDir>
#include <QEventLoop>

SplashScene::SplashScene(QObject *parent):
  QGraphicsScene(parent) {
  openlocation = DefaultLocation::defaultLocation();
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

  QImage img(":/front.jpg");
  QGraphicsPixmapItem *bg = new QGraphicsPixmapItem();
  bg->setPixmap(QPixmap::fromImage(img));
  addItem(bg);
  QTransform t;
  t.scale(style.real("page-width")/img.width(),
	  style.real("page-height")/img.height());
  bg->setTransform(t);

  QGraphicsTextItem *eln = addText(Translate::_("eln"),
                                   style.font("splash-font"));
  centerAt(eln, style.real("page-width")/2, style.real("page-height") - 50);
  QGraphicsTextItem *dw
    = addText(QString("v. ") + Version::toString() + QString::fromUtf8(" — ")
	      + QString::fromUtf8("(C) Daniel Wagenaar 2013–")
	      + QString::number(Version::buildDate().year()),
	      style.font("splash-small-font"));
  centerAt(dw, style.real("page-width")/2, style.real("page-height") - 20);

}

void SplashScene::makeItems() {
  const double MARGIN = 5;

  double y = 22;
  double x = 45;
  BookSplashItem *bsi;

  int NMAX =  (sceneRect().height() - y - 2*BookSplashItem::SMALLBOXHEIGHT
	       - MARGIN - 80) / (BookSplashItem::BOXHEIGHT + MARGIN);

  QStringList dirs = localNotebooks();
  while (dirs.size()>NMAX)
    dirs.removeLast();
  QSet<QString> absdirs;
  foreach (QString d, dirs)
    absdirs << QDir::cleanPath(QDir::current().absoluteFilePath(d));

  QStringList recent = RecentBooks::instance()->byDate();
  while (dirs.size()<NMAX && !recent.isEmpty()) {
    QString r = recent.takeFirst();
    QString r0 = QDir::cleanPath(r);
    if (!absdirs.contains(r)) {
      dirs << r;
      absdirs << r0;
    }
  }

  QList<BookInfo> books;
  foreach (QString d, dirs)
    if (RecentBooks::instance()->contains(d))
      books << (*RecentBooks::instance())[d];
    else
      books << BookInfo(d);

  std::sort(books.begin(), books.end());
  
  foreach (BookInfo const &b, books) {
    bsi = new BookSplashItem(b.dirname, b);
    connect(bsi, &BookSplashItem::leftClick, this, &SplashScene::openNamed);
    addItem(bsi);
    bsi->setPos(x, y);
    y += BookSplashItem::BOXHEIGHT + MARGIN;
  }

  y += 2*MARGIN;

  bsi = new BookSplashItem(dirs.isEmpty()
			   ? Translate::_("open-existing")
			   : Translate::_("open-other"));
  connect(bsi, &BookSplashItem::leftClick, this, &SplashScene::openExisting);
  addItem(bsi);
  bsi->setPos(x, y);
  y += BookSplashItem::SMALLBOXHEIGHT + MARGIN;

  bsi = new BookSplashItem(Translate::_("create-new"));
  connect(bsi, &BookSplashItem::leftClick, this, &SplashScene::createNew);
  addItem(bsi);
  bsi->setPos(x, y);
  y += BookSplashItem::SMALLBOXHEIGHT + MARGIN;

  if (VersionControl::isGitAvailable()) {
    bsi = new BookSplashItem(Translate::_("clone-remote"));
    connect(bsi, &BookSplashItem::leftClick, this, &SplashScene::cloneRemote);
    addItem(bsi);
    bsi->setPos(x, y);
    y += BookSplashItem::SMALLBOXHEIGHT + MARGIN;
  }
}

QStringList SplashScene::localNotebooks() {
  QDir d(QDir::current());
  QStringList flt; flt << "*.nb";
  return d.entryList(flt, QDir::Dirs, QDir::Name);
}

void SplashScene::cloneRemote() {
  QString loc = CloneBookDialog::getClone();
  if (!loc.isEmpty()) {
    named = loc;
    emit done();
  }
}

void SplashScene::createNew() {
  QString fn = NewBookDialog::getNew();
  if (!fn.isEmpty()) {
    named = fn;
    emit done();
  }
}

void SplashScene::openExisting() {
  QFileDialog qfd(widget);
  qfd.setDirectory(openlocation);
  qfd.setWindowTitle(Translate::_("title-open-existing"));
  qfd.setFileMode(QFileDialog::Directory);
  qfd.setOptions(QFileDialog::ShowDirsOnly);
  if (!qfd.exec()) 
    return;
  QStringList fns = qfd.selectedFiles();
  if (fns.isEmpty())
    return;
  openlocation = qfd.directory().absolutePath();
  if (fns.size()>1)
    qDebug() << "Multiple files selected; using only the first.";
  QString fn = fns[0];
  QDir d(fn);
  if (!d.exists()) {
    QMessageBox::warning(widget, Translate::_("eln"),
                         "'" + fn + "' does not exist.",
                         QMessageBox::Cancel);
    return;
  }
  if ((!d.exists("book.json") && !d.exists("book.eln"))
      || !d.exists("pages")) {
    QMessageBox::warning(widget, Translate::_("eln"),
                         "'" + fn + "' is not a notebook.",
                         QMessageBox::Cancel);
    return;
  }
  named = fn;
  emit done();
}

void SplashScene::openNamed(QString dir) {
  named = dir;
  emit done();
}

 void SplashScene::setWidget(QWidget *w) {
   widget = w;
 }
 
Notebook *SplashScene::openNotebook() {
  SplashScene *ss = new SplashScene();
  QEventLoop el;
  QObject::connect(ss, &SplashScene::done,
                   &el, &QEventLoop::quit,
                   Qt::QueuedConnection);

  SplashView *gv = new SplashView();
  gv->setScene(ss);
  ss->setWidget(gv);
  connect(gv, &SplashView::closing, &el, &QEventLoop::quit);
  QString appname = Translate::_("eln");
#ifndef QT_NO_DEBUG
  appname += " (debug vsn)";
#endif
  gv->setWindowTitle(appname);
  
  gv->resize(DefaultSize::onScreenSize(gv->sizeHint()));

  Notebook *nb = 0;
  while (true) {
    gv->show();
    el.exec();
    gv->hide();
    if (ss->named.isEmpty())
      break;

    if (AlreadyOpen::check(ss->named))
      break;
    nb = Notebook::open(ss->named);
    if (nb)
      break;
    QMessageBox::critical(gv, Translate::_("eln"),
                          "'" + ss->named + "' could not be opened."
			  + "\n" + Notebook::errorMessage(),
                          QMessageBox::Cancel);
    ss->named = "";
  }

  delete gv;
  delete ss;

  return nb;
}
