// App/App.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// App.C

#include "App.h"
#include "Version.h"
#include "CUI.h"
#include <stdio.h>
#include <QIcon>

App::App(int &argc, char **argv): QApplication(argc, argv) {
  setApplicationName("NotedELN");
  setApplicationVersion(Version::toString());
  setWindowIcon(QIcon(":/eln.png"));

  cui_ = new CUI();
  
#if defined Q_OS_ANDROID
  setAutoSipEnabled(true);
#endif
}

App::~App() {
}

App *App::instance() {
  return dynamic_cast<App*>(QApplication::instance());
}

CUI *App::cui() {
  return cui_;
}
