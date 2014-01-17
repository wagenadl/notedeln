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
  void add(class EntryScene const &source);
  void add(class TextBlockItem const &source);
  void add(class GfxBlockItem const &source);
  void add(class TableBlockItem const &source);
  void add(class FootnoteItem const &source);
private:
  QFile file;
  QTextStream html;
  QDir res;
  QString local; // local name of res dir
};

#endif
