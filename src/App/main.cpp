// App/main.cpp - This file is part of eln

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
#include "Assert.h"
#include "SplashScene.h"
#include "AlreadyOpen.h"
#include <QMessageBox>
#include "CrashReport.h"

int main(int argc, char **argv) {
  CrashReport cr;
  Notebook *nb = 0;
  App app(argc, argv);
  try {
    app.setWindowIcon(QIcon(":/eln.png"));
    Fonts fonts;

    if (argc==1) {
      nb = SplashScene::openNotebook();
      if (!nb)
        return 0;
    } else if (argc==2 && argv[1][0]!='-') {
      QString fn = argv[1];
      if (fn.endsWith("/book.eln"))
        fn = fn.left(fn.length() - 9);
      if (AlreadyOpen::check(fn))
        return 0;
      nb = Notebook::open(fn);
      if (!nb) {
        QMessageBox::critical(0, "eln",
                              Translate::_("could-not-open-notebook").arg(fn)
                              + "\n" + Notebook::errorMessage(),
                              QMessageBox::Close);
        return 1;
      }
    } else if (argc==3 && argv[1]==QString("-new")) {
      QString fn = argv[2];
      if (QDir(fn).exists()) {
        QMessageBox::critical(0, "eln",
                              Translate::_("could-not-create-notebook-exists")
                              .arg(fn),
                              QMessageBox::Abort);
        return 1;
      }
      nb = Notebook::create(fn) ? Notebook::open(fn) : 0;
      if (!nb) {
        QMessageBox::critical(0, "eln",
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

    int r = 0;
    {
      AppInstance inst(&app, nb);
      r = app.exec();
    }

    delete RecentBooks::instance();
    return r;
  } catch (Assertion a) {
    try {
      if (a.shouldSave() && nb)
        nb->flush();
    } catch (Assertion b) {
      QMessageBox mb(QMessageBox::Critical, "eln",
                "eln suffered a fatal internal error and will have to close:",
                   QMessageBox::Close);
    
      QString msg = a.message().trimmed();
      if (!msg.endsWith("."))
        msg += ".";
      msg += "\nWhile trying to save your most recent changes,"
        " another problem occured:";
      msg += "\n" + b.message().trimmed();
      if (!msg.endsWith("."))
        msg += ".";
      msg += "\n\nPlease send a bug report to the author.";
      mb.setInformativeText(msg);
      if (!a.backtrace().isEmpty())
        mb.setInformativeText("Stack backtrace:\n" + a.backtrace());
      mb.exec();
      return 1;
    }

    QMessageBox mb(QMessageBox::Critical, "eln",
                 "eln suffered a fatal internal error and will have to close:",
                   QMessageBox::Close);
    
    QString msg = a.message().trimmed();
    if (!msg.endsWith("."))
      msg += ".";
    if (a.shouldSave() && nb)
      msg += "\nYour notebook has been saved.";
    else if (nb)
      msg += "\nRegrettably, your work of the last few seconds"
        " may have been lost.";
    msg += "\n\nPlease send a bug report to the author.";
    mb.setInformativeText(msg);
    if (!a.backtrace().isEmpty())
      mb.setInformativeText("Stack backtrace:\n" + a.backtrace());
    mb.exec();
    return 1;
  }
}
