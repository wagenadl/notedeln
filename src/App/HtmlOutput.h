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
