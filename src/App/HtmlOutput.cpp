// HtmlOutput.cpp

#include "HtmlOutput.H"
#include "EntryScene.H"
#include "TextBlockItem.H"
#include "TextData.H"
#include "TitleData.H"
#include "ResManager.H"
#include "GfxBlockItem.H"
#include "TableBlockItem.H"
#include "TableData.H"
#include "FootnoteData.H"
#include "LateNoteData.H"
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
      QString x = in.readLine();
      out << x << "\n";
    }
  }
}

HtmlOutput::~HtmlOutput() {
  html.setDevice(0); // close before file gets deleted
}

void HtmlOutput::add(EntryScene const *source) {
  html << "<div class=\"entry\">\n";
  html << "<div class=\"date\">"
       << source->data()->created()
    .toString(source->style().string("date-format"))
       << "</div>\n";
  html << "<div class=\"title\">" << Qt::escape(source->title()) << "</div>\n";

  ResManager const *resmgr = source->data()->resManager();
  
  foreach (BlockItem const *b, source->blocks()) 
    if (dynamic_cast<TableBlockItem const *>(b))
      add(dynamic_cast<TableBlockItem const *>(b), resmgr);
    else if (dynamic_cast<TextBlockItem const *>(b))
      add(dynamic_cast<TextBlockItem const *>(b), resmgr);
    else if (dynamic_cast<GfxBlockItem const *>(b))
      add(dynamic_cast<GfxBlockItem const *>(b), resmgr);

  addGfxNotes(source->data()->title(), resmgr);

  html << "<div class=\"footnotes\">\n";
  addFootnotes(source->data(), resmgr);
  html << "</div>\n";
  html << "</div>\n";  
}

void HtmlOutput::addFootnotes(Data const *data, ResManager const *resmgr) {
  ASSERT(data);
  FootnoteData const *fnd = dynamic_cast<FootnoteData const *>(data);
  if (fnd) 
    add(fnd, resmgr);
  foreach (Data const *d, data->allChildren())
    addFootnotes(d, resmgr);
}

void HtmlOutput::addGfxNotes(Data const *data, ResManager const *resmgr) {
  ASSERT(data);
  GfxNoteData const *nd = dynamic_cast<GfxNoteData const *>(data);
  if (nd)
    add(nd, resmgr);
  foreach (Data const *d, data->allChildren())
    addGfxNotes(d, resmgr);
}
void HtmlOutput::buildGfxRefs(Data const *source, QList<QString> &dst) {
  ASSERT(source);
  TextData const *td = dynamic_cast<TextData const *>(source);
  if (td) {
    QString txt = td->text();
    foreach (MarkupData *md, td->markups()) 
      if (md->style()==MarkupData::Link)
	dst << txt.mid(md->start(), md->end()-md->start());
  }
  foreach (Data const *d, source->allChildren())
    buildGfxRefs(d, dst);
}

void HtmlOutput::add(GfxBlockItem const *source, ResManager const *resmgr) {
  html << "<div class=\"gfxblock\">\n";
  html << "<div class=\"gfx\">\n";
  QImage img((source->sceneBoundingRect().size()*2).toSize(),
	     QImage::Format_ARGB32);
  QPainter p(&img);
  source->scene()->render(&p, QRectF(), source->sceneBoundingRect());
  p.end();
  QString fn = QString("%1.png").arg((qulonglong)source->data());
  img.save(res.absoluteFilePath(fn));
  html << "<img src=\"" + local + "/" + fn +"\">";
  html << "</div>\n";
  QList<QString> gfxrefs;
  buildGfxRefs(source->data(), gfxrefs);
  if (!gfxrefs.isEmpty()) {
    html << "<div class=\"gfxrefs\">\n";
    // sort in some nicer way?
    foreach (QString key, gfxrefs) {
      html << "<div class=\"gfxref\">\n";
      // should actually get the resource
      html << "<a href=\"" << Qt::escape(key) << "\">";
      html << Qt::escape(key) << "</a>\n";
      html << "/div>\n";
    }
    html << "</div>\n";
  }
  html << "</div>\n";
}

void HtmlOutput::add(TableBlockItem const *source, ResManager const *resmgr) {
  html << "<div class=\"tableblock\">\n";
  html << "<table>\n";
  TableData const *td = source->data()->table();
  for (int r=0; r<td->rows(); r++) {
    html << "<tr>\n";
    for (int c=0; c<td->columns(); c++) {
      html << "<td>\n";
      add(td, resmgr,
	  td->cellStart(r, c), 
	  td->cellStart(r, c) + td->cellLength(r, c)); 
      html << "</td>\n";
    }
    html << "</tr>\n";
  }
  html << "</table>\n";
  html << "</div>\n";
}

