// HtmlOutput.cpp

#include "HtmlOutput.H"
#include "EntryScene.H"
#include "TextBlockItem.H"
#include "GfxBlockItem.H"
#include "TableBlockItem.H"
#include "FootnoteItem.H"
#include "EntryData.H"
#include <QTextDocument>
#include <QDebug>
#include <QImage>
#include <QPainter>

bool HtmlOutput::ok() const {
  return file.error() == QFile::NoError && res.exists();
}

HtmlOutput::HtmlOutput(QString outputFile, QString pageTitle):
  file(outputFile) {
  if (!file.open(QFile::WriteOnly))
    return;

  outputFile.replace(QRegExp(".html$"), "");
  outputFile += "_files";
  int slash = outputFile.lastIndexOf("/");
  local = slash>=0 ? outputFile.mid(slash+1) : outputFile;
  
  html.setDevice(&file);
  html.setCodec("UTF-8");
  html << "<!DOCTYPE HTML PUBLIC "
       << "\"-//W3C//DTD HTML 4.01 Transitional//EN\">\n";
  html << "<html>\n";
  html << "<head>\n";
  html << "<title>" << Qt::escape(pageTitle) << "</title>";
  html << "<link rel=\"stylesheet\" href=\"" << local
       << "/eln.css\" type=\"text/css\">\n";
  html << "<meta http-equiv=\"Content-Type\" "
       << "content=\"text/html;charset=utf-8\" >\n";
  html << "</head>\n";
  html << "\n";

  res = QDir(outputFile);
  if (!res.exists())
    res.mkpath(res.absolutePath());

  QFile cssin(":eln.css");
  cssin.open(QFile::ReadOnly);
  QFile cssout(res.absoluteFilePath("eln.css"));
  cssout.open(QFile::WriteOnly);
  {
    QTextStream in(&cssin);
    QTextStream out(&cssout);
    while (!in.atEnd()) {
      QString x;
      in >> x;
      out << x;
    }
  }
}

HtmlOutput::~HtmlOutput() {
  html.setDevice(0); // close before file gets deleted
}

void HtmlOutput::add(EntryScene const &source) {
  html << "<div class=\"entry\">\n";
  html << "<div class=\"date\">"
       << source.data()->created()
    .toString(source.style().string("date-format"))
       << "</div>\n";
  html << "<div class=\"title\">" << Qt::escape(source.title()) << "</div>\n";

  foreach (BlockItem const *b, source.blocks()) 
    if (dynamic_cast<TextBlockItem const *>(b))
      add(*dynamic_cast<TextBlockItem const *>(b));
    else if (dynamic_cast<GfxBlockItem const *>(b))
      add(*dynamic_cast<GfxBlockItem const *>(b));
    else if (dynamic_cast<TableBlockItem const *>(b))
      add(*dynamic_cast<TableBlockItem const *>(b));

  foreach (FootnoteItem const *f, source.footnotes())
    add(*f);

  html << "</div>\n";  
}


void HtmlOutput::add(GfxBlockItem const &source) {
  html << "<div class=\"gfxblock\">\n";
  QImage img((source.sceneBoundingRect().size()*2).toSize(),
	     QImage::Format_ARGB32);
  QPainter p(&img);
  source.scene()->render(&p, QRectF(), source.sceneBoundingRect());
  p.end();
  QString fn = QString("%1.png").arg((qulonglong)source.data());
  img.save(res.absoluteFilePath(fn));
  html << "<img src=\"" + local + "/" + fn +"\">";
  html << "</div>\n";
}

void HtmlOutput::add(TableBlockItem const &source) {
  html << "<div class=\"tableblock\">\n";
  html << "</div>\n";
}

