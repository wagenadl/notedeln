#include <QApplication>
#include <QGraphicsWebView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWebPage>
#include <QDebug>
#include "MozillaPage.H"
#include "Printer.H"
#include "Options.H"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  Options options(argc, argv);

  QGraphicsView view;
  QGraphicsScene scene;
  view.setScene(&scene);
  QGraphicsWebView webview;
  webview.setPage(new MozillaPage());
  webview.page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  webview.setResizesToContents(true);
  Printer p(&webview, options);
  QObject::connect(&webview, SIGNAL(loadFinished(bool)),
	  &p, SLOT(complete(bool)));
  scene.addItem(&webview);
  qDebug() << "Loading url " << options.url;
  webview.load(QUrl(options.url));
  app.exec();
  return 0;
}
