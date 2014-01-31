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
#include "BlockData.H"
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
  html << "<title>" << escape(pageTitle) << "</title>";
  html << "<link rel=\"stylesheet\" href=\"" << local
       << "/eln.css\" type=\"text/css\">\n";
  html << "<meta http-equiv=\"Content-Type\" "
       << "content=\"text/html;charset=utf-8\" >\n";
  html << "</head>\n";
  html << "\n";

  res = QDir(outputFile);
  if (res.exists()) 
    removeDirRecursively(res);
  res.mkpath(res.absolutePath());

  QFile cssin(":eln.css");
  cssin.copy(res.absoluteFilePath("eln.css"));
}

void HtmlOutput::removeDirRecursively(QDir dir) {
  foreach(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot
                                            | QDir::System | QDir::Hidden
                                            | QDir::AllDirs | QDir::Files,
                                            QDir::DirsFirst)) 
    if (info.isDir()) 
      removeDirRecursively(QDir(info.absoluteFilePath()));
    else 
      QFile::remove(info.absoluteFilePath());
  dir.rmdir(dir.absolutePath());
}

HtmlOutput::~HtmlOutput() {
  html.setDevice(0); // close before file gets deleted
}

void HtmlOutput::add(EntryScene *source) {
  html << "<div class=\"entry\">\n";
  html << "<div class=\"date\">"
       << source->data()->created()
    .toString(source->style().string("date-format"))
       << "</div>\n";
  html << "<div class=\"title\">" << escape(source->title()) << "</div>\n";

  ResManager const *resmgr = source->data()->resManager();

  QList<GfxNoteData const *> gfxnotes;
  collectGfxNotes(source->data()->title(), gfxnotes);

  int sheet0 = source->currentSheet();
  int sheet = sheet0;
  
  foreach (BlockItem const *b, source->blocks()) {
    if (b->data()->sheet() != sheet)
      source->gotoSheet(sheet = b->data()->sheet());
    if (dynamic_cast<TableBlockItem const *>(b))
      add(dynamic_cast<TableBlockItem const *>(b), resmgr);
    else if (dynamic_cast<TextBlockItem const *>(b))
      add(dynamic_cast<TextBlockItem const *>(b), resmgr);
    else if (dynamic_cast<GfxBlockItem const *>(b))
      add(dynamic_cast<GfxBlockItem const *>(b), resmgr);
    QList<GfxNoteData const *> herenotes;
    foreach (GfxNoteData const *nd, gfxnotes) {
      if (nd->sheet() <= b->data()->sheet() &&
          nd->y() < b->data()->y0() + b->data()->height()) {
        herenotes << nd;
        add(nd, resmgr);
      }
    }
    foreach (GfxNoteData const *nd, herenotes)
      gfxnotes.removeOne(nd);
  }

  if (sheet!=sheet0)
    source->gotoSheet(sheet0);

  foreach (GfxNoteData const *nd, gfxnotes) 
    add(nd, resmgr);

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

void HtmlOutput::collectGfxNotes(Data const *src,
                                 QList<GfxNoteData const *> &dst) {
  GfxNoteData const *nd = dynamic_cast<GfxNoteData const *>(src);
  if (nd)
    dst << nd;
  foreach (Data const *d, src->allChildren())
    collectGfxNotes(d, dst);
}

void  HtmlOutput::addGfxNotes(Data const *data, ResManager const *resmgr) {
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

void HtmlOutput::addRefStart(QString key, ResManager const *resmgr) {
  Resource *r = resmgr->byTag(key);
  if (r) {
    QUrl url = r->sourceURL();
    if (url.isValid()) 
      html << "<a href=\"" << escape(url.toString()) << "\">";
    else 
      html << "<span class=\"badurl\">"; // is this acceptable at all?
  } else {
    if (key.startsWith("http://") || key.startsWith("https://"))
      html << "<a href=\"" << escape(key) << "\">";
    else if (key.startsWith("www"))
      html << "<a href=\"http://" << escape(key) << "\">";
    else
      html << "<span class=\"nores\">";
  }
}

void HtmlOutput::addRefEnd(QString key, ResManager const *resmgr) {
  Resource *r = resmgr->byTag(key);
  if (r) {
    QUrl url = r->sourceURL();
    if (url.isValid()) 
      html << "</a>";
    else
      html << "</span>";
    if (r->hasArchive()) {
      QString fn = r->archivePath();
      QFile resfile(fn);
      int idx = fn.lastIndexOf("/");
      if (idx>=0)
        fn = fn.mid(idx+1);
      resfile.copy(res.absoluteFilePath(fn));
      html << " <a href=\"" << escape(local) << "/" << fn << "\">"
           << "(archived)</a>";
    }
  } else {
    if (key.startsWith("http://") || key.startsWith("https://"))
      html << "</a>";
    else if (key.startsWith("www"))
      html << "</a>";
    else
      html << "</span>";
  }
}
  

void HtmlOutput::addRef(QString key, ResManager const *resmgr) {
  addRefStart(key, resmgr);
  html << escape(key) << "</a>";
  addRefEnd(key, resmgr);
}

void HtmlOutput::add(GfxBlockItem const *source, ResManager const *resmgr) {
  html << "<div class=\"gfxblock\">\n";
  html << "<div class=\"gfx\">\n";
  QRectF r = source->sceneBoundingRect();
  r |= source->mapRectToScene(source->childrenBoundingRect());
  QImage img((r.size()*1.25).toSize(),
	     QImage::Format_ARGB32);
  QPainter p(&img);
  source->scene()->render(&p, QRectF(), r);
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
      addRef(key, resmgr);
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
  for (unsigned int r=0; r<td->rows(); r++) {
    html << "<tr>\n";
    for (unsigned int c=0; c<td->columns(); c++) {
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
  QMap<int, QString> hrefends;
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
    if (md->style()==MarkupData::Link || md->style()==MarkupData::FootnoteRef) {
      hrefs[s] = txt.mid(md->start(), md->end()-md->start());
      hrefends[e] = txt.mid(md->start(), md->end()-md->start());
    }
      
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
        if (tag == "a") 
          addRefEnd(hrefends[edge], resmgr);
        else
          html << ((idx>=0) ? ("</" + tag.left(idx) +">") : ("</" + tag + ">"));
      }
    }
    for (int i=0; i<starts.size(); ++i) {
      QString tag = starts[i];
      if (tag=="a")
        addRefStart(hrefs[edge], resmgr);
      else if (tag == "a #")
	html << ("<a href=\"#" + escape(hrefs[edge]) + "\" class=\"footnoteref\">");
      else if (!tag.isEmpty())
	html << ("<" + tag + ">");
    }
    QString txt = textBits[edge];
    if (endidx>=0 && endidx<edge+txt.size())
      txt = txt.left(endidx-edge);
    if (startidx>edge)
      txt = txt.mid(startidx-edge);
    html << escape(txt);
  }  

  html << "</span>\n";

}

void HtmlOutput::add(FootnoteData const *source, ResManager const *resmgr) {
  html << "<div class=\"footnote\">\n";
  html << "<span class=\"tag\">\n";
  html << "<a name=\"" << escape(source->tag()) << "\"></a>";
  html << escape(source->tag());
  html << ":</span>\n";
  add(source->text(), resmgr);
  html << "</div>\n";
}

QString HtmlOutput::escape(QString x) {
#if QT_VERSION >= 0x050000
  return x.toHtmlEscaped();
#else
  return Qt::escape(x);
#endif
}
