// CloneBookDialog.h

#ifndef CLONEBOOKDIALOG_H

#define CLONEBOOKDIALOG_H

#include <QDialog>

class CloneBookDialog: public QDialog {
  Q_OBJECT;
public:
  CloneBookDialog(QWidget *parent=0);
  virtual ~CloneBookDialog();
public:
  static QString getClone();

  QString archiveLocation() const;
  QString archiveHost() const;
  bool isLocal() const;
  QString cloneLocation() const; // = dest + leaf
  QString cloneDestination() const;
  QString leaf() const;
private slots:
  void abrowse();
  void browse();
  void updateLocation(QString);
private:
  class Ui_cloneBookDialog *ui;
};

#endif
