// App/AppInstance.cpp - This file is part of NotedELN

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

// AppInstance.cpp

#include "AppInstance.h"

#include "Notebook.h"
#include "SceneBank.h"
#include "PageEditor.h"
#include "AlreadyOpen.h"
#include "DefaultSize.h"
#include <QApplication>
#include "VersionControl.h"
#include "BackgroundVC.h"
#include "ElnAssert.h"
#include "Style.h"
#include <QTimer>

#include <QTimer>
#include <QDebug>

#define COMMIT_IVAL_S 600 // If vc, commit every once in a while
#define COMMIT_AVOID_S 60 // ... but not too soon after activity
#define UPDATE_IVAL_S 3600 // If vc, check for updates once in a while
#define UPDATE_AVOID_S 900 // ... but not if anything has recently changed

AppInstance::AppInstance(QApplication *app, Notebook *nb): book(nb) {
  commitTimer = 0;
  updateTimer = 0;
  backgroundVC = 0;

  if (!book->isReadOnly())
    setupVC();

  book->load();

  connect(app, &QApplication::aboutToQuit, this, &AppInstance::commitNow);

  bank = new SceneBank(nb);

  PageEditor *editor = new PageEditor(bank);
  editor->resize(DefaultSize::onScreenSize(editor->sizeHint()));
  editor->show();
  registerEditor(editor);

  aopen = new AlreadyOpen(nb->dirPath(), editor);
}

AppInstance::~AppInstance() {
  commitNow();
  
  delete aopen;
  for (auto pe: editors)
    delete pe;
  delete bank;
  delete book;
}

void AppInstance::registerEditor(QObject *e) {
  PageEditor *ed = dynamic_cast<PageEditor *>(e);
  if (ed) {
    editors << ed;
    connect(ed, &QObject::destroyed, this, &AppInstance::forgetEditor);
  }
}

void AppInstance::forgetEditor(QObject *e) {
  PageEditor *ed = static_cast<PageEditor *>(e);
  /* Cannot "dynamic_cast", because object already deleted. */
  if (ed) {
    editors.remove(ed);
  }
}

void AppInstance::setupVC() {
  QString vc = book->checkVersionControl();
  if (!vc.isEmpty()) {
    qDebug() << "Have VC";
    if (VersionControl::update(book->dirPath(), vc)) {
      qDebug() << "Updated from VC";
      backgroundVC = new BackgroundVC(this);
      commitTimer = new QTimer(this);
      commitTimer->setSingleShot(true);
      connect(book, &Notebook::mod, this, &AppInstance::commitSoonish);
      connect(backgroundVC, &BackgroundVC::done, this, &AppInstance::committed);
      connect(commitTimer, &QTimer::timeout, this, &AppInstance::commitNowUnless);
    } else {
      book->markReadOnly();
    }
  }

  updateTimer = new QTimer(this);
  updateTimer->setSingleShot(true);
  updateTimer->start();
  connect(updateTimer, &QTimer::timeout, this, &AppInstance::updateNowUnless);
}  

void AppInstance::updateNowUnless() {
  ASSERT(updateTimer);
  if (mostRecentChange.secsTo(QDateTime::currentDateTime()) < UPDATE_AVOID_S) {
    updateTimer->setInterval(500 * UPDATE_AVOID_S); // that's 1/2 x avoid ival
    updateTimer->start();
  } else {
    // let's see if there is anything to update
    updateNow();
    updateTimer->setInterval(1000 * UPDATE_IVAL_S);
    updateTimer->start();
  }
}

void AppInstance::updateNow() {
  /* I think the logic must be:
     (1) Try to update the folder;
     (2) If something was fetched,  hibernate the editors and close
         the notebook;
     (3) Reopen the notebook and let Notebook deal with merge conflicts.
   */
}


void AppInstance::commitSoonish() {
  mostRecentChange = QDateTime::currentDateTime();
  if (commitTimer && !commitTimer->isActive()) {
    commitTimer->setInterval(COMMIT_IVAL_S * 1000);
    commitTimer->start();
  }
}

void AppInstance::commitNowUnless() {
  if (mostRecentChange.isNull())
    return;
  
  ASSERT(commitTimer);

  if (mostRecentChange.secsTo(QDateTime::currentDateTime()) < COMMIT_AVOID_S) {
    // let's not do it quite yet (test again in a while)
    commitTimer->setInterval(500 * COMMIT_AVOID_S); // that's 1/2 x avoid ival
    commitTimer->start();
  } else {
    book->flush();
    if (!backgroundVC)
      return;
    QString vc = book->style().string("vc");
    if (vc.isEmpty())
      return;
    mostRecentChange = QDateTime(); // invalidate
    backgroundVC->commit(book->dirPath(), vc);
  }
}
  
void AppInstance::commitNow() {
  book->flush();
  if (mostRecentChange.isNull())
    return;
  QString vc = book->style().string("vc");
  if (vc.isEmpty())
    return;
  VersionControl::commit(book->dirPath(), vc);
  mostRecentChange = QDateTime(); // invalidate
}

void AppInstance::committed(bool ok) {
  if (ok) {
    // all good
  } else {
    // we'll have to try again
    commitSoonish();
  }
}
