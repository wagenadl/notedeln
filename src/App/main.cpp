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
#include "PageEditor.h"
#include "SceneBank.h"
#include <QApplication>
#include <QFile>
#include <QDir>
#include "Notebook.h"
#include "RecentBooks.h"
#include "App.h"
#include "Fonts.h"
#include <QDesktopWidget>
#include <QDebug>
#include <stdlib.h>
#include "Assert.h"
#include "SplashScene.h"
#include "AlreadyOpen.h"
#include "DefaultSize.h"
#include <QMessageBox>
#include "SignalGrabber.h"

int main(int argc, char **argv) {
  SignalGrabber siggrab;
  Notebook *nb = 0;
  try {
    App app(argc, argv);
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
      nb = Notebook::load(fn);
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
      nb = Notebook::create(fn) ? Notebook::load(fn) : 0;
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

    QObject::connect(&app, SIGNAL(aboutToQuit()), nb, SLOT(commitNow()));

    SceneBank *bank = new SceneBank(nb);
    PageEditor *editor = new PageEditor(bank);

    editor->resize(DefaultSize::onScreenSize(editor->sizeHint()));
    new AlreadyOpen(nb->dirPath(), editor);
    editor->show();
    int r = app.exec();
    delete bank;
    delete nb;
    nb = 0;
    delete RecentBooks::instance();
    return r;
  } catch (Assertion a) {
    try {
      if (a.shouldSave() && nb)
        nb->flush();
    } catch (Assertion b) {
      QString msg = a.message();
      if (!msg.endsWith("."))
        msg += ".";
      msg += "\nWhile trying to save your most recent changes,"
        " another problem occured:";
      msg += "\n" + b.message();
      if (!msg.endsWith("."))
        msg += ".";
      msg += "\nThe application has to quit now. Please file a bug report.";
      QMessageBox::critical(0, "eln", msg);
      return 1;
    }
    QString msg = a.message();
    if (!msg.endsWith("."))
      msg += ".";
    if (a.shouldSave() && nb)
      msg += "\nYour notebook has been saved.";
    msg += "\nThe application has to quit now. Please file a bug report.";
    QMessageBox::critical(0, "eln", msg);
    return 1;
  }
}
