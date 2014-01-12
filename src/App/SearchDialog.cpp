// SearchDialog.cpp

#include "SearchDialog.H"

#include "PageView.H"
#include "Search.H"
#include "SearchResultScene.H"
#include "SearchView.H"
#include "BookData.H"
#include <QInputDialog>
#include <QProgressDialog>
#include <QMessageBox>

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
      = new SearchResultScene(QString::fromUtf8("Search results for “%1”")
                              .arg(phrase),
                              res,
                              pgView->notebook()->bookData());
    scene->populate();
    connect(scene, SIGNAL(pageNumberClicked(int)),
            this, SLOT(gotoPage(int)));
    SearchView *view = new SearchView(scene);
    delete progress;
    
    view->resize(pgView->size());
    QString ttl = pgView->notebook()->bookData()->title();
    view->setWindowTitle(ttl.replace(QRegExp("\\s\\s*"), " ") + " - eln");
    view->show();
  }
}
                                         
void SearchDialog::gotoPage(int n) {
  pgView->gotoEntryPage(n);
  pgView->window()->raise();
}
