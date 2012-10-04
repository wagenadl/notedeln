// Data.C

#include "Data.H"

Data::Data(Data *parent): QObject(parent) {
  loading_ = true;
  setCreated(QDateTime::currentDateTime());
  setModified(QDateTime::currentDateTime());
  setType("data");
  loading_ = false;
}

Data::~Data() {
}

QDateTime const &Data::created() const {
  return created_;
}

QDateTime const &Data::modified() const {
  return modified_;
}

QString const &Data::type() const {
  return type_;
}

QString const &Data::id() const {
  return id_;
}

void Data::setCreated(QDateTime const &dt) {
  Q_ASSERT(loading_, "Data::setCreated");
  created_ = dt;
}

void Data::setModified(QDateTime const &dt) {
  Q_ASSERT(loading_, "Data::setModified");
  modified_ = dt;
}

void Data::setType(QString const &t) {
  type_ = t;
  markModified();
}

void Data::setId(QString const &i) {
  id_ = i;
  markModified();
}

void Data::markModified() {
  if (loading_)
    return;
  modified_ = QDateTime::currentDateTime();
  emit modify();
}

void Data::load(QVariantMap const &src) {
  loading_ = true;
  loadProps(src);
  loadMore(src);
  loading_ = false;
}

void Data::loadMore(QVariantMap const &) {
  // Descendents may do more interesting things
}

QVariantMap Data::save(QVariantMap) {
  QVariantMap dst;
  saveProps(dst);
  saveMore(dst);
  return dst;
}

void Data::saveMore(QVariantMap &) {
  // Descendents may do more interesting things
}

/* ----- Following code adapted from QJSON's "qobjecthelper.cpp" ----- */

/*
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
   
void Data::loadProps(QVariantMap const &src) {
  QSet<QString> props;
  
  QMetaObject const *metaobj = metaObject();
  int nProps = metaobj->propertyCount();
  for (int i=0; i<nProps; ++i) {
    QMetaProperty metaprop = metaobj->property(i);
    if (metaprop.isWritable())
      props.insert(QString::fromLatin1(metaprop.name()));
  }
  
  for (QVariantMap::const_iterator i = src.begin(); i != src.end(); ++i)
    if (props.contains(i.key()))
      setProperty(i.key().toLatin1(), i.value());
}

void Data::saveProps(QVariantMap &dst) {
  QMetaObject const *metaobj = dst->metaObject();
  int nProps = metaobj->propertyCount();
  for (int i=0; i<nProps; ++i) {
    QMetaProperty metaprop = metaobj->property(i);
    char const *n = metaprop.name();
    QString name = QString::fromLatin1(name);
    if (name!="objectName" && metaprop.isReadable())
      dst
	[name] = property(n);
  }
}

/* ----- End of code adapted from QJSON's "qobjecthelper.cpp" ----- */
  
