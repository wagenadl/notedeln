// AboutBox.cpp

#include "AboutBox.h"
#include "Translate.h"
#include "Version.h"
#include <QMessageBox>
#include <QIcon>

void AboutBox::about() {
  QString author = "Daniel A. Wagenaar";
  QString summary = Translate::_("eln-is");
  QString manuref = Translate::_("where-man");
  QString gpl = Translate::_("gpl");
  QString eln = Translate::_("eln");
  QString ttl = "<b>" + eln + "</b> " + Version::toString();
  QString copyright = QString::fromUtf8("(C) 2013–")
    + QString::number(Version::buildDate().year())
    + " " + author;
  QString msg = ttl + "<br>"
    + copyright + "<br><br>"
    + summary + " "
    + manuref + "<br><br>"
    + gpl;

  QMessageBox box;
  box.setWindowTitle(Translate::_("about-eln"));
  box.setTextFormat(Qt::RichText);
  box.setText(msg);
  box.setIconPixmap(QPixmap(":/eln.png"));
  box.exec();
}
