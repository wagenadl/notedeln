// App/HtmlOutput.h - This file is part of eln

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

// HtmlOutput.H

#ifndef HTMLOUTPUT_H

#define HTMLOUTPUT_H

#include <QString>
#include <QTextStream>
#include <QDir>
#include <QFile>

class HtmlOutput {
public:
  HtmlOutput(QString outputFile, QString pageTitle);
  bool ok() const;
  ~HtmlOutput();
  void add(class EntryScene *source);
  void add(class TextBlockItem const *source, class ResManager const *resmgr);
  void add(class GfxBlockItem const *source, ResManager const *resmgr);
  void add(class TableBlockItem const *source, ResManager const *resmgr);
  void add(class TextData const *source, ResManager const *resmgr,
	   int startidx=0, int endidx=-1, QString cls="text");
  void add(class GfxNoteData const *source, ResManager const *resmgr);
  void add(class FootnoteData const *source, ResManager const *resmgr);
private:
  void addFootnotes(class Data const *source, ResManager const *resmgr);
  void collectGfxNotes(Data const *src, QList<GfxNoteData const *> &dst);
  void addGfxNotes(Data const *src, ResManager const *resmgr);
  void addRef(QString key, ResManager const *resmgr);
  void addRefStart(QString key, ResManager const *resmgr);
  void addRefEnd(QString key, ResManager const *resmgr);
  void buildGfxRefs(Data const *source, QList<QString> &dst);
  static void removeDirRecursively(QDir dir);
  static QString escape(QString);
private:
  QFile file;
  QTextStream html;
  QDir res;
  QString local; // local name of res dir
};

#endif
