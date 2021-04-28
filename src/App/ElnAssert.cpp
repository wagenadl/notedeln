// App/Elnassert.cpp - This file is part of eln

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

// Elnassert.C

#include "ElnAssert.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include "Calltrace.h"
#include "Notebook.h"
#include "Translate.h"
#include <QList>
#include <QPointer>
#include "Version.h"

#ifdef QT_NO_DEBUG
#define ASSERT_BACKTRACE 0
#else
#define ASSERT_BACKTRACE 1
#endif


#include <QString>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QList>

class Assertion {
public:
  Assertion(QString msg);
  QString message() const { return msg; }
  QString backtrace() const { return trc; }
public:
  static void crash(QString msg, char const *file=0, int line=0);
  static void registerNotebook(class Notebook *);
private:
  void reportSaved(int nsaved, int nunsaved);
  void reportFailedToSave(QString msg2);
  QString tryToSave(); // catches further assertions, returns msg, or "" if OK
private:
  QString msg;
  QString trc;
private:
  static int &priorFailures();
  static QList<QPointer<QObject> > &registeredBooks();
  static QSet<class Notebook *> notebooks();
  static QString email;
  static QString vsn;
};

QString Assertion::email;
QString Assertion::vsn;

//////////////////////////////////////////////////////////////////////

Assertion::Assertion(QString msg): msg(msg) {
#if ASSERT_BACKTRACE
  trc = Calltrace::full(1);
#endif
  qDebug() << trc;
  qDebug() << msg;
}

int &Assertion::priorFailures() {
  static int pf = 0;
  return pf;
}

void Assertion::crash(QString msg, char const *file, int line) {
  Assertion a(QString::fromUtf8("Assertion “%1” failed"
                                " in file “%2” at line %3.")
              .arg(msg).arg(file).arg(line));

  if (++priorFailures()>2) {
    qDebug() << "Assertion failed while quitting. Terminating hard.\n";
    abort();
  } else {
    // QString msg = a.tryToSave();
    int n = notebooks().size();
    a.reportSaved(0, n);
    ::exit(1);
    // throw AssertedException();
  }
}

void Assertion::registerNotebook(Notebook *nb) {
  registeredBooks() << nb;
  if (vsn.isEmpty())
    vsn = Version::toString();
  if (email.isEmpty())
    email = Translate::_("author-email");
}

QList<QPointer<QObject> > &Assertion::registeredBooks() {
  static QList<QPointer<QObject> > nbb;
  return nbb;
}

QSet<Notebook *> Assertion::notebooks() {
  QSet<Notebook *> nbb;
  for (QObject *b: registeredBooks()) {
    Notebook *nb = dynamic_cast<Notebook *>(b);
    if (nb)
      nbb << nb;
  }
  return nbb;
}

QString Assertion::tryToSave() {
  if (priorFailures()>1)
    return ""; // don't save unless this is first failed assertion
  try {
    for (Notebook *nb: notebooks())
      nb->flush();
    return "";
  } catch (Assertion a) {
    return a.message();
  }
}
  
void Assertion::reportSaved(int nsaved, int nunsaved) {
  QMessageBox mb(QMessageBox::Critical, Translate::_("eln"),
                 Translate::_("eln")
                 + " suffered a fatal internal error and will have to close:",
                 QMessageBox::Close);
    
  QString msg = message().trimmed();
  if (!msg.endsWith("."))
    msg += ".";
  if (!backtrace().isEmpty())
    msg += "\n\nStack backtrace:\n" + backtrace();
  if (nunsaved>0)
    msg += "\nRegrettably, your work of the last few seconds"
      " may have been lost.";
  else if (nsaved>0)
    msg += "\nYour notebook has been saved.";
  msg += "\n\nPlease send a bug report to the author";
  if (!email.isEmpty())
    msg += " at " + email;
  msg += ".";
  if (!vsn.isEmpty())
    msg += " Please mention this ELN version: " + vsn + ".";
  msg += "\n\n(ELN automatically saves your work every few seconds,"
    " so hopefully your data loss is minimal."
    " Regardless: apologies for the inconvenience.)";
  mb.setInformativeText(msg);
  mb.exec();
}

void Assertion::reportFailedToSave(QString msg2) {
  QMessageBox mb(QMessageBox::Critical, Translate::_("eln"),
                 Translate::_("eln")
                 + " suffered a fatal internal error and will have to close:",
                 QMessageBox::Close);
    
  QString msg = message().trimmed();
  if (!msg.endsWith("."))
    msg += ".";

  msg += "\nWhile trying to save your most recent changes,"
    " another problem occurred:";
  msg += "\n" + msg2.trimmed();
  if (!msg.endsWith("."))
    msg += ".";

  if (!backtrace().isEmpty())
    msg += "\n\nStack backtrace:\n" + backtrace();

  msg += "\nRegrettably, your work of the last few seconds"
    " may therefore have been lost.";
  msg += "\n\nPlease send a bug report to the author.";
  mb.setInformativeText(msg);
  mb.exec();
}

//////////////////////////////////////////////////////////////////////
void assertion_register_notebook(class Notebook *nb) {
  Assertion::registerNotebook(nb);
}

void assertion_crash(QString msg, char const *file, int line) {
  Assertion::crash(msg, file, line);
}

void assertion_complain(QString msg, char const *file, int line) {
  qDebug() << "Trouble: " << msg << "in file" << file << "at line" << line;
}
