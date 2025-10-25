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
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char **argv) {
  CrashReport cr;
  bool ro = false;
  Notebook *nb = 0;
  qDebug() << "Hello world";
  qDebug() << "argc is " << argc;
  qDebug() << "argv is " << argv;
  for (int k=0; k<argc; k++) {
      qDebug() << "argv[" << k << "] is  " << argv[k];
  }
  App app(argc, argv);
  qDebug() << "App constructed";

  QCommandLineOption cli_new("new", "Create new notebook");
  QCommandLineOption cli_ro("ro", "Open notebook read-only");
  QCommandLineOption cli_novc("novc", "Disable version control");
  QCommandLineOption cli_nocui("nocui", "Disable use of computer IDs");

  QCommandLineParser cli;
  cli.setApplicationDescription("\n"
    "NotedELN is an Electronic Lab Notebook for scientists of all stripes.\n"
    "\n"
    "More information is at https://danielwagenaar.net/eln.");
  cli.addHelpOption();
  cli.addVersionOption();
  cli.addPositionalArgument("book", "Specify notebook to open", "[book]");
  cli.addOption(cli_new);
  cli.addOption(cli_ro);
  cli.addOption(cli_novc);
  cli.addOption(cli_nocui);
                                    
  cli.process(app);
  QStringList args = cli.positionalArguments();

  try {
    Fonts fonts;
    if (cli.isSet("novc")) 
      VersionControl::globallyDisable();
    if (cli.isSet("nocui"))
      CUI::globallyDisable();
    
    if (args.size()>0) {
      QString fn = QDir::fromNativeSeparators(args[0]);
      if (cli.isSet("new")) {
        if (cli.isSet("ro"))
          cli.showHelp(3);
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
      } else { // not new
        if (fn.endsWith("/book.eln"))
          fn = fn.left(fn.length() - 9);
        if (AlreadyOpen::check(fn))
          return 0;
        nb = Notebook::open(fn, cli.isSet("ro"));
        if (!nb) {
          QMessageBox::critical(0, Translate::_("eln"),
                                Translate::_("could-not-open-notebook").arg(fn)
                                + "\n" + Notebook::errorMessage(),
                                QMessageBox::Close);
          return 1;
        }
      }
    } else { // no book specified
      if (cli.isSet("new") || cli.isSet("ro"))
        cli.showHelp(4);
      nb = SplashScene::openNotebook();
      if (!nb)
        return 0;
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
