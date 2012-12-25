// Notebook.H

#ifndef NOTEBOOK_H

#define NOTEBOOK_H

#include <QString>
#include <QDir>
#include <QFile>
#include "PageFile.H"
#include "BookFile.H"
#include "TOCFile.H"

class Notebook: public QObject {
  Q_OBJECT;
public:
  static Notebook *load(QString path);
  static Notebook *create(QString path);
  /* Returns null if couldn't create, e.g., if already exists */
public:
  ~Notebook();
  bool hasPage(int pgno) const;   // } pgno must 
  PageFile *page(int pgno);       // } be the first
  PageFile *createPage(int pgno); // } sheet of a
  bool deletePage(int pgno);      // } Page
  class TOC *toc() const;
  class BookData *bookData() const;
  class Style const &style() const;
  class Mode *mode() const;
  QString filePath(QString) const;
public slots:
  void flush();
private:
  Notebook(QString path);
private slots:
  void titleMod();
  void sheetCountMod();
private:
  QDir root;
  QMap<int, PageFile *> pgFiles;
  TOCFile *tocFile_;
  BookFile *bookFile_;
  Style const *style_;
  Mode *mode_;
};

#endif
