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
#include "CachedEntry.h"
#include "BookFile.h"
#include "TOCFile.h"
#include <QMap>

class Notebook: public QObject {
  Q_OBJECT;
public:
  static QString checkVersionControl(QString path);
  static Notebook *open(QString path, bool readonly=false);
  /* Returns 0 if couldn't load. */
  static bool create(QString path, QString vc="");
  /* Returns false if couldn't create, e.g., if already exists. */
  static QString errorMessage();
  /* Returns error message from open() or create(). */
public:
  void load();
  QString checkVersionControl();
  ~Notebook();
  bool isReadOnly() const { return ro; }
  /* For hasEntry, entry, createEntry, and deleteEntry, pgno must be
     the first sheet of an Entry. */
  bool hasEntry(int pgno) const;
  CachedEntry entry(int pgno);
  /* If the entry does not exist, we assume TOC corruption and try to recover.
     If recovery fails, the program exits. */
  CachedEntry createEntry(int pgno);
  /* The entry must not already exist. Else, the program exits. */
  bool deleteEntry(int pgno);
  class TOC *toc() const;
  class Index *index() const;
  class BookData *bookData() const;
  QDate endDate() const;
  class Style const &style() const;
  QString filePath(QString) const; // path of file in root
  QString dirPath() const; // path of root
  bool needToSave() const;
signals:
  void mod();
public slots:
  bool flush();
  void markReadOnly();
private:
  Notebook(QString path, bool readonly); // throws QString exception on failure
private slots:
  void titleMod();
  void sheetCountMod();
private:
  CachedEntry recoverFromExistingEntry(int pgno);
  EntryFile *recoverFromMissingEntry(int pgno);
  static QString &errMsg();
  static void copyStyleFile(QDir, QString vc);
  static bool createGitArchive(QDir);
private:
  QDir root;
  bool ro;
  QMap<int, CachedEntry> pgFiles;
  TOCFile *tocFile_;
  BookFile *bookFile_;
  Index *index_;
  Style const *style_;
};

#endif
