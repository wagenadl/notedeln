#include <QApplication>
#include <QGraphicsWebView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWebPage>
#include <QDebug>

#include "Printer.H"

int main(int argc, char **argv) {
  bool paginate = false;
  if (argc>=2 && QString("-p")==argv[1]) {
    paginate = true;
    argc--;
    argv++;
  }
  if (argc<2 || argc>3) {
    qDebug() << "Usage: webprinter [-p] url [outfile.pdf]";
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
  p.paginate(paginate);
  QObject::connect(&webview, SIGNAL(loadFinished(bool)),
	  &p, SLOT(complete(bool)));
  webview.load(QUrl(argv[1]));
  scene.addItem(&webview);
  //  view.show();
  app.exec();
  return 0;
}
