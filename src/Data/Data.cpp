// Data/Data.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// Data.C

#include "Data.h"
#include <QSet>
#include <QMetaProperty>
#include "EntryData.h"
#include <QDebug>
#include "Style.h"
#include "Notebook.h"
#include "Assert.h"
#include "UUID.h"

Data::Data(Data *parent0): QObject(parent0) {
  loading_ = false;
  setCreated(QDateTime::currentDateTime());
  setModified(QDateTime::currentDateTime());
  setUuid(UUID::create(32));
  if (parent())
    parent()->addChild(this);
  setType("data");
}

Data::~Data() {
  if (parent())
    qDebug() << "Warning! Data " << this
             << " being deleted while still a child of " << parent();
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

Style const &Data::style() const {
  Notebook *b = book();
  ASSERT(b);
  return b->style();
}

bool Data::isWritable() const {
  Notebook *b = book();
  if (!b || b->isReadOnly())
    return false;
  Data *p = parent();
  return p ? p->isWritable() : false;
}

bool Data::lateNotesAllowed() const {
  Notebook *b = book();
  if (!b || b->isReadOnly())
    return false;
  Data *p = parent();
  return p ? p->lateNotesAllowed() : false;
}

bool Data::isRecent() const {
  QDateTime now = QDateTime::currentDateTime();
  QDate d0 = created().date();
  if (now.date() == d0) // same day
    return true;

  QDateTime nextMorning(d0.addDays(1), QTime(0, 0, 0));
  double allow_h = book() ? book()->style().real("midnight-allowance") : 4;
  return nextMorning.secsTo(now) < allow_h*60*60;
}

void Data::setCreated(QDateTime const &dt) {
  if (dt==created_)
    return;
  created_ = dt;
  markModified(InternalMod);
}

void Data::setModified(QDateTime const &dt) {
  if (dt==modified_)
    return;
  modified_ = dt;
  markModified(InternalMod);
}

void Data::setType(QString const &t) {
  if (type_==t)
    return;
  type_ = t;
  markModified(InternalMod);
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
  loadResTags(src);
  loadChildren(src);
  loadMore(src);
  setCreated(src["cre"].toDateTime());
  setModified(src["mod"].toDateTime());
  loading_ = false;
}

QVariantMap Data::save() const {
  QVariantMap dst;
  saveProps(dst);
  saveResTags(dst);
  saveChildren(dst);
  saveMore(dst);
  return dst;
}

void Data::loadResTags(QVariantMap const &src) {
  resTags.clear();

  if (!src.contains("res"))
    return;

  QVariantList l = src["res"].toList();
  foreach (QVariant v, l) 
    resTags.append(v.toString());
}

void Data::loadChildren(QVariantMap const &src) {
  foreach (Data *d, children_) {
    d->setParent(0); // prevent warning
    delete d;
  }
  children_.clear();

  if (!src.contains("cc"))
    return;
  
  QVariantList l = src["cc"].toList();
  foreach (QVariant v, l) {
    QVariantMap m = v.toMap();
    Data *d = create(m["typ"].toString(), this);
    if (d) {
      d->load(m);
    } else {
      qDebug() << "Data: Failed to create child of type"
	       << m["typ"].toString() << "(no creator)";
    }
  }
}

void Data::saveResTags(QVariantMap &dst) const {
  if (resTags.isEmpty())
    return;
  
  QVariantList l;
  foreach (QString s, resTags)
    l.append(s);
  dst["res"] = l;
}

void Data::saveChildren(QVariantMap &dst) const {
  if (children_.isEmpty())
    return;
  
  QVariantList l;
  foreach (Data *d, children_) {
    QVariantMap m = d->save();
    l.append(m);
  }
  dst["cc"] = l;
}  

void Data::insertChildBefore(Data *d, Data *ref, ModType mt) {
  ASSERT(!children_.contains(d));
  ASSERT(d->parent()==0);
  if (ref==0) {
    addChild(d, mt);
    return;
  }
  for (QList<Data *>::iterator i=children_.begin(); i!=children_.end(); ++i) {
    if (*i == ref) {
      children_.insert(i, d);
      d->setParent(this);
      markModified(mt);
      return;
    }
  }
  qDebug() << "Data::insertChildBefore failed: not found " << ref;
  ASSERT(0);
}  


void Data::addChild(Data *d, ModType mt) {
  ASSERT(!children_.contains(d));
  ASSERT(d->parent()==0 || d->parent()==this);
  children_.append(d);
  d->setParent(this);
  markModified(mt);
}

bool Data::deleteChild(Data *d, ModType mt) {
  if (takeChild(d, mt)) {
    d->deleteLater();
    return true;
  } else {
    return false;
  }
}

Data *Data::takeChild(Data *d, ModType mt) {
  if (children_.removeOne(d)) {
    markModified(mt);
    d->setParent(0);
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
	ASSERT(setProperty(i.key().toLatin1(), i.value().toInt()));
      else
	ASSERT(setProperty(i.key().toLatin1(), i.value()));
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
      dst[name] = property(n);
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

EntryData const *Data::entry() const {
  Data const *p = parent();
  return p ? p->entry() : 0;
}

EntryData *Data::entry() {
  Data *p = parent();
  return p ? p->entry() : 0;
}

Notebook *Data::book() const {
  Data *p = parent();
  return p ? p->book() : 0;
}

ResManager *Data::resManager() const {
  Data const *p = parent();
  return p ? p->resManager() : 0;
}

bool Data::loading() const {
  return loading_;
}

void Data::attachResource(QString r) {
  if (resTags.contains(r)) {
    qDebug() << "Already contained";
    return;
  }
  resTags.append(r);
  markModified();
}

void Data::detachResource(QString r) {
  if (!resTags.contains(r))
    return;
  resTags.removeOne(r);
  markModified();
}

QStringList const &Data::resourceTags() const {
  return resTags;
}

void Data::setResourceTags(QStringList const &l) {
  if (resTags==l)
    return;
  resTags = l;
  markModified();
}

void Data::detachAllResources() {
  if (resTags.isEmpty())
    return;
  resTags.clear();
  markModified();
}

QString const &Data::uuid() const {
  return uuid_;
}

void Data::setUuid(QString const &u) {
  if (uuid_==u)
    return;
  uuid_ = u;
  markModified(InternalMod);
}

Data const *Data::findChildByUUID(QString id) const {
  if (uuid_==id)
    return this;
  foreach (Data const *c, allChildren()) {
    Data const *r = c->findChildByUUID(id);
    if (r)
      return r;
  }
  return 0;
}
