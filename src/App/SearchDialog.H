// SearchDialog.H

#ifndef SEARCHDIALOG_H

#define SEARCHDIALOG_H

#include <QObject>

class SearchDialog: public QObject {
  Q_OBJECT;
public:
  SearchDialog(class PageView *parent);
  virtual ~SearchDialog();
public slots:
  void newSearch();
private slots:
  void gotoPage(int n, QString phrase);
private:
  class PageView *pgView;
  QString lastPhrase;
};

#endif
