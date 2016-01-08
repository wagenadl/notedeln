// App/PrintDialog.h - This file is part of eln

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

// PrintDialog.H

#ifndef PRINTDIALOG_H

#define PRINTDIALOG_H

#include <QDialog>

class PrintDialog: public QDialog {
  Q_OBJECT;
public:
  enum Range { All, CurrentPage, CurrentEntry, FromTo };
public:
  PrintDialog(QWidget *parent=0);
  virtual ~PrintDialog();
public:
  bool toFile() const;
  QString filename() const;
  QString printername() const;
  bool isDuplex() const;
  bool printFrontPage() const;
  bool printTOC() const;
  bool printEntries() const;
  Range tocRange() const;
  int tocFrom() const;
  int tocTo() const;
  Range entriesRange() const;
  int entriesFrom() const;
  int entriesTo() const;
  bool openFileAfter() const;
public slots:
  void setMaxPage(int);
  void setMaxTOCPage(int);
private slots:
  void toggleTocRange(bool);
  void toggleEntriesRange(bool);
  void setDestination();
  void browse();
private:
  class Ui_printDialog *ui;
};

#endif
