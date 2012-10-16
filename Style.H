// Style.H

#ifndef STYLE_H

#define STYLE_H

#include <QVariant>

class Style {
public:
  Style(QString fn);
  static Style const &defaultStyle();
  QVariant operator[](QString) const;
  QVariantMap const &options() const;
private:
  QVariantMap options_;
};

#endif
