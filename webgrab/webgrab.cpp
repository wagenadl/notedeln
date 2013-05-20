// webgrab/webgrab.cpp - This file is part of eln

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
