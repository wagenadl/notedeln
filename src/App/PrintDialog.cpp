// PrintDialog.cpp

#include "PrintDialog.H"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QFormLayout>
#include <QPrinter>
#include <QPrinterInfo>

PrintDialog::PrintDialog(QWidget *parent): QDialog(parent) {

  foreach (QPrinterInfo const &x, QPrinterInfo::availablePrinters())
    availPr.append(x);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QFormLayout *settingsLayout = new QFormLayout;
  QHBoxLayout *actionLayout = new QHBoxLayout;

  printers = new QComboBox;
  //  location = new QLabel;
  //  type = new QLabel;
  filename = new QLineEdit;
  duplex = new QCheckBox("Duplex");
  from = new QLineEdit;
  to = new QLineEdit;
  rangeCombo = new QComboBox;
  
  QPushButton *browse = new QPushButton("...");
  QHBoxLayout *fileLayout = new QHBoxLayout;
  QHBoxLayout *rangeLayout = new QHBoxLayout;
  QPushButton *cancel = new QPushButton("Cancel");
  QPushButton *print = new QPushButton("Print");

  foreach (QPrinterInfo const &x, availPr) 
    printers->addItem(x.printerName());
  printers->addSeparator(availPr.size());
  printers->addItem("Print to file (pdf)");

  QMap<PrintRange, QString> rn;
  rn[AllPages] = "All pages";
  rn[CurrentEntry] = "Current entry";
  // etc.
  for (QString x, rn)
    rangeCombo->addItem(x); // in order

  rangeLayout->addWidget(new QLabel("from"));
  rangeLayout->addWidget(from);
  rangeLayout->addWidget(new QLabel("to"));
  rangeLayout->addWidget(to);
  
  QFrame *hline = new QFrame;
  hline->setFrameShape(QFrame::HLine);
  hline->setFrameShadow(QFrame::Raised);
  hline->setLineWidth(1);
  
  formLayout->addRow("Printer", printers);
  fileLayout->addWidget(filename);
  fileLayout->addWidget(browse);
  formLayout->addRow("Filename", fileLayout);
  formLayout->addRow("Range", rangeCombo);

  formLayout->addRow("", duplex);

  actionLayout->addWidget(cancel);
  actionLayout->addStretch();
  actionLayout->addWidget(print);
  
  mainLayout->addLayout(settingsLayout);
  mainLayout->addWidget(hline);
  mainLayout->addLayout(actionLayout);
  
  setLayout(mainLayout);
  resize(sizeHint());

  connect(printers, SIGNAL(currentIndexChanged(int)), SLOT(printerChanged()));
  connect(browse, SIGNAL(clicked(bool)), SLOT(startBrowse()));
  connect(cancel, SIGNAL(clicked(bool)), SLOT(reject()));
  connect(print, SIGNAL(clicked(bool)), SLOT(accept()));
  
  printerChanged();

}

void PrintDialog::printerChanged() {
  int i = printers.currentIndex();
  if (i<availPr.size()) {
    // actual printer
    filename->disable();
    duplex->enable();
  } else {
    filename->enable();
    duplex->disable();
  }
}

void PrintDialog::startBrowse() {
  qDebug() << "PrintDialog::startBrowse";
}






int PrintDialog::romanToInt(QString s) {
  int res = 0;
  QMap tbl;
  tbl["i"] = 1;
  tbl["v"] = 5;
  tbl["x"] = 10;
  tbl["l"] = 50;
  tbl["c"] = 100;
  tbl["d"] = 500;
  tbl["m"] = 1000;
  while (!s.isEmpty()) {
    int i = tbl[s[0].toLower()];
    if (i==0)
      return -1;
    if (s.length()>=2) {
      int j = tbl[s[1].toLower()];
      if (j==0)
        return -1;
      if (j>i) {
        res += j - i;
        s = s.mid(2);
      } else {
        res += i;
        s = s.mid(1);
      }
    } else {
        res += i;
        s = s.mid(1);
    }
  }
  return res;
}

QString PrintDialog::intToRoman(int n) {
  char *huns[] = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
  char *tens[] = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
  char *ones[] = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};

  QString res;

  if (val>=5000)
    return QString::number(val);
  
  // Following code adapted from "paxdiablo" at stackoverflow
  while (val >= 1000) {
    res += "M";
    val -= 1000;
  }

  res +=  huns[val/100];
  val = val % 100;
  res += tens[val/10];
  val = val % 10;
  res += ones[val];

  return res;
}