void HtmlOutput::add(TextBlockItem const &source) {
  html << "<div class=\"textblock\">\n";

  QString txt = source.data()->text()->text();
  QList<MarkupData *> markups = source.data()->text()->markups();
  /* Markups are not necessarily nesting properly. But we have to clean that
     up now, because "Markups are <i>not <b>necessarily</i> nesting</b>"
     is not legal in html. One solution is to rewrite that as
     "Markups are <i>not </i><b><i>necessarily</i></b><b> nesting</b>."
     Of course one could clean that up a little, but I think there's no need.
     So: I'll simply close all at every markup edge.
     Oh. No. That won't work. I cannot arbitrarily close <a href> markups.*/

  QMap<int, QString> hrefs;
  QSet<int> edges;
  edges.insert(0);
  foreach (MarkupData *md, markups) {
    edges.insert(md->start());
    edges.insert(md->end());
    hrefs[md->start()] = txt.mid(md->start(), md->end()-md->start());
  }
  edges.insert(txt.size());
  QList<int> edgeList = edges.toList();
  qSort(edgeList);
  QMap<int, QString> textBits;
  for (int k=0; k<edgeList.size()-1; k++)
    textBits[edgeList[k]] = txt.mid(edgeList[k], edgeList[k+1]-edgeList[k]);

  QMap<int, QSet<QString> > startTags; // maps from edge position
  QMap<int, QSet<QString> > endTags;
  foreach (MarkupData *md, markups) {
    QString tag;
    switch (md->style()) {
    case MarkupData::Italic: tag = "i"; break;
    case MarkupData::Bold: tag = "b"; break;
    case MarkupData::Underline: tag = "u"; break;
    case MarkupData::Link: tag = "a"; break;
    case MarkupData::FootnoteRef: tag = "a #"; break;
    case MarkupData::Superscript: tag = "sup"; break;
    case MarkupData::Subscript: tag = "sub"; break;
    case MarkupData::StrikeThrough: tag = "s"; break;
    case MarkupData::Emphasize: tag = "span class=\"emph\""; break;
    }
    startTags[md->start()].insert(tag);
    endTags[md->end()].insert(tag);
  }

  // Make sure that we properly nest by closing and reopening at edges
  QSet<QString> currentTags;
  foreach (int edge, edgeList) {
    QList<QString> starts = startTags[edge].toList(); qSort(starts);
    QList<QString> ends = endTags[edge].toList(); qSort(ends);
    QString lowest;
    if (!starts.isEmpty())
      lowest = starts[0];
    if (!ends.isEmpty())
      if (lowest.isEmpty() || ends[0]<lowest)
	lowest = ends[0];
    if (!lowest.isEmpty()) {
      foreach (QString tag, currentTags) {
	if (tag>lowest) {
	  if (!endTags[edge].contains(tag))
	    startTags[edge].insert(tag);
	  endTags[edge].insert(tag);
	}
      }
    }
    foreach (QString tag, endTags[edge])
      currentTags.remove(tag);
    foreach (QString tag, startTags[edge])
      currentTags.insert(tag);
  }

  foreach (int edge, edgeList) {
    QList<QString> starts = startTags[edge].toList(); qSort(starts);
    QList<QString> ends = endTags[edge].toList(); qSort(ends);
    for (int i=ends.size()-1; i>=0; --i) {
      QString tag = ends[i];
      int idx = tag.indexOf(" ");
      html << ((idx>=0) ? ("</" + tag.left(idx) +">") : ("</" + tag + ">"));
    }
    for (int i=0; i<starts.size(); ++i) {
      QString tag = starts[i];
      if (tag=="a") 
	html << ("<a href=\"" + Qt::escape(hrefs[edge]) + "\">");
      else if (tag == "a #")
	html << ("<a href=\"#" + Qt::escape(hrefs[edge]) + "\">");
      else
	html << ("<" + tag + ">");
    }
    html << Qt::escape(textBits[edge]);
  }  
  html << "</div>\n";
}

void HtmlOutput::add(FootnoteItem const &source) {
  html << "<div class=\"footnote\">\n";
  html << "</div>\n";
}
