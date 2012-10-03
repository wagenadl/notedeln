// VariantProps.H

#ifndef VARIANTPROPS_H

#define VARIANTPROPS_H

#include <QSet>
#include <QString>
#include <QObject>
#include <QVariant>

namespace VariantProps {
  QVariantMap getprops(QObject const *obj);
  QVariantMap getprops(QObject const *obj, QSet<QString> const &ignore);
  void setprops(QVariantMap const &src, QObject *obj);  
};

#endif
