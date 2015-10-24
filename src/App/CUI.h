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
  QString adjustID(QString, qint64, qint64);
  QString adjustID(QString, qint64);
  friend class App;
  QString c, c1, c2;
};

#endif
