// PageChunk.C

#include "PageChunk.H"
#include "ModLimit.H"

PageChunk::PageChunk(QDomElement elt, double y0, QObject *parent):
  QObject(parent) {
  if (elt.tagName == "chunk") {
    xml = elt;
  } else {
    xml = elt.firstChildElement("chunk");
    if (xml.isNull()) {
      xml = elt.ownerDocument().createElement("chunk");
      elt.appendChild(xml);
    }
    xml.setAttribute("y0", QString("%1").arg(y0));
    xml.setAttribute("created", QDateTime::currentDateTime().toString());
    xml.setAttribute("modified", QDateTime::currentDateTime().toString());
  }

  for (QDomElement e = xml.firstChildElement("note");
       !e.isNull();
       e = e.nextSiblingElement("note")) {
    notes_.append(new Note(e, this));
  }
}

QDateTime PageChunk::created() const {
  return QDateTime(xml.attribute("created"));
}

QDateTime PageChunk::modified() const {
  return QDateTime(xml.attribute("modified"));
}

double PageChunk::y0() const {
  return xml.attribute("y0").toDouble();
}

double PageChunk::h() const {
  return 0;
}
  
QList<Note const *> PageChunk::notes() const {
  return notes_; // is this an accepted conversion?
}

Note *PageChunk::newNote(double x, double yrel) {
  Note *note = new Note(xml, x, yrel, this);
  notes_.append(note);
  xml.setAttribute("modified", QDateTime::currentDateTime().toString());
  connect(note, SIGNAL(changed()), this, SIGNAL(changed()));
  emit changed();
  return note;
}

void PageChunk::childChange() {
  // called when anything other than a Note is changed
  xml.setAttribute("modified", QDateTime::currentDateTime().toString());
  emit changed();
}

bool PageChunk::editable() {
  return modified().secsTo(QDateTime::currentDateTime()) < MODLIMIT;
}

//////////////////////////////////////////////////////////////////////

QMap<QString, PageChunk *(*)(QDomElement, QObject*)> PageChunk::makers;

PageChunk *PageChunk::readChunk(QDomElement elt, QObject *parent) {
  QString typ = elt.attribute("type");
  if (makers.contains(typ))
    return (*makers[typ])(elt, parent);
  else
    return 0;
}

void PageChunk::registerMaker(QString typ,
			      PageChunk *(*foo)(QDomElement, QObject*)) {
  makers[typ] = foo;
}

