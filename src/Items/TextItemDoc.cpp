// TextItemDoc.cpp

#include "TextItemDoc.h"
#include "TextItemDocData.h"
#include "TextData.h"
#include <QRegExp>
#include <QFontMetricsF>
#include <QPainter>

void TextItemDoc::setFont(QFont const &f) { d->setBaseFont(f); }
QFont TextItemDoc::font() const { return d->baseFont; }
void TextItemDoc::setIndent(double pix) { d->indent = pix; }
double TextItemDoc::indent() const { return d->indent; }
void TextItemDoc::setWidth(double pix) { d->width = pix; }
double TextItemDoc::width() const { return d->width; }
void TextItemDoc::setLineHeight(double pix) { d->lineheight = pix; }
double TextItemDoc::lineHeight() const { return d->lineheight; }
void TextItemDoc::setColor(QColor const &c) { d->color = c; }
QColor TextItemDoc::color() const { return d->color; }
QRectF TextItemDoc::boundingRect() const { return d->br; }

TextItemDoc::TextItemDoc(TextData *data): d(new TextItemDocData(data)) {
  d->linestarts = d->text->lineStarts();
  if (d->linestarts.isEmpty())
    relayout();
}

TextItemDoc::~TextItemDoc() {
  delete d;
}

void TextItemDoc::relayout(bool preserveWidth) {
  if (!preserveWidth)
    d->forgetWidths();
  
  /* We'll relayout the entire text. We are not handling tables yet. */
  QVector<double> charwidths = d->charWidths();

  /* Let's find splittable positions. */
  QVector<int> bitstarts;
  QVector<QString> bits;
  QVector<bool> spacebefore;
  QVector<bool> parbefore;

  QString txt = d->text->text();
  QRegExp re(QString::fromUtf8("[-— \n] *"));
  int off = 0;
  spacebefore << false;
  parbefore << true;
  while (off>=0) {
    int start = off;
    bitstarts << start;
    off = re.indexIn(txt, off);
    if (off<0) {
      // last
      bits << txt.mid(start);
    } else {
      QString cap = re.cap();
      bits << txt.mid(start, off-start);
      off = off + cap.length();
      parbefore << cap.contains("\n");
      spacebefore << cap.contains(" ");
    }
  }

  /* Really, the spacewidth should be based on the local font,
     but for right now... */
  QFontMetricsF fm(d->baseFont);
  double spacewidth = fm.width(" ");
  
  /* Next, find the widths of all the bits */
  int N = bits.size();
  QVector<double> widths;
  widths.resize(N);
  for (int i=0; i<N; i++) {
    double w = 0;
    int k0 = bitstarts[i];
    QString bit = bits[i];
    int K = bit.size();
    for (int k=0; k<K; k++)
      w += charwidths[k+k0];
    widths[N] = w;
  }
  
  /* Now, let's lay out some paragraphs... */
  QVector<int> linestarts;
  for (int idx=0; idx<bits.size(); ) {
    double availwidth = d->width;
    if (parbefore[idx] && d->indent>0)
      availwidth -= d->indent;
    else if (!parbefore[idx] && d->indent<0)
      availwidth+= d->indent;
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

  d->linestarts = linestarts;
  // We *won't* copy it back to the TextData

  d->br = QRectF(QPointF(0, 0),
                 QSizeF(d->width, linestarts.size()*d->lineheight));
}

void TextItemDoc::partialRelayout(int /*offset*/) {
  relayout();
}

template <typename T> int findLastLE(QVector<T> const &vec, T key) {
  /* Given a sorted vector, returns the index of the last element in the vector
     that does not exceed key.
     Returns -1 if there is no such element.
   */
  int n0 = 0;
  int n1 = vec.size();
  if (n0==n1 || vec[n0]>key)
    return -1;
  while (n1>n0+1) {
    int nk = (n0+n1)/2;
    if (vec[nk]<=key)
      n0 = nk;
    else
      n1 = nk;
  }
  return n0;
}
  

QRectF TextItemDoc::locate(int offset) const {
  Q_ASSERT(!d->linestarts.isEmpty());
  
  QVector<double> const &charw = d->charWidths();
  QVector<int> const &starts = d->linestarts;
  int line = findLastLE(starts, offset);
  Q_ASSERT(line>=0);
  double ytop = line * d->lineheight;
  double ybot = ytop + d->lineheight;
  double xl = 0;
  int pos = starts[line];
  while (pos<offset)
    xl += charw[pos++];
  return QRectF(QPointF(xl-.5, ytop), QPointF(xl+.5, ybot));
}

int TextItemDoc::find(QPointF xy) const {
  Q_ASSERT(!d->linestarts.isEmpty());
  
  double x = xy.x();
  if (x<0 || x>d->width)
    return -1;

  double y = xy.y();
  int line = int(y/d->lineheight);
  if (line<0)
    return -1;

  QVector<int> const &starts = d->linestarts;
  if (line>=starts.size())
    return -1;

  QVector<double> const &charw = d->charWidths();
  

  int pos = starts[line];
  int npos = line+1<starts.size() ? starts[line+1] : d->text->text().size();
  double x0 = 0;
  while (pos<npos) {
    double x1 = x0 + charw[pos];
    if (x1>x)
      return pos;
    pos++;
  }
  return pos==npos ? npos : pos-1; // return position at end of line
  // rather than at start of next line if possible
}

void TextItemDoc::insert(int offset, QString text) {
  /* Inserts text into the document, updating the MarkupData,
     character width table, and line starts.
  */
     
  if (text.isEmpty())
    return;

  QString t0 = d->text->text();
  Q_ASSERT(offset<=t0.size());
  
  QVector<double> cw0 = d->charWidths();
  int N0 = cw0.size();
  Q_ASSERT(N0==t0.size());
  int dN = text.size();
  
  QVector<double> cw1(N0 + dN);
  memcpy(cw1.data(), cw0.data(), offset*sizeof(double));
  memcpy(&cw1[offset+dN], &cw0[offset],
	 (N0-offset)*sizeof(double));

  d->text->setText(t0.left(offset) + text + t0.mid(offset));
  foreach (MarkupData *md, d->text->markups()) 
    md->update(N0, 0, dN);
      
  d->recalcSomeWidths(N0, N0+dN);

  relayout(true);
  /* Really what we should do is try to preserve most linestarts. */
}

void TextItemDoc::remove(int offset, int length) {
  /* Removes text from the document, updating the MarkupData,
     character width table, and line starts.
  */

  if (length<=0)
    return;
  
  QString t0 = d->text->text();
  Q_ASSERT(offset+length <= t0.size());

  QVector<double> cw0 = d->charWidths();
  int N0 = cw0.size();
  Q_ASSERT(N0==t0.size());
  int dN = length;
  
  d->text->setText(t0.left(offset) + t0.mid(offset+length));
  foreach (MarkupData *md, d->text->markups()) 
    md->update(N0, dN, 0);

  QVector<double> cw1(N0 - dN);
  memcpy(cw1.data(), cw0.data(), offset*sizeof(double));
  memcpy(&cw1[offset], &cw0[offset+dN],
	 (N0-offset-dN)*sizeof(double));

  relayout(true);
  /* Really what we should do is try to preserve most linestarts. */
}
  
void TextItemDoc::render(QPainter *p, QRectF roi) const {
  QString txt = d->text->text();
  int N = d->linestarts.size();
  MarkupData::Styles style = MarkupData::Normal;

  QMap<int, MarkupData::Styles> ends;
  QMap<int, MarkupData::Styles> starts;
  foreach (MarkupData *m, d->text->markups()) {
    if (m->end()>m->start())
      starts[m->start()] |= m->style();
    ends[m->end()] |= m->style();
  }
  
  FontVariants &fonts = d->fonts();
  double ascent = fonts.metrics(style)->ascent();
  QVector<double> const &cw = d->charWidths();

  for (int n=0; n<N; n++) {
    int start = d->linestarts[n];
    int end = (n+1<N) ? d->linestarts[n+1] : txt.size();
    double ytop = n*d->lineheight;
    double ybot = (n+1)*d->lineheight;
    double ybase = ytop + ascent;
    if (ytop < roi.bottom() && ybot > roi.top()) {
      bool parstart = n==0 || txt[start-1]=='\n';
      double x = (parstart && d->indent>0) ? d->indent
        : (!parstart && d->indent<0) ? -d->indent
        : 0;
      QString line = txt.mid(start, end-start);

      QMap<int, MarkupData::Styles> edges;
      edges[start] = style;
      for (int k=start; k<end; k++) {
        if (ends.contains(k)) {
          style &= ~starts[k];
          edges[k] = style;
        }
        if (starts.contains(k)) {
          style |= starts[k];
          edges[k] = style;
        }
      }
      edges[end] = style;
      int Q = edges.size()-1;
      for (int q=0; q<Q; q++) {
        QString bit = line.mid(edges[q]-start, edges[q+1]-edges[q]);
        p->setFont(*fonts.font(edges[q]));
        p->drawText(QPointF(x, ybase), bit);
        for (int k=edges[q]; k<edges[q+1]; q++)
          x += cw[k];
      }
    } else if (ytop < roi.bottom()) {
      // update style; we'll need them later
      for (int k=start; k<end; k++) {
        if (ends.contains(k)) 
          style &= ~starts[k];
        if (starts.contains(k)) 
          style |= starts[k];
      }
    } else {
      break;
      // line below roi, we don't care
    }
  }
}
