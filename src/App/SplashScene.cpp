// App/SplashScene.cpp - This file is part of eln

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

// SplashScene.C

#include "SplashScene.h"
#include "SplashView.h"
#include "Notebook.h"
#include "BookSplashItem.h"
#include "Style.h"
#include "AlreadyOpen.h"
#include "Version.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsView>
#include <QDebug>
#include <QDir>
#include <QEventLoop>

SplashScene::SplashScene(QObject *parent):
  QGraphicsScene(parent) {
  newRequested = false;
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

  QGraphicsTextItem *eln = addText("ELN", style.font("splash-font"));
  centerAt(eln, style.real("page-width")/2, style.real("page-height") - 50);
  QGraphicsTextItem *dw
    = addText(QString("v. ") + VERSION + QString::fromUtf8(" — ")
	      + QString::fromUtf8("(C) Daniel Wagenaar 2013–2014"),
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
  y += DY; ///2;

  bsi = new BookSplashItem("Create new notebook...");
  connect(bsi, SIGNAL(leftClick(QString)), SLOT(createNew()));
  addItem(bsi);
  bsi->setPos(x, y);
  y += DY; ///2;

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
  QString fn = QFileDialog::getSaveFileName(0, "Create new notebook...",
                                            "",
                                            "Notebooks (*.nb)");
  if (fn.isEmpty())
    return; // aborted by user, we'll go back to splash screen
  if (!fn.endsWith(".nb"))
    fn += ".nb";
  if (QDir::current().exists(fn)) {
    QMessageBox::warning(widget, "eln",
                         "Will not create a new notebook '" + fn
                         + "': file exists.",
                         QMessageBox::Cancel);
    return; // go back to splash screen
  }
  newRequested = true;
  named = fn;
  emit done();
}

void SplashScene::openExisting() {
  QFileDialog qfd(widget);
  qfd.setWindowTitle("Open existing notebook...");
  qfd.setFileMode(QFileDialog::Directory);
  qfd.setOptions(QFileDialog::ShowDirsOnly);
  if (!qfd.exec()) 
    return;
  QStringList fns = qfd.selectedFiles();
  if (fns.isEmpty())
    return;
  if (fns.size()>1)
    qDebug() << "Multiple files selected; using only the first.";
  QString fn = fns[0];
  QDir d(fn);
  if (!d.exists()) {
    QMessageBox::warning(widget, "eln",
                         "'" + fn + "' does not exist.",
                         QMessageBox::Cancel);
    return;
  }
  if (!d.exists("book.json") || !d.exists("toc.json")
      || !d.exists("pages")) {
    QMessageBox::warning(widget, "eln",
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
  QObject::connect(ss, SIGNAL(done()),
                   &el, SLOT(quit()),
                   Qt::QueuedConnection);

  SplashView *gv = new SplashView();
  gv->setScene(ss);
  ss->setWidget(gv);
  connect(gv, SIGNAL(closing()), &el, SLOT(quit()));
  QString appname = "eln";
#ifndef QT_NO_DEBUG
  appname += " (debug vsn)";
#endif
  gv->setWindowTitle(appname);
  
  QSizeF size = gv->sizeHint();
  double dpiX = QApplication::desktop()->logicalDpiX();
  double dpiY = QApplication::desktop()->logicalDpiY();
  gv->resize(size.width()*dpiX/72, size.height()*dpiY/72);

  Notebook *nb = 0;
  while (nb==0) {
    gv->show();
    el.exec();
    gv->hide();
    if (ss->named.isEmpty())
      break;
    if (ss->newRequested) {
      nb = Notebook::create(ss->named);
      if (!nb) 
        QMessageBox::warning(gv, "eln",
                             "'" + ss->named + "' could not be created.",
                             QMessageBox::Cancel);
    } else {
      if (AlreadyOpen::check(ss->named))
	break;
      nb = Notebook::load(ss->named);
      if (!nb) 
        QMessageBox::warning(gv, "eln",
                             "'" + ss->named + "' could not be loaded.",
                             QMessageBox::Cancel);
    }
  }

  delete gv;
  delete ss;

  return nb;
}
