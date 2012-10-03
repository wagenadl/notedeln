// PageFile.H

#ifndef PAGEFILE_H

#define PAGEFILE_H

#include "PageData.H"

class PageFile: public QObject {
public:
  PageFile(QString fn, QObject *parent=0);
  virtual ~PageFile();
  PageData *page();
  bool save() const;
public:
  static PageFile *create(QString fn);
  static void setSaveDelay(double t_s);
  static void setSaveImmediatelyToo(bool);
private slots:
  void saveSoon();
  void saveTimerTimeout();
private:
  PageFile(PageData *data, QString fn, QObject *parent=0);
private:
  PageData *data_;
  QString fn_;
  mutable bool needToSave_;
  class QTimer *saveTimer_;
  static double saveDelay_;
  static bool saveImmediatelyToo_;
};

#endif
