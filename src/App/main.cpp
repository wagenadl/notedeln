// App/main.cpp - This file is part of NotedELN

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

// main.C

#include "Translate.h"
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QIcon>
#include "Notebook.h"
#include "RecentBooks.h"
#include "App.h"
#include "Fonts.h"
#include "AppInstance.h"
#include <QDebug>
#include <stdlib.h>
#include "ElnAssert.h"
#include "SplashScene.h"
#include "AlreadyOpen.h"
#include <QMessageBox>
#include "CrashReport.h"
#include "VersionControl.h"
#include "CUI.h"

int main(int argc, char **argv) {
  CrashReport cr;
  Notebook *nb = 0;
  App app(argc, argv);
  try {
    app.setWindowIcon(QIcon(":/eln.png"));
    Fonts fonts;
    if (argc>1 && QString("-novc")==argv[1]) {
      VersionControl::globallyDisable();
      argc--;
      argv++;
    }
    if (argc>1 && QString("-nocui")==argv[1]) {
      CUI::globallyDisable();
      argc--;
      argv++;
    }

    if (argc==1) {
      nb = SplashScene::openNotebook();
      if (!nb)
        return 0;
    } else if (argc==2 && argv[1][0]!='-') {
      QString fn = QDir::fromNativeSeparators(argv[1]);
      if (fn.endsWith("/book.eln"))
        fn = fn.left(fn.length() - 9);
      if (AlreadyOpen::check(fn))
        return 0;
      nb = Notebook::open(fn);
      if (!nb) {
        QMessageBox::critical(0, Translate::_("eln"),
                              Translate::_("could-not-open-notebook").arg(fn)
                              + "\n" + Notebook::errorMessage(),
                              QMessageBox::Close);
        return 1;
      }
    } else if (argc==3 && argv[1]==QString("-new")) {
      QString fn = argv[2];
      if (QDir(fn).exists()) {
        QMessageBox::critical(0, Translate::_("eln"),
                              Translate::_("could-not-create-notebook-exists")
                              .arg(fn),
                              QMessageBox::Abort);
        return 1;
      }
      nb = Notebook::create(fn) ? Notebook::open(fn) : 0;
      if (!nb) {
        QMessageBox::critical(0, Translate::_("eln"),
                              Translate::_("could-not-create-notebook")
                              .arg(fn)
                              + "\n" + Notebook::errorMessage(),
                              QMessageBox::Abort);
        return 1;
      }
      
    } else {
      qDebug() << Translate::_("usage");
      return 1;
    }
    ASSERT(nb);

    assertion_register_notebook(nb);

    int r = 0;
    AppInstance *inst = 0;
    try {
      inst = new AppInstance(&app, nb);
    } catch (QString s) {
      QMessageBox::critical(0, Translate::_("eln"),
                            Translate::_("could-not-open-notebook")
                            .arg(nb->dirPath())
                            + "\n" + Notebook::errorMessage(),
                            QMessageBox::Close);
    }
    r = app.exec();
    delete inst;

    delete RecentBooks::instance();
    return r;
  } catch (AssertedException) {
    return 1;
  }
}
