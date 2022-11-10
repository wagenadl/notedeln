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
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QDebug>
#include "PrinterWE.h"
#include "Options.h"

void setGlobalSettings() {
  QWebEngineSettings *s = QWebEngineProfile::defaultProfile()->settings();
  s->setAttribute(QWebEngineSettings::AutoLoadImages, true);
  s->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
  s->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
  s->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);
  s->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
  s->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
  s->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, false);
  s->setAttribute(QWebEngineSettings::PluginsEnabled, false);
  s->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, false);
  s->setAttribute(QWebEngineSettings::TouchIconsEnabled, false);
}

int main(int argc, char **argv) {
  qDebug() << "webgrab";
  QApplication app(argc, argv);
  qDebug() << "app created";
  Options options(argc, argv);
  qDebug() << "options created";
  //setGlobalSettings();
  qDebug() << "options set";
  QWidget *w = new QWidget;
  qDebug() << "widget created";
  QWebEngineView *webview = new QWebEngineView(w);
  qDebug() << "view created";
  PrinterWE *p = new PrinterWE(webview, options);
  qDebug() << "printer created";
  QObject::connect(webview, &QWebEngineView::loadFinished,
                   p, &PrinterWE::complete);
  QWebEnginePage *page = webview->page();
  qDebug() << "page is " << page;
  QObject::connect(page, &QWebEnginePage::featurePermissionRequested,
                   p, &PrinterWE::featureReq);
  QObject::connect(page, &QWebEnginePage::contentsSizeChanged,
                   p, &PrinterWE::sizeChange);
  qDebug() << "connected";
  page->setAudioMuted(true);
  qDebug() << "Loading url " << options.url;
  webview->load(QUrl(options.url));
  qDebug() << "ready to exec";
  app.exec();
  qDebug() << "back from exec";
  return 0;
}
