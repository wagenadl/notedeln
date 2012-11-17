#include <QApplication>
#include <QGraphicsWebView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWebPage>
#include <QDebug>

#include "Printer.H"

int main(int argc, char **argv) {
  if (argc<2 || argc>3) {
    qDebug() << "Usage: webtosvg url [outfile.svg]";
    return 1;
  }
  QApplication app(argc, argv);
  QGraphicsView view;
  QGraphicsScene scene;
  view.setScene(&scene);
  QGraphicsWebView webview;
  webview.page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  webview.setResizesToContents(true);
  QString fn;
  if (argc>=3)
    fn = argv[2];
  Printer p(&webview, fn);
  QObject::connect(&webview, SIGNAL(loadFinished(bool)),
	  &p, SLOT(complete(bool)));
  webview.load(QUrl(argv[1]));
  scene.addItem(&webview);
  //  view.show();
  app.exec();
  return 0;
}
