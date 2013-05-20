// Book/Notebook.H - This file is part of eln

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

// Notebook.H

#ifndef NOTEBOOK_H

#define NOTEBOOK_H

#include <QString>
#include <QDir>
#include <QFile>
#include "EntryFile.H"
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
  EntryFile *page(int pgno);       // } be the first
  EntryFile *createPage(int pgno); // } sheet of a
  bool deletePage(int pgno);      // } Page
  class TOC *toc() const;
  class BookData *bookData() const;
  class Style const &style() const;
  class Mode *mode() const;
  QString filePath(QString) const;
  QString dirPath() const;
public slots:
  void flush(bool mustcommit=false);
private:
  Notebook(QString path);
private slots:
  void titleMod();
  void sheetCountMod();
private:
  QDir root;
  QMap<int, EntryFile *> pgFiles;
  TOCFile *tocFile_;
  BookFile *bookFile_;
  Style const *style_;
  Mode *mode_;
  bool uncommitted;
  QDateTime lastCommit;
};

#endif
