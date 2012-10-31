// Data.H

#ifndef DATA_H

#define DATA_H

#include <QObject>
#include <QDateTime>
#include <QString>
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
  void markModified(ModType mt=UserVisibleMod);
  void load(QVariantMap const &);
  QVariantMap save() const;
  bool isRecent() const;
  Data *parent() const;
  QList<Data *> const &allChildren() const { return children_; }
  template <class T> QList<T *> children() const {
    QList<T *> res;
    for (QList<Data*>::const_iterator i=children_.begin();
	 i!=children_.end(); ++i) {
      T *c = dynamic_cast<T*>(*i);
      if (c)
	res.append(c);
    }
    return res;
  }
  template <class T> T *firstChild() const {
    for (QList<Data*>::const_iterator i=children_.begin();
	 i!=children_.end(); ++i) {
      T *c = dynamic_cast<T*>(*i);
      if (c)
	return c;
    }
    return 0;
  }
  template <class T> void insertChild(int idx, T *d,
				      ModType mt=UserVisibleMod) {
    for (QList<Data*>::iterator i=children_.begin(); i!=children_.end(); ++i) {
      if (dynamic_cast<T*>(*i))
	--idx;
      if (idx<0) {
	children_.insert(i, d);
	break;
      }
    }
    if (idx>=0)
      children_.append(d);
    addedChild(d, mt);
  }
  void addChild(Data *, ModType mt=UserVisibleMod);
  bool deleteChild(Data *, ModType mt=UserVisibleMod);
  Data *takeChild(Data *, ModType mt=UserVisibleMod); // remove but don't delete
  virtual class PageData const *page() const; // pointer to ...
  virtual class PageData *page(); // ... parent page, if any.
  virtual class Notebook *book() const; // pointer to parent book, if any.
  virtual class ResourceManager *resMgr() const; // if any.
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
  void addedChild(Data *d, ModType mt);
private:
  QDateTime created_;
  QDateTime modified_;
  QString type_;
  QList<Data *> children_;
private:
  bool loading_;
public: // but only to be used in defs of descendents
  template <class T> static T *deepCopy(T const *data) {
    /* T must be derived from Data */
    T *copy = new T(data->parent());
    copy->load(data->save());
    return copy;
  }
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

#endif
