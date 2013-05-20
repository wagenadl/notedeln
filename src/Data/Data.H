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
public:
  enum ModType {
    UserVisibleMod,
    InternalMod,
    NonPropMod,
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
  // write properties - not for casual use
  void setCreated(QDateTime const &);
  void setModified(QDateTime const &);
  void setType(QString const &);
  // other
  virtual void markModified(ModType mt=UserVisibleMod);
  void load(QVariantMap const &);
  QVariantMap save() const;
  bool isRecent() const;
  Data *parent() const;
  QList<Data *> const &allChildren() const { return children_; }
  template <class T> QList<T *> children() const;
  template <class T> T *firstChild() const;
  
  void insertChildBefore(Data *newChild, Data *ref, ModType mt=UserVisibleMod);
  void addChild(Data *, ModType mt=UserVisibleMod);
  bool deleteChild(Data *, ModType mt=UserVisibleMod);
  Data *takeChild(Data *, ModType mt=UserVisibleMod); // remove but don't delete
  virtual class EntryData const *page() const; // pointer to ...
  virtual class EntryData *page(); // ... parent page, if any.
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

#include "DataTemplates.H"

#endif
