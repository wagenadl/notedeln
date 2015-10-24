// CUI.h

#ifndef CUI_H

#define CUI_H

#include <QString>

class CUI {
public:
  bool match(QString) const;
  QString current() const;
private:
  CUI();
  friend class App;
  QString c, c1, c2;
};

#endif
