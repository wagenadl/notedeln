// Style.H

#ifndef STYLE_H

#define STYLE_H

#include <QVariant>
#include <QFont>

class Style {
public:
  Style(QString fn);
  QVariant operator[](QString) const;
  double real(QString) const;
  QString string(QString) const;
  bool flag(QString) const;
  int integer(QString) const;
  QColor color(QString) const;
  QFont font(QString) const;
  QVariantMap const &options() const;
  bool contains(QString) const;
private:
  QVariantMap options_;
};

#endif
