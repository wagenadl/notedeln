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
  static Notebook *load(QString path);
  static Notebook *create(QString path);
  /* Returns null if couldn't create, e.g., if already exists */
public:
  ~Notebook();
  bool hasEntry(int pgno) const;    // } pgno must 
  CachedEntry entry(int pgno);       // } be the first
  CachedEntry createEntry(int pgno); // } sheet of an
  bool deleteEntry(int pgno);       // } Entry
  class TOC *toc() const;
  class Index *index() const;
  class BookData *bookData() const;
  class Style const &style() const;
  class Mode *mode() const;
  QString filePath(QString) const; // path of file in root
  QString dirPath() const; // path of root
signals:
  void reloaded(QMap<int,int>);
  /*:S reloaded
   *:D Emitted after the reload() function completes.
   *:A A map of changed start pages. (Currently empty.)
   */
public slots:
  void flush();
  void commitSoonish();
  void commitNow();
  bool reload();
  /*:F reload
   *:D Reloads a notebook from disk. (Useful after bzr update.)
   *:N This summarily removes all pagefiles from memory, invalidating
       all pointers.
   *:R Returns false if reloading was impossible because we are in the
       middle of a background commit.
   */
private:
  Notebook(QString path);
private slots:
  void titleMod();
  void sheetCountMod();
  void updateNowUnless();
  void commitNowUnless();
  void committed(bool ok);
private:
  void loadme();
  void unloadme();
private:
  QDir root;
  QMap<int, CachedEntry> pgFiles;
  TOCFile *tocFile_;
  BookFile *bookFile_;
  Index *index_;
  Style const *style_;
  Mode *mode_;
  class QTimer *updateTimer;
  class QTimer *commitTimer;
  QDateTime mostRecentChange;
  bool hasVC;
  class BackgroundVC *backgroundVC;
};

#endif