void HtmlOutput::add(TextBlockItem const *source, ResManager const *resmgr) {
  html << "<div class=\"textblock\">\n";
  add(source->data()->text(), resmgr);
  addGfxNotes(source->data(), resmgr);
  html << "</div>\n";
}

void HtmlOutput::add(GfxNoteData const *source, ResManager const *resmgr) {
  bool isLate = dynamic_cast<LateNoteData const *>(source);
  html << "<div class=\"";
  html << (isLate ? "latenote" : "gfxnote");
  html << "\">\n";
  if (isLate) {
    html << "<span class=\"latedate\">";
    html << source->created().toString(source->style().string("date-format"));
    html << QString::fromUtf8(" —");
    html << "</span>\n";
  }
  add(source->text(), resmgr, 0, -1, isLate ? "latenote" : "gfxnote");
  html << "</div>\n";
}

void HtmlOutput::add(TextData const *source, ResManager const *resmgr,
		     int startidx, int endidx, QString cls) {
  QString txt = source->text();
  QList<MarkupData *> markups = source->markups();
  /* Markups are not necessarily nesting properly. But we have to clean that
     up now, because "Markups are <i>not <b>necessarily</i> nesting</b>"
     is not legal in html. One solution is to rewrite that as
     "Markups are <i>not </i><b><i>necessarily</i></b><b> nesting</b>."
     Of course one could clean that up a little, but I think there's no need.
     So: I'll simply close all at every markup edge.
     Oh. No. That won't work. I cannot arbitrarily close <a href> markups.*/

  QMap<int, QString> hrefs;
  QSet<int> edges;
  edges.insert(startidx);
  foreach (MarkupData *md, markups) {
    int s = md->start();
    if (s<startidx)
      s=startidx;
    edges.insert(s);
    int e = md->end();
    if (endidx>=0 && e>endidx)
      e=endidx;
    edges.insert(e);
    if (md->style()==MarkupData::Link || md->style()==MarkupData::FootnoteRef)
      hrefs[s] = txt.mid(md->start(), md->end()-md->start());
  }
  int e = txt.size();
  if (endidx>=0 && e>endidx)
    e=endidx;
  edges.insert(e);
  
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
    case MarkupData::Normal: tag = ""; break;
    }
    startTags[md->start()].insert(tag);
    endTags[md->end()].insert(tag);
  }

  foreach (int x, startTags.keys()) {
    QSet<QString> dups;
    foreach (QString tag, startTags[x])
      if (endTags[x].contains(tag) && !tag.startsWith("a"))
	dups << tag;
    foreach (QString tag, dups) {
      startTags[x].remove(tag);
      endTags[x].remove(tag);
    }
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

  html << ("<span class=\"" + cls + "\">\n");
  
  foreach (int edge, edgeList) {
    QList<QString> starts = startTags[edge].toList(); qSort(starts);
    QList<QString> ends = endTags[edge].toList(); qSort(ends);
    for (int i=ends.size()-1; i>=0; --i) {
      QString tag = ends[i];
      if (!tag.isEmpty()) {
	int idx = tag.indexOf(" ");
	html << ((idx>=0) ? ("</" + tag.left(idx) +">") : ("</" + tag + ">"));
      }
    }
    for (int i=0; i<starts.size(); ++i) {
      QString tag = starts[i];
      if (tag=="a") 
	html << ("<a href=\"" + Qt::escape(hrefs[edge]) + "\">");
      else if (tag == "a #")
	html << ("<a href=\"#" + Qt::escape(hrefs[edge]) + "\" class=\"footnoteref\">");
      else if (!tag.isEmpty())
	html << ("<" + tag + ">");
    }
    QString txt = textBits[edge];
    if (endidx>=0 && endidx<edge+txt.size())
      txt = txt.left(endidx-edge);
    if (startidx>edge)
      txt = txt.mid(startidx-edge);
    html << Qt::escape(txt);
  }  

  html << "</span>\n";

}

void HtmlOutput::add(FootnoteData const *source, ResManager const *resmgr) {
  html << "<div class=\"footnote\">\n";
  html << "<span class=\"tag\">\n";
  html << "<a name=\"" << Qt::escape(source->tag()) << "\"></a>";
  html << Qt::escape(source->tag());
  html << ":</span>\n";
  add(source->text(), resmgr);
  html << "</div>\n";
}
