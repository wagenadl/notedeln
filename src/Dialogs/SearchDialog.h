// App/SearchDialog.h - This file is part of eln

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

// SearchDialog.H

#ifndef SEARCHDIALOG_H

#define SEARCHDIALOG_H

#include <QObject>
#include <QPointer>
#include "PageView.h"

class SearchDialog: public QObject {
  Q_OBJECT;
public:
  SearchDialog(class PageView *parent);
  virtual ~SearchDialog();
  static QString latestPhrase();
  static void setLatestPhrase(QString);
public slots:
  void newSearch();
private slots:
  void gotoPage(int n, Qt::KeyboardModifiers, QString uuid, QString phrase);
private:
  QPointer<PageView> pgView;
  QString lastPhrase;
  static QString &storedPhrase();
};

#endif
