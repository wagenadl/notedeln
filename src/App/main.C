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
  app.setWindowIcon(QIcon(":/eln.png"));

  Notebook *nb = 0;
  if (argc==1) {
    QStringList lb = SplashScene::localNotebooks();
    if (lb.size()==1)
      nb = Notebook::load(lb[0]);
    else
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
      exit(1);
    }
    nb = Notebook::create(argv[2]);
  } else {
    qDebug() << "Usage: eln";
    qDebug() << "Usage: eln notebook";
    qDebug() << "Usage: eln -new notebook";
    qDebug() << "Usage: eln -splash";
    exit(1);
  }
  if (!nb)
    exit(2);

  QObject::connect(&app, SIGNAL(aboutToQuit()), nb, SLOT(flush()));
  
  PageEditor *editor = new PageEditor(nb);
  QSizeF size = editor->sizeHint();
  double dpiX = app.desktop()->logicalDpiX();
  double dpiY = app.desktop()->logicalDpiY();
  editor->resize(size.width()*dpiX/72, size.height()*dpiY/72);
  editor->show();
  int r = app.exec();
  delete editor;
  delete RecentBooks::instance();
  return r;
}
