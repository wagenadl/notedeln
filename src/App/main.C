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

QString findANotebook() {
  // we could try to remember our most recent one
  QDir d(QDir::current());
  QStringList flt; flt.append("*.nb");
  QStringList dd = d.entryList(flt, QDir::Dirs, QDir::Name);
  if (dd.isEmpty()) {
    qDebug() << "eln: No notebook found in" << d.absolutePath();
    exit(1);
  } else if (dd.size()>1) {
    qDebug() << "eln: Multiple notebooks found in" << d.absolutePath();
    exit(1);
  }
  qDebug() << "Loading notebook" << dd[0];
  return dd[0];
}

int main(int argc, char **argv) {
  App app(argc, argv);
  app.setWindowIcon(QIcon(":/eln.png"));

  QString fn;
  int argi=1;
  bool newFlag = false;
  if (argc>argi) {
    if (argv[1]==QString("-new")) {
      newFlag = true;
      argi++;
    }
  }
  if (argc==argi) 
    fn = findANotebook();
  else if (argc==argi+1)
    fn = argv[argi];
  else {
    qDebug() << "Usage: eln [-new] [notebook]";
    exit(1);
  }

  QDir d(fn);
  Notebook *nb=0;
  if (newFlag) {
    if (d.exists()) {
      qDebug() << "eln: Cannot create new notebook" << fn
	       << "- found existing notebook";
      exit(1);
    }
    nb = Notebook::create(fn);
  } else {
    if (!d.exists()) {
      qDebug() << "eln: Cannot load notebook" << fn
	       << "- not found";
      exit(1);
    }
    nb = Notebook::load(fn);
  }
  ASSERT(nb);
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
