// AutoNote.C


#include "AutoNote.H"
#include "TextItem.H"
#include "Style.H"
#include "ResManager.H"
#include "TextData.H"
#include "MagicBiblio.H"

#include <QDebug>

static QString autoNote(QString funcName, QString tag, Style const &st) {
  qDebug() << "AutoNote" << funcName << tag;
  if (funcName=="bib")
    return MagicBiblio(tag, st).ref();
  else
    return "";
}

bool AutoNote::autoNote(QString tag, TextItem *dest, Style const &st) {
  QVariantMap autos = st["auto-notes"].toMap();
  foreach (QString k, autos.keys()) {
    QRegExp re(k);
    if (re.exactMatch(tag)) {
      QString txt = ::autoNote(autos[k].toString(), tag, st);
      if (txt.isEmpty())
	return false;
      qDebug() << "autonote -> " << txt;
      dest->insertBasicHtml(txt, dest->textCursor().position());
      return true;
    }
  }
  return false;
}
