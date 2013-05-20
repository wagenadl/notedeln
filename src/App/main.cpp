// main.C

#include "PageEditor.H"
#include <QApplication>
#include <QFile>
#include <QDir>
#include "Notebook.H"
#include "RecentBooks.H"
#include "App.H"
#include <QDesktopWidget>
#include <QDebug>
#include <stdlib.h>
#include "Assert.H"
#include "SplashScene.H"

int main(int argc, char **argv) {
  App app(argc, argv);
  eln_grabsignals();
  app.setWindowIcon(QIcon(":/eln.png"));

  Notebook *nb = 0;
  if (argc==1) {
    QDir here(QDir::current());
    if (here.path().endsWith(".nb") && here.exists("toc.json") && here.exists("book.json") && here.exists("pages")) {
      // inside a notebook
      nb = Notebook::load(here.path());
    }
    if (!nb) {
      QStringList lb = SplashScene::localNotebooks();
      if (lb.size()==1)
        nb = Notebook::load(lb[0]);
    }
    if (!nb)
      nb = SplashScene::openNotebook();
  } else if (argc==2) {
    if (argv[1]==QString("-splash"))
      nb = SplashScene::openNotebook();
    else
      nb = Notebook::load(argv[1]);
  } else if (argc==3 && argv[1]==QString("-new")) {
    if (QDir(argv[2]).exists()) {
      qDebug() << "eln: Cannot create new notebook '" << argv[2]
	       << "': found existing notebook";
      return 1;
    }
    nb = Notebook::create(argv[2]);
  } else {
    qDebug() << "Usage: eln";
    qDebug() << "Usage: eln notebook";
    qDebug() << "Usage: eln -new notebook";
    qDebug() << "Usage: eln -splash";
    return 1;
  }
  if (!nb)
    return 0;

  QObject::connect(&app, SIGNAL(aboutToQuit()), nb, SLOT(flush()));
  
  PageEditor *editor = new PageEditor(nb);
  QString ttl = nb->bookData()->title();
  editor->setWindowTitle(ttl.replace(QRegExp("\\s\\s*"), " ") + " - eln");
  QSizeF size = editor->sizeHint();
  double dpiX = app.desktop()->logicalDpiX();
  double dpiY = app.desktop()->logicalDpiY();
  editor->resize(size.width()*dpiX/72, size.height()*dpiY/72);
  editor->show();
  int r = app.exec();
  delete editor;
  delete nb;
  delete RecentBooks::instance();
  return r;
}
