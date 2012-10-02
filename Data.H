// Data.H

#ifndef DATA_H

#define DATA_H

#include <QDomElement>
#include <QString>
#include <QMap>
#include <QDateTime>

class Data {
public:
  Data(QDomElement defn);
  /*:F constructor
   *:D Loads a datum from xml.
   */
  Data(QString tag, QDomElement parent,  QString type="data");
  /*:F constructor
   *:D Creates a new datum of given type and name as a child under parent.
   */
  virtual ~Data();
  static Data *load(QDomElement defn);
  /*:F load
   *:D Loads a new datum of whatever subtype from xml.
   */
  static Data *create(QString tag, QDomElement parent, QString type="data");
  /*:F create
   *:D Creates a new datum of given type and name as a child under parent.
   */
public:
  QString type() const;
  /*:F type
   *:D Returns the type of this datum.
   */
  QString tag() const;
  /*:F tag
   *:D Returns the tag of this datum.
   */
  QDateTime created() const;
  /*:F created
   *:D Returns the creation date of this datum.
   */
  QDateTime modified() const;
  /*:F modified
   *:D Returns the most recent modification date of this datum.
   */
public:
  static void registerLoader(QString typ,
			     Data *(*foo)(QDomElement));
  static void registerCreator(QString typ,
			    Data *(*foo)(QString, QDomElement));
  /*:F registerLoader, registerCreator
   *:D A descendent class can register its existence by calling these with
       pointers to a function that constructs a datum of the given type by
       loading it from an XML element or by creating it from scratch.
       Any descendent *must* register makers, because otherwise, it
       would not be possible to load any data.
       It is easier to use the DataMaker template than to call this function.
       See DataText.C for an example.
  */
protected:
  void markModified();
  /*:F markModified
   *:D Set the modification date of this datum to the current time.
   */
protected:
  friend class FilePage;
  QDomElement defn;
  /*:V defn
   *:D Descendents may directly access the XML definition of the datum.
   */
private:
  static QMap<QString, Data *(*)(QDomElement)> &loaders();
  static QMap<QString, Data *(*)(QString, QDomElement)> &creators();
};

/* DataMaker is a convenience class to register descendents of Data.
   Use as in "static DataMaker<DataXXX> maker("xxx");" in the
   C++ source of DataXX. See DataText.C for an example.
*/

template <class T> class DataMaker {
 public:
  DataMaker<T>(QString typ) {
    T::registerLoader(typ, &load);
    T::registerCreator(typ, &create);
  }
  static Data *load(QDomElement defn) {
    return new T(defn);
  }
  static Data *create(QString tag, QDomElement defn) {
    return new T(tag, defn);
  }
};

#endif
