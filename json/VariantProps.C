/* The following code was derived from qjson's "qobjecthelper.cpp" and its
   copyright rests with the original authors as indicated below. */

/* This file is part of qjson
  *
  * Copyright (C) 2009 Till Adam <adam@kde.org>
  * Copyright (C) 2009 Flavio Castelli <flavio@castelli.name>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */

#include "VariantProps.H"

namespace VariantProps {
  QVariantMap getprops(QObject const *obj) {
    QSet<QString> ignore;
    ignore.insert("objectName");
    return getprops(obj, ignore);
  }

  QVariantMap getprops(QObject const *obj,
		       QSet<QString> const &ignore) {
    QVariantMap res;

    QMetaObject const *metaobj = obj->metaObject();
    int nProps = metaobj->propertyCount();
    for (int i=0; i<nProps; ++i) {
      QMetaProperty metaprop = metaobj->property(i);
      char const *n = metaprop.name();
      QString name = QString::fromLatin1(name);
      if (!ignore.contains(name) && metaprop.isReadable())
	res[name] = obj->property(n);
    }

    return res;
  }

  void setprops(QVariantMap const &src, QObject *obj) {
    QSet<QString> props;
    
    QMetaObject const *metaobj = obj->metaObject();
    int nProps = metaobj->propertyCount();
    for (int i=0; i<nProps; ++i) {
      QMetaProperty metaprop = metaobj->property(i);
      if (metaprop.isWritable())
	props.insert(QString::fromLatin1(metaprop.name()));
    }

    for (QVariantMap::const_iterator i = src.begin(); i != src.end(); ++i)
      if (props.contains(i.key()))
	obj->setProperty(i.key().toLatin1(), i.value());
  }
};
