// File/DataFile.H - This file is part of eln

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

// DataFile.H

#ifndef DATAFILE_H

#define DATAFILE_H

#include "Data.H"

class DataFile0: public QObject {
  Q_OBJECT;
public:  virtual ~DataFile0();
  bool ok() const; // true if last save or creation worked
  Data *data() const;
  bool saveNow() const; // won't do anything if needToSave() is false
  // returns true if ok, even if nothing was saved
  QString fileName() const;
  bool needToSave() const;
public slots:
  void cancelSave();
  void saveSoon();
public:
  static void setSaveDelay(double t_s);
  static void setSaveImmediatelyToo(bool);
protected:
public:
  DataFile0(Data *data, QString fn, QObject *parent=0); // creates
  DataFile0(QString fn, QObject *parent=0); // loads
private slots:
  void saveTimerTimeout();
private:
  mutable bool ok_;
  Data *data_;
  QString fn_;
  mutable bool needToSave_;
  class QTimer *saveTimer_;
  static double saveDelay_;
  static bool saveImmediatelyToo_;
};

template <class T> class DataFile: public DataFile0 {
 protected:
  DataFile<T>(T *data, QString fn, QObject *parent=0):
    DataFile0(data, fn, parent) { }
  DataFile<T>(QString fn, QObject *parent=0):
    DataFile0(fn, parent) { }
 public:
  T *data() const { return dynamic_cast<T*>(DataFile0::data()); }
  static DataFile<T> *create(QString fn, QObject *parent=0) {
    DataFile<T> *df = new DataFile<T>(new T(), fn, parent);
    if (df->ok())
      return df;
    delete df;
    return 0;
  }
  static DataFile<T> *load(QString fn, QObject *parent=0) {
    DataFile<T> *df = new DataFile<T>(fn, parent);
    if (df->ok())
      return df;
    delete df;
    return 0;
  }
};

#endif
