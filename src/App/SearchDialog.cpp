// SearchDialog.cpp

#include "SearchDialog.H"

#include "PageView.H"
#include "Search.H"
#include "SearchResultScene.H"
#include "SearchView.H"
#include "BookData.H"
#include "FindOverlay.H"
#include "Assert.H"
#include "SheetScene.H"

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
  } else {
    SearchResultScene *scene
      = new SearchResultScene(phrase,
			      QString::fromUtf8("Search results for “%1”")
                              .arg(phrase),
                              res,
                              pgView->notebook()->bookData());
    scene->populate();
    connect(scene,
	    SIGNAL(pageNumberClicked(int, Qt::KeyboardModifiers, QString)),
            this,
	    SLOT(gotoPage(int, Qt::KeyboardModifiers, QString)));
    SearchView *view = new SearchView(scene);
    view->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(parent(), SIGNAL(destroyed()), view, SLOT(close()));
    delete progress;
    
    view->resize(pgView->size()*.9);
    QString ttl = pgView->notebook()->bookData()->title();
    view->setWindowTitle("Search in: " + ttl.replace(QRegExp("\\s\\s*"), " ") + " - eln");
    view->show();
  }
}

void SearchDialog::gotoPage(int n, Qt::KeyboardModifiers m, QString phrase) {
  if (!pgView) {
    qDebug() << "SearchDialog: Pageview disappeared on me.";
    return;
  }
  PageView *view = pgView;
  if (m & Qt::ShiftModifier)
    view = pgView->newView(QString::number(n));
  else
    pgView->gotoEntryPage(n);
  
  view->window()->raise();
  qDebug() << "gotoPage" << n << phrase;
  SheetScene *bs = dynamic_cast<SheetScene *>(view->scene());
  ASSERT(bs);
  bs->setOverlay(new FindOverlay(bs, phrase));
}
