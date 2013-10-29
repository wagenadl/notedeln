// PrintDialog.H

#ifndef PRINTDIALOG_H

#define PRINTDIALOG_H

#include <QDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QList>

class PrintDialog: public QDialog {
  Q_OBJECT;
public:
  PrintDialog(QWidget *parent=0);
  virtual ~PrintDialog();
public:
  enum PrintRange {
    AllPages = 0,
    CurrentEntry = 1,
    PageRange = 2,
    CurrentPage = 3,
    FrontPage = 4,
    TOC = 5,
    AllEntries = 6,
    Index = 7, // NYI
  };
  /* It is worth noting that eln pages are numbered in a slightly curious way:
     p. 0 is the front page
     pp. i-- are the toc
     pp. 1-- are the entries
   */
public: // properties
  PrintRange range() const;
  void setRange(PrintRange pr);
  bool firstPageIsRoman() const;
  int firstPage() const;
  QString firstPageS() const;
  void setFirstPage(QString s);
  void setFirstPage(int n, bool isRoman=false);
  bool lastPageIsRoman() const;
  int lastPage() const;
  QString lastPageS() const;
  void setLastPage(QString s);
  void setLastPage(int n, bool isRoman=false);
  QPrinter *printer() const;
  void setPrinter(QPrinter *);
private:
  static int romanToInt(QString s); // -1 if invalid
  /* romanToInt is not very strict about what it accepts.
     For instance, it magnanimously accepts "il" to mean 49, even though
     "xxxxix" is standard form.
     As a side effect, it does make some poor interpretations. For instance,
     it will happily accept the malformed "xxiiiv" to mean 20+2+4=26.
     Other malformed strings return -1.
  */
  static QString intToRoman(int n); 
  /* intToRoman returns lower case. Numbers above 5000 are not converted. */
signals:
  // rejected and accepted from QDialog are used.
private slots:
  void printerChanged();
  void rangeChanged();
  void startBrowse();
private:
  QList<QPrinterInfo> availPr;

  class QComboBox *printers;
  //  class QLabel *location;
  //  class QLabel *type;
  class QLineEdit *filename;
  class QCheckBox *duplex;
  class QLineEdit *from;
  class QLineEdit *to;
  class QComboBox *rangeCombo;
  
};

#endif
