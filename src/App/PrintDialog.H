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
