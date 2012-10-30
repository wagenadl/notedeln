// Data.C

#include "Data.H"
#include <QSet>
#include <QMetaProperty>
#include "PageData.H"
#include <QDebug>

#define MAX_EDIT_DELAY_H 12
// Should this be a style option?

Data::Data(Data *parent): QObject(parent) {
  loading_ = false;
  setCreated(QDateTime::currentDateTime());
  setModified(QDateTime::currentDateTime());
  setType("data");
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

bool Data::editable() const {
  return modified().secsTo(QDateTime::currentDateTime())
    < 60*60*MAX_EDIT_DELAY_H;
}

void Data::setCreated(QDateTime const &dt) {
  created_ = dt;
}

void Data::setModified(QDateTime const &dt) {
  modified_ = dt;
}

void Data::setType(QString const &t) {
  type_ = t;
  //  markModified();
}

void Data::markModified(Data::ModType mt) {
  if (loading_)
    return;
  if (mt==UserVisibleMod || mt==NonPropMod)
    modified_ = QDateTime::currentDateTime();
  emit mod();
  
  if (mt==NonPropMod)
    mt = InternalMod;
  Data *p = dynamic_cast<Data*>(parent());
  if (p)
    p->markModified(mt);
}

void Data::load(QVariantMap const &src) {
  loading_ = true;
  loadProps(src);
  loadChildren(src);
  loadMore(src);
  setCreated(src["cre"].toDateTime());
  setModified(src["mod"].toDateTime());
  loading_ = false;
}

QVariantMap Data::save() const {
  QVariantMap dst;
  saveProps(dst);
  saveChildren(dst);
  saveMore(dst);
  return dst;
}

void Data::loadChildren(QVariantMap const &src) {
  foreach (Data *d, children_)
    delete d;
  children_.clear();

  QVariantList l = src["cc"].toList();
  foreach (QVariant v, l) {
    QVariantMap m = v.toMap();
    Data *d = create(m["typ"].toString(), this);
    if (d) {
      children_.append(d);
      d->load(m);
    } else {
      qDebug() << "Failed to create child of type" << m["typ"].toString();
    }
  }
}

void Data::saveChildren(QVariantMap &dst) const {
  QVariantList l;
  foreach (Data *d, children_) {
    QVariantMap m = d->save();
    l.append(m);
  }
  dst["cc"] = l;
}  

void Data::addChild(Data *d, ModType mt) {
  children_.append(d);
  d->setParent(this);
  markModified(mt);
}

bool Data::deleteChild(Data *d, ModType mt) {
  if (takeChild(d, mt)) {
    delete d;
    return true;
  } else {
    return false;
  }
}

Data *Data::takeChild(Data *d, ModType mt) {
  if (children_.removeOne(d)) {
    markModified(mt);
    return d;
  } else {
    return 0;
  }
}

void Data::loadMore(QVariantMap const &) {
  // Descendents may do more interesting things
}

void Data::saveMore(QVariantMap &) const {
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
  QSet<QString> enumprops;
  
  QMetaObject const *metaobj = metaObject();
  int nProps = metaobj->propertyCount();
  for (int i=0; i<nProps; ++i) {
    QMetaProperty metaprop = metaobj->property(i);
    if (metaprop.isWritable()) {
      QString name = QString::fromLatin1(metaprop.name());
      props.insert(name);
      if (metaprop.isEnumType())
	enumprops.insert(name);
    }
  }
  
  for (QVariantMap::const_iterator i = src.begin(); i != src.end(); ++i) {
    if (props.contains(i.key())) {
      if (enumprops.contains(i.key()))
	// This ridiculous trick is needed to make qt load enum values,
	// because qt doesn't like longlong variants for enum.
	Q_ASSERT(setProperty(i.key().toLatin1(), i.value().toInt()));
      else
	Q_ASSERT(setProperty(i.key().toLatin1(), i.value()));
    }
  }
}

void Data::saveProps(QVariantMap &dst) const {
  QMetaObject const *metaobj = metaObject();
  int nProps = metaobj->propertyCount();
  for (int i=0; i<nProps; ++i) {
    QMetaProperty metaprop = metaobj->property(i);
    char const *n = metaprop.name();
    QString name = QString::fromLatin1(n);
    if (name!="objectName" && metaprop.isReadable())
      dst
	[name] = property(n);
  }
}

/* ----- End of code adapted from QJSON's "qobjecthelper.cpp" ----- */
  
Data *Data::parent() const {
  return dynamic_cast<Data *>(QObject::parent());
}


QMap<QString, Data *(*)(Data *)> &Data::creators() {
  static QMap<QString, Data *(*)(Data *)> g;
  return g;
}

Data *Data::create(QString t, Data *parent) {
  if (creators().contains(t))
    return creators()[t](parent);
  else
    return 0;
}

PageData const *Data::page() const {
  Data const *p = parent();
  return p ? p->page() : 0;
}

PageData *Data::page() {
  Data *p = parent();
  return p ? p->page() : 0;
}

Notebook *Data::book() const {
  Data *p = parent();
  return p ? p->book() : 0;
}

ResourceManager *Data::resMgr() const {
  PageData const *pg = page();
  return pg ? pg->resMgr() : 0;
}

bool Data::loading() const {
  return loading_;
}

