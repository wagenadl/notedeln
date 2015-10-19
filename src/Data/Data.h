// Data/Data.H - This file is part of eln

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

// Data.H

#ifndef DATA_H

#define DATA_H

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMap>

class Data: public QObject {
  Q_OBJECT;
  Q_PROPERTY(QDateTime cre READ created WRITE setCreated)
  Q_PROPERTY(QDateTime mod READ modified WRITE setModified)
  Q_PROPERTY(QString typ READ type WRITE setType)
  Q_PROPERTY(QString uuid READ uuid WRITE setUuid)
public:
  enum ModType {
    UserVisibleMod, // mark modif. time on this object and ancestors
    InternalMod, // do not mark modif. time at all
    NonPropMod, // mark modif. time on this object but not ancestors
    /* This has no effect on whether the Data will be saved to disk: that
       always happens, except while loading. */
  };
public:
  // constructor and destructor
  Data(Data *parent=0);
  virtual ~Data();
  static Data *create(QString type, Data *parent=0);
  // read properties
  QDateTime const &created() const;
  QDateTime const &modified() const;
  QString const &type() const;
  QString const &uuid() const;
  // write properties - not for casual use
  void setCreated(QDateTime const &);
  void setModified(QDateTime const &);
  void setType(QString const &);
  void setUuid(QString const &);
  // other
  virtual void markModified(ModType mt=UserVisibleMod);
  void load(QVariantMap const &);
  QVariantMap save() const;
  virtual bool isWritable() const;
  virtual bool lateNotesAllowed() const;
  virtual bool isRecent() const;
  Data *parent() const;
  QList<Data *> const &allChildren() const { return children_; }
  template <class T> QList<T *> children() const;
  template <class T> T *firstChild() const;
  Data const*findChildByUUID(QString uuid) const;
  
  void insertChildBefore(Data *newChild, Data *ref, ModType mt=UserVisibleMod);
  virtual void addChild(Data *, ModType mt=UserVisibleMod);
  virtual bool deleteChild(Data *, ModType mt=UserVisibleMod);
  virtual Data *takeChild(Data *, ModType mt=UserVisibleMod); // remove but don't delete
  virtual class EntryData const *entry() const; // pointer to ...
  virtual class EntryData *entry(); // ... parent page, if any.
  virtual class Notebook *book() const; // pointer to parent book, if any.
  class Style const &style() const;
  virtual class ResManager *resManager() const; // if any.
  void attachResource(QString);
  void detachResource(QString);
  void detachAllResources();
  QStringList const &resourceTags() const;
  void setResourceTags(QStringList const &);
signals:
  void mod();
protected:
  virtual void loadMore(QVariantMap const &);
  virtual void saveMore(QVariantMap &) const;
  bool loading() const;
private:
  void loadProps(QVariantMap const &);
  void saveProps(QVariantMap &) const;
  void loadChildren(QVariantMap const &);
  void saveChildren(QVariantMap &) const;
  void loadResTags(QVariantMap const &);
  void saveResTags(QVariantMap &) const;
private:
  QDateTime created_;
  QDateTime modified_;
  QString type_;
  QString uuid_;
  QList<Data *> children_;
  QStringList resTags;
private:
  bool loading_;
public:
  template <class T> static T *deepCopy(T const *data);
  /* T must be derived from Data. The copy will be parentless. */
  /* Caution! deepCopy() does not attach signals and slots to the new vsn. */
public: // but only to be used in defs of descendents
  template <class T> class Creator {
  public:
    Creator<T>(QString typ) {
      Data::creators()[typ] = &create;
    }
    static Data *create(Data *parent=0) {
      return new T(parent);
    }
  };
private:
  static QMap<QString, Data *(*)(Data *)> &creators();
};

#include "DataTemplates.h"

#endif
