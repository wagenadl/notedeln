// TextItemDoc.cpp

#include "TextItemDoc.h"
#include "TextData.h"
#include <QRegExp>
#include <QFontMetricsF>
#include "MarkupEdgeMap.h"

TextItemDoc::TextItemDoc(TextData *data): data(data) {
}

void TextItemDoc::relayout() {
  /* We'll relayout the entire text. We are not handling tables yet. */
  /* First, let's find splittable positions. */
  QList<int> bitstarts;
  QList<QString> bits;
  QList<bool> spacebefore;
  QList<bool> parbefore;

  QString text = data->text();
  QRegExp re(QString::fromUtf8("[-— \n] *"));
  int off = 0;
  spacebefore << false;
  parbefore << true;
  while (off>=0) {
    int start = off;
    bitstarts << start;
    off = re.indexIn(text, off);
    if (off<0) {
      // last
      bits << text.mid(start);
    } else {
      QString cap = re.cap();
      bits << text.mid(start, off-start);
      off = off + cap.length();
      parbefore << cap.contains("\n");
      spacebefore << cap.contains(" ");
    }
  }

  /* Find the edges that matter for each bit */
  QList< QMap<int, MarkupData::Styles> > edgesperbit;
  QMap<int, MarkupData::Styles> edgemap
    = MarkupEdgeMap(data->markups()).edges();
  QList<int> edges = edgemap.keys();
  for (int i=0; i<bits.size(); i++) {
    QMap<int, MarkupData::Styles> myedges;
    MarkupData::Styles atstart = MarkupData::Normal;
    MarkupData::Styles atend  = MarkupData::Normal;
    int bitstart = bitstarts[i];
    int bitend = bitstart + bits[i].size();
    foreach (int e, edges) {
      if (e<=bitstart)
        atstart = edgemap[e];
      else if (e>=bitstart && e<bitend)
        myedges[e-bitstart] = edgemap[e];
      else if (e==bitend) 
        atend = edgemap[e];
    }
    myedges[0] = atstart;
    myedges[bitend-bitstart] = atend;
    edgesperbit << myedges;
  }

  QFontMetricsF fm(font_);
  double spacewidth = fm.width(" ");
  double itcorrection = spacewidth / 6;
  
  /* Next, find the widths of all the bits */
  QList<double> widths;
  for (int i=0; i<bits.size(); i++) {
    if (bits[i].isEmpty()) {
      widths << 0;
    } else {
      QList<int> edges = edgesperbit[i].keys();
      QList<MarkupData::Styles> styles = edgesperbit[i].values();
      double w = 0;
      for (int k=0; k<edges.size()-1; k++) {
        QString subbit = bits[i].mid(edges[k], edges[k+1]-edges[k]);
        MarkupData::Styles st = styles[k];
        QFont f = font_;
        if (st & MarkupData::Italic)
          f.setStyle(QFont::StyleItalic);
        if (st & MarkupData::Bold)
          f.setWeight(QFont::Bold);
        if ((st & MarkupData::Superscript) || (st & MarkupData::Subscript))
          f.setPixelSize(0.7*f.pixelSize());
        QFontMetricsF fm(f);
        if (k>0 && (st & MarkupData::Italic)
            && !(styles[k-1] & MarkupData::Italic))
          w += itcorrection;
        w += fm.width(subbit);
      }
      if ((styles[styles.size()-2] & MarkupData::Italic)
          && !(styles[styles.size()-1] & MarkupData::Italic))
        w += itcorrection;
      widths << w;
    }
  }
  
  /* Now, let's lay out some paragraphs... */
  QList<int> linestarts;
  for (int idx=0; idx<bits.size(); ) {
    double availwidth = width_;
    if (parbefore[idx] && indent_>0)
      availwidth -= indent_;
    else if (!parbefore[idx] && indent_<0)
      availwidth+= indent_;
    linestarts << bitstarts[idx];
    double usedwidth = 0;
    while (idx<bits.size()) {
      // let's lay out a line
      if (usedwidth==0) {
        // at start of line, unconditionally add
        idx++;
        usedwidth += widths[idx];
      } else {
        double nextwidth = widths[idx] + (spacebefore[idx] ? spacewidth : 0);
        if (usedwidth+nextwidth < availwidth) {
          idx++;
          usedwidth += nextwidth;
        } else {
          break;
        }
      }
    }
  }

  data->setLineStarts(linestarts);
}

void TextItemDoc::partialRelayout(int) {
  relayout();
}
