// DataText.C

#include "DataText.H"

static DataMaker<DataText> maker("text");

DataText::DataText(QString id, QDomElement parent, QString type):
  Data(id, parent, type) {
}

DataText::DataText(QDomElement defn): Data(defn) {
  for (QDomNode n = defn.firstChild(); !n.isNull();
       n = n.nextSibling()) {
    if (n.isText()) {
      textnode = n.toText();
      break;
    }
  }
}

DataText::~DataText() {
}

QString DataText::text() const {
  if (textnode.isNull())
    return "";
  else
    return textnode.data();
}

void DataText::setText(QString t) {
  if (textnode.isNull()) {
    textnode = defn.ownerDocument().createTextNode(t);
    defn.appendChild(textnode);
  } else {
    textnode.setData(t);
  }
  markModified();
}
