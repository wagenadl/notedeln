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
  QApplication app(argc, argv);
  Options options(argc, argv);
  setGlobalSettings();
  {
    QWidget w;
    QWebEngineView webview(&w);
    PrinterWE p(&webview, options);
    QObject::connect(&webview, &QWebEngineView::loadFinished,
                     &p, &PrinterWE::complete);
    QWebEnginePage *page = webview.page();
    QObject::connect(page, &QWebEnginePage::featurePermissionRequested,
                     &p, &PrinterWE::featureReq);
    QObject::connect(page, &QWebEnginePage::contentsSizeChanged,
                     &p, &PrinterWE::sizeChange);
    page->setAudioMuted(true);
    qDebug() << "Loading url " << options.url;
    webview.load(QUrl(options.url));
    app.exec();
  }
  return 0;
}
