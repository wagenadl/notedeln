// TableItemDoc.h

#ifndef TABLEITEMDOC_H

#define TABLEITEMDOC_H

#include "TextItemDoc.h"

class TableItemDoc: public TextItemDoc {
  Q_OBJECT;
public:
  TableItemDoc(class TableData *data, QObject *parent=0);
};

#endif
