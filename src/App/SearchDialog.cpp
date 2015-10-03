// App/SearchDialog.cpp - This file is part of eln

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

// SearchDialog.cpp

#include "SearchDialog.h"

#include "PageView.h"
#include "Search.h"
#include "SearchResultScene.h"
#include "SearchView.h"
#include "BookData.h"
//#include "FindOverlay.h"
#include "Assert.h"
#include "SheetScene.h"

#include <QInputDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QDebug>

SearchDialog::SearchDialog(PageView *parent): QObject(parent) {
  pgView = parent;
  lastPhrase = "";
}

SearchDialog::~SearchDialog() {
}

void SearchDialog::newSearch() {
  QString phrase = QInputDialog::getText(pgView, "Search in notebook",
                                         "Search phrase:",
                                         QLineEdit::Normal,
                                         lastPhrase);
  lastPhrase = phrase;

  if (phrase.isEmpty())
    return;

  QProgressDialog *progress = new QProgressDialog(pgView);
  progress->setLabelText("Searching...");
  progress->setCancelButton(0);
  progress->setMinimumDuration(500);
  progress->setValue(0);
  Search *search = new Search(pgView->notebook());
  QList<SearchResult> res = search->immediatelyFindPhrase(phrase);

  if (res.isEmpty()) {
    delete progress;
    QMessageBox::information(pgView, "Search - eln",
                             QString::fromUtf8("Search phrase “%1” not found")
                             .arg(phrase));
    return;
  }
  
  SearchResultScene *scene
    = new SearchResultScene(phrase,
			    QString::fromUtf8("Search results for “%1”")
			    .arg(phrase),
			    res,
			    pgView->notebook()->bookData());
  scene->populate();
  connect(scene,
	  SIGNAL(pageNumberClicked(int, Qt::KeyboardModifiers,
				   QString, QString)),
	  this,
	  SLOT(gotoPage(int, Qt::KeyboardModifiers, QString, QString)));
  SearchView *view = new SearchView(scene);
  view->setAttribute(Qt::WA_DeleteOnClose, true);
  connect(parent(), SIGNAL(destroyed()), view, SLOT(close()));
  delete progress;
  
  view->resize(pgView->size()*.9);
  QString ttl = pgView->notebook()->bookData()->title();
  view->setWindowTitle("Search in: "
		       + ttl.replace(QRegExp("\\s\\s*"), " ") + " - eln");
  view->show();
}

void SearchDialog::gotoPage(int n, Qt::KeyboardModifiers m,
                            QString uuid, QString phrase) {
  setLatestPhrase(phrase);
  if (!pgView) {
    qDebug() << "SearchDialog: Pageview disappeared on me.";
    return;
  }
  PageView *view = pgView;
  ASSERT(view);
  if (m & Qt::ShiftModifier)
    view = view->newView();
  view->gotoEntryPage(n);
  view->ensureSearchVisible(uuid, phrase);
  view->window()->raise();
}

void SearchDialog::setLatestPhrase(QString s) {
  storedPhrase() = s;
}

QString SearchDialog::latestPhrase() {
  return storedPhrase();
}

QString &SearchDialog::storedPhrase() {
  static QString s;
  return s;
}
