// App/NewBookDialog.h - This file is part of NotedELN

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

// NewBookDialog.h

#ifndef NEWBOOKDIALOG_H

#define NEWBOOKDIALOG_H

#include <QDialog>

class NewBookDialog: public QDialog {
  Q_OBJECT;
public:
  NewBookDialog(QWidget *parent=0);
  virtual ~NewBookDialog();
public:
  static QString getNew();
  static QString getNewSimple();
  static QString getNewArchive();
public:
  QString location() const;
  bool hasArchive() const;
  bool isRemote() const;
  QString remoteHost() const;
  QString archiveRoot() const;
  QString leaf() const;
private slots:
  void browse();
  void abrowse();
  void locationChanged();
private:
  class Ui_newBookDialog *ui;
};

#endif
