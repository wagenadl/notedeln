// Note.C

#include "Note.H"
#include "ModLimit.H"

Note::Note(QDomElement elt, double x0, double y0, QObject *parent):
  QObject(parent) {
  if (elt.tagName == "note") {
    xml = elt;
  } else {
    xml = elt.firstChildElement("note");
    if (xml.isNull()) {
      xml = elt.ownerDocument().createElement("note");
      elt.appendChild(xml);
    }
    xml.setAttribute("x0", QString("%1").arg(x0));
    xml.setAttribute("y0", QString("%1").arg(y0));
    xml.setAttribute("created", QDateTime::currentDateTime().toString());
    xml.setAttribute("modified", QDateTime::currentDateTime().toString());
  }
}

Note::~Note() {
}

QDateTime Note::created() const {
  return QDateTime(xml.attribute("created"));
}

QDateTime Note::modified() const {
  return QDateTime(xml.attribute("modified"));
}

double Note::x0() const {
  return xml.attribute("x0").toDouble();
}

double Note::y0() const {
  return xml.attribute("y0").toDouble();
}

QString Note::text() const {
  return xml.attribute("text");
}

bool Note::setText(QString t) {
  if (modified().secsTo(QDateTime::currentDateTime()) >= MODLIMIT)
    return false;
  
  xml.setAttribute("text", text);
  xml.setAttribute("modified", QDateTime::currentDateTime().toString());
  emit changed();
  return true;
}

// Really, a Note should be GraphicsItem with a couple of children for
// the date and the actual text
