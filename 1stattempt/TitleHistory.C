// TitleHistory.C

#include "TitleHistory.H"
#include "ModLimit.H"

TitleHistory::TitleHistory(QDomElement elt, QObject *parent):
  QObject(parent) {
  if (elt.tagName == "title") {
    xml = elt;
  } else {
    xml = elt.firstChildElement("title");
    if (xml.isNull()) {
      xml = elt.ownerDocument().createElement("title");
      elt.appendChild(xml);
    }
  }

  for (QDomElement e = xml.firstChildElement("version");
       !e.isNull();
       e = e.nextSiblingElement("version")) {
    dates.append(QDateTime(e.attribute("date")));
    titles.append(e.attribute("title"));
  }
}

TitleHistory::~TitleHistory() {
}

QString TitleHistory::current() const {
  if (titles.isEmpty())
    return "";
  return titles.last();
}

QString TitleHistory::original() const {
  if (titles.isEmpty())
    return "";
  return titles.first();
}
  
QStringList TitleHistory::allVersions() const {
  return titles;
}

QList<QDateTime> TitleHistory::versionTimes() const {
  return dates;
}

void TitleHistory::newTitle(QString title) {
  if (dates.isEmpty()) {
    dates.append(QDateTime::currentDateTime());
    titles.append(title);
  } else if (dates.last().secsTo(QDateTime::currentDateTime()) > MODLIMIT) {
    dates.append(QDateTime::currentDateTime());
    titles.append(title);
  } else {
    dates.last() = QDateTime::currentDateTime();
    titles.last() = title;
  }
  emit changed();
}
  
