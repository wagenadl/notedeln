// AutoNote.H

#ifndef AUTONOTE_H

#define AUTONOTE_H

#include <QString>

class AutoNote {
public:
  static bool autoNote(QString tag, class TextItem *dest, class Style const &);
};

#endif
