// Notebook.H

#ifndef NOTEBOOK_H

#define NOTEBOOK_H

#include <QString>
#include <QDir>
#include <QFile>
#include "PageFile.H"
#include "BookFile.H"
#include "TOC.H"

class Notebook: public QObject {
  Q_OBJECT;
public:
  static Notebook *load(QString path);
  static Notebook *create(QString path);
  /* Returns null if couldn't create, e.g., if already exists */
public:
  ~Notebook();
  bool hasPage(int pgno) const;
  PageFile *page(int pgno);
  PageFile *createPage(int pgno);
  class TOC *toc() const;
  class ResourceManager *resMgr(int pgno);
  class BookData *bookData() const;
  class Style const &style() const;
public slots:
  void flush();
private:
  Notebook(QString path);
private slots:
  void titleMod();
  void sheetCountMod();
private:
  QDir root;
  QMap<int, ResourceManager *> resMgrs;
  QMap<int, PageFile *> pgFiles;
  TOCFile *tocFile_;
  BookFile *bookFile_;
  Style const *style_;
};

#endif
