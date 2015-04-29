// TextItemDoc.cpp

#include "TextItemDoc.h"
#include "TextItemDocData.h"
#include "TextData.h"
#include <QRegExp>
#include <QFontMetricsF>
#include <QPainter>
#include <math.h>
#include "MarkupEdges.h"
#include "Style.h"
#include <QDebug>
#include "TextCursor.h"
#include "TableItemDoc.h"
#include "TableData.h"

TextItemDoc::TextItemDoc(TextData *data, QObject *parent):
  QObject(parent), d(new TextItemDocData(data)) {
  d->linestarts = d->text->lineStarts();
  qDebug() << "TextItemDoc constructor" << d->linestarts;
  if (d->linestarts.isEmpty())
    relayout();
}

TextItemDoc::~TextItemDoc() {
  delete d;
}

void TextItemDoc::setFont(QFont const &f) {
  d->setBaseFont(f);
}

QFont TextItemDoc::font() const {
  return d->baseFont;
}

void TextItemDoc::setIndent(double pix) {
  d->indent = pix;
}

double TextItemDoc::indent() const {
  return d->indent;
}

void TextItemDoc::setWidth(double pix) {
  d->width = pix;
}

double TextItemDoc::width() const {
  return d->width;
}

void TextItemDoc::setLeftMargin(double pix) {
  d->leftmargin = pix;
}

double TextItemDoc::leftMargin() const {
  return d->leftmargin;
}

void TextItemDoc::setRightMargin(double pix) {
  d->rightmargin = pix;
}

double TextItemDoc::rightMargin() const {
  return d->rightmargin;
}

void TextItemDoc::setLineHeight(double pix) {
  qDebug() << "TID: lh=" << pix;
  d->lineheight = pix;
}

double TextItemDoc::lineHeight() const {
  return d->lineheight;
}

void TextItemDoc::setColor(QColor const &c) {
  d->color = c;
}

QColor TextItemDoc::color() const {
  return d->color;
}

QRectF TextItemDoc::boundingRect() const {
  return d->br;
}

void TextItemDoc::relayout(bool preserveWidth) {
  if (!preserveWidth)
    d->forgetWidths();
  
  /* We'll relayout the entire text. We are not handling tables yet. */
  QVector<double> charwidths = d->charWidths();

  /* Let's find splittable positions. */
  QVector<int> bitstarts;
  QVector<QString> bits;
  QVector<QString> caps;
  QVector<bool> parbefore;

  QString txt = d->text->text();

  qDebug() << "TID::relayout" << txt;
  QVector<double> cw = charwidths;
  cw.resize(5);
  qDebug() << cw;


  QRegExp re(QString::fromUtf8("[-/— \n]"));
  int off = 0;
  parbefore << true;
  while (off>=0) {
    int start = off;
    bitstarts << start;
    off = re.indexIn(txt, off);
    if (off<0) {
      // last
      bits << txt.mid(start);
      caps << "";
    } else {
      QString cap = re.cap();
      bits << txt.mid(start, off-start);
      caps << cap;
      off = off + cap.length();
      parbefore << cap.contains("\n");
    }
  }

  /* Next, find the widths of all the bits and caps */
  int N = bits.size();
  QVector<double> widths;
  QVector<double> capwidths;
  widths.resize(N);
  capwidths.resize(N);
  for (int i=0; i<N; i++) {
    int k0 = bitstarts[i];
    QString bit = bits[i];
    int K = bit.size();
    double w = 0;
    for (int k=0; k<K; k++)
      w += charwidths[k+k0];
    widths[i] = w;

    k0 += K;
    QString cap = caps[i];
    K = cap.size();
    w = 0;
    for (int k=0; k<K; k++)
      w += charwidths[k+k0];
    capwidths[i] = w;
  }

  QVector<double> ww = widths;
  ww.resize(5);
  qDebug() << ww;
  ww = capwidths;
  ww.resize(5);
  qDebug() << ww;
  
  /* Now, let's lay out some paragraphs... */
  QVector<int> linestarts;
  qDebug() << d->width << d->leftmargin << d->rightmargin << d->indent;
  for (int idx=0; idx<bits.size(); ) {
    // let's lay out one line
    double availwidth = d->width - d->leftmargin - d->rightmargin;
    qDebug() << "availwidth" << availwidth;
    if (parbefore[idx])
      availwidth -= d->indent;
    linestarts << bitstarts[idx];
    double usedwidth = 0;
    QString line = "";
    while (idx<bits.size()) {
      // let's add words to the line
      if (usedwidth==0) {
        // at start of line, unconditionally add
        line += bits[idx] + caps[idx];
        usedwidth += widths[idx] + capwidths[idx];
        idx++;
      } else if (parbefore[idx]) {
        break;
      } else {
        double nextwidth = widths[idx];
	bool trivcap = QString(" \n").contains(caps[idx]);
	if (!trivcap)
	  nextwidth += capwidths[idx]; // must fit the cap as well
        if (usedwidth+nextwidth < availwidth) {
          line += bits[idx];
          usedwidth += nextwidth;
	  if (trivcap)
	    usedwidth += capwidths[idx]; // it wasn't counted before
          idx++;
        } else {
          break;
        }
      }
    }
    qDebug() << line << usedwidth;
  }

  qDebug() << linestarts;
  
  d->linestarts = linestarts;

  if (d->writable)
    d->text->setLineStarts(linestarts);

  d->br = QRectF(QPointF(0, 0),
                 QSizeF(d->width, linestarts.size()*d->lineheight + 4));

  qDebug() << d->br;
}

void TextItemDoc::partialRelayout(int /* start */, int /* end */) {
  relayout();
}

template <typename T> int findLastLE(QVector<T> const &vec, T key) {
  /* Given a sorted vector, returns the index of the last element in the
     vector that does not exceed key.
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

template <typename T> int findFirstGT(QVector<T> const &vec, T key) {
  /* Given a sorted vector, returns the index of the first element in the
     vector that exceeds key.
     Returns -1 if there is no such element.
   */
  int N = vec.size();
  if (N==0 || vec[N-1]<=key)
    return -1;
  else if (vec[0]>key)
    return 0;
  // So now I now that the first element is <=key and the final element >key
  // That means that this will return nonnegative:
  int k = findLastLE(vec, key);
  return k+1;
}

QPointF TextItemDoc::locate(int offset) const {
  Q_ASSERT(!d->linestarts.isEmpty());
  
  QVector<double> const &charw = d->charWidths();
  QVector<int> const &starts = d->linestarts;
  int line = findLastLE(starts, offset);
  Q_ASSERT(line>=0);
  double ytop = line * d->lineheight + 4;
  double ascent = d->fonts().metrics(MarkupStyles())->ascent();
  double xl = d->leftmargin;
  int pos = starts[line];
  bool startpar = line==0 || text()[pos-1]==QChar('\n');
  if (startpar)
    xl += d->indent;

  while (pos<offset)
    xl += charw[pos++];
  return QPointF(xl, ytop + ascent);
}

int TextItemDoc::find(QPointF xy, bool strict) const {
  Q_ASSERT(!d->linestarts.isEmpty());

  double w = d->width - d->leftmargin - d->rightmargin;
  double x = xy.x() - d->leftmargin;

  if (strict)
    if (x<0 || x>w)
      return -1;

  if (x<0)
    x = 0;
  else if (x>w)
    x = w;

  double y = xy.y() - 4;
  int line = int(y/d->lineheight);

  if (strict)
    if (line<0)
      return -1;
  
  QVector<int> const &starts = d->linestarts;
  int h = starts.size();

  if (strict)
    if (line>=h)
      return -1;

  if (line<0)
    return 0;

  int N = d->text->text().size();

  if (line>=h)
    return N;

  QVector<double> const &charw = d->charWidths();

  int pos = starts[line];
  int npos = line+1<h ? starts[line+1] : N;

  bool startpar = line==0 || text()[pos-1]==QChar('\n');
  if (startpar)
    x -= d->indent;

  qDebug() << "TID:find" << x << y;
  
  double x0 = 0;
  while (pos<npos) {
    double x1 = x0 + charw[pos];
    if (x0+x1>=2*x) // past the halfway point of the character?
      return pos;
    pos++;
    x0 = x1;
  }
  return pos>=N ? N : npos-1; // return position at end of line
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
  memcpy(cw1.data()+offset+dN, cw0.data()+offset,
	 (N0-offset)*sizeof(double));
  d->setCharWidths(cw1);

  d->text->setText(t0.left(offset) + text + t0.mid(offset));
  foreach (MarkupData *md, d->text->markups()) 
    md->update(N0, 0, dN);
      
  d->recalcSomeWidths(offset, offset+dN);

  relayout(true);
  /* Really what we should do is try to preserve most linestarts. */

  emit contentsChange(offset, 0, dN);
}

void TextItemDoc::remove(int offset, int length) {
  /* Removes text from the document, updating the MarkupData,
     character width table, and line starts.
  */

  if (offset<0) {
    length += offset;
    offset = 0;
  }
  
  QString t0 = d->text->text();

  if (length+offset > t0.size())
    length = t0.size() - offset;
  
  if (length<=0)
    return;

  QVector<double> cw0 = d->charWidths();
  int N0 = cw0.size();
  Q_ASSERT(N0==t0.size());
  int dN = length;
  
  d->text->setText(t0.left(offset) + t0.mid(offset+length));
  foreach (MarkupData *md, d->text->markups()) 
    md->update(N0, dN, 0);

  QVector<double> cw1(N0 - dN);
  memcpy(cw1.data(), cw0.data(), offset*sizeof(double));
  memcpy(cw1.data()+offset, cw0.data()+offset+dN,
	 (N0-offset-dN)*sizeof(double));
  d->setCharWidths(cw1);

  relayout(true);
  /* Really what we should do is try to preserve most linestarts. */

  emit contentsChange(offset, dN, 0);
}

static QColor alphaBlend(QColor base, QColor over) {
  double r0 = base.redF();
  double g0 = base.greenF();
  double b0 = base.blueF();
  double a0 = base.alphaF();

  double r1 = over.redF();
  double g1 = over.greenF();
  double b1 = over.blueF();
  double a1 = over.alphaF();

  /* Equations from wikipedia */
  double a = 1 - (1-a0)*(1-a1);
  double r = a1/a*r1 + (1-a1/a)*r0;
  double g = a1/a*g1 + (1-a1/a)*g0;
  double b = a1/a*b1 + (1-a1/a)*b0;
  
  QColor out;
  out.setRgbF(r, g, b, a);
  return out;
}

void TextItemDoc::render(QPainter *p, QRectF roi) const {
  QString txt = d->text->text();
  if (roi.isNull())
    roi = p->clipBoundingRect();
  qDebug() << "TextItemDoc::render" << roi << txt;
  qDebug() << "  " << d->linestarts;
  int N = d->linestarts.size();

  FontVariants &fonts = d->fonts();
  double ascent = fonts.metrics(MarkupStyles())->ascent();
  double xheight = fonts.metrics(MarkupStyles())->xHeight();
  double descent = fonts.metrics(MarkupStyles())->descent();
  QVector<double> const &cw = d->charWidths();

  int n0 = floor((roi.top()-4)/d->lineheight);
  int n1 = ceil(roi.bottom()/d->lineheight);
  if (n0<0)
    n0 = 0;
  if (n1>d->linestarts.size())
    n1 = d->linestarts.size();
  
  int k0 = d->linestarts[n0];

  QList<TransientMarkup> tmm;
  if (d->selstart>=0)
    tmm << TransientMarkup(d->selstart, d->selend, MarkupData::Selected);
  MarkupEdges edges(d->text->markups(), tmm);
  MarkupStyles style;
  foreach (int k, edges.keys()) 
    if (k<k0)
      style = edges[k];
    else
      break;

  qDebug() << edges.keys();
  
  for (int n=n0; n<n1; n++) {
    int start = d->linestarts[n];
    int end = (n+1<N) ? d->linestarts[n+1] : txt.size();
    double ytop = n*d->lineheight + 4;
    double ybase = ytop + ascent;
    double ybottom = ybase + descent;
    
    bool parstart = n==0 || txt[start-1]=='\n';
    double x = parstart ? d->indent : 0;
    x += d->leftmargin;
    QString line = txt.mid(start, end-start);

    QVector<int> nowedges;
    QVector<MarkupStyles> nowstyles;
    if (!edges.contains(start)) {
      nowedges << start;
      nowstyles << style;
    }

    foreach (int k, edges.keys()) {
      if (k>=start && k<end) {
        style = edges[k];
        nowedges << k;
        nowstyles << style;
      } else if (k>=end) {
        break;
      }
    }
    nowedges << end;

    qDebug() << "line " << n;
    qDebug() << nowedges;
    QVector<int> ddd;
    foreach (MarkupStyles s, nowstyles)
      ddd << s.toInt();
    qDebug() << ddd;
    
    
    int Q = nowedges.size()-1;
    for (int q=0; q<Q; q++) {
      QString bit = line.mid(nowedges[q] - start,
                             nowedges[q+1] - nowedges[q]);
      while (bit.endsWith("\n"))
        bit = bit.left(bit.size()-1);
      MarkupStyles const &s = nowstyles[q];
      double y0 = s.contains(MarkupData::Superscript) ? ybase + xheight*.7
        : s.contains(MarkupData::Subscript) ? ybase - xheight *.5
        : ybase;

      double x0 = x;
      for (int k=nowedges[q]; k<nowedges[q+1]; k++)
        x += cw[k];

      QColor bgcol("#ffffff"); bgcol.setAlphaF(0);
      Style const &st(d->text->style());
      if (s.contains(MarkupData::DeadLink)) 
        bgcol = alphaBlend(bgcol, st.alphaColor("hover-not-found"));
      else if (s.contains(MarkupData::Link)) 
        bgcol = alphaBlend(bgcol, st.alphaColor("hover-found"));
      if (s.contains(MarkupData::Emphasize))
        bgcol = alphaBlend(bgcol, st.alphaColor("emphasize"));
      if (s.contains(MarkupData::SearchResult))
        bgcol = alphaBlend(bgcol, st.alphaColor("transientshading"));
      if (s.contains(MarkupData::Selected))
        bgcol = alphaBlend(bgcol, st.alphaColor("selected"));
      if (bgcol.alpha()>0) {
        p->setPen(QPen(Qt::NoPen));
        p->setBrush(bgcol);
        p->drawRect(QRectF(QPointF(x0, ytop), QPointF(x, ybottom)));
        p->setBrush(Qt::NoBrush);
      }

      if (s.contains(MarkupData::FootnoteRef))
        p->setPen(QPen(d->text->style().color("customref-color")));
      else
        p->setPen(QPen(color()));
      
      if (s.contains(MarkupData::StrikeThrough)) {
        QFont f(*fonts.font(s));
        f.setStrikeOut(true);
        p->setFont(f);
      } else {
        p->setFont(*fonts.font(s));
      }
       
      p->drawText(QPointF(x0, y0), bit);
    }
  }
}

QString TextItemDoc::text() const {
  return d->text->text();
}

QVector<int> TextItemDoc::lineStarts() const {
  Q_ASSERT(!d->linestarts.isEmpty());
  return d->linestarts;
}

int TextItemDoc::lineStartFor(int pos) const {
  QVector<int> starts = lineStarts();
  int line = findLastLE(starts, pos);
  Q_ASSERT(line>=0);
  return starts[line];
}

int TextItemDoc::lineEndFor(int pos) const {
  QVector<int> starts = lineStarts();
  int line = findFirstGT(starts, pos);
  if (line<0)
    return text().size();
  else
    return starts[line]-1;
}

bool TextItemDoc::isEmpty() const {
  return d->text->isEmpty();
}

QChar TextItemDoc::characterAt(int pos) const {
  return d->text->text()[pos];
}

int TextItemDoc::find(QString s) const {
  return text().indexOf(s);
}

void TextItemDoc::setSelection(TextCursor const &c) {
  if (c.hasSelection()) {
    d->selstart = c.selectionStart();
    d->selend = c.selectionEnd();
  } else {
    clearSelection();
  }
}

void TextItemDoc::clearSelection() {
  d->selstart = d->selend = -1;
}

void TextItemDoc::makeWritable() {
  d->writable = true;
}

TextItemDoc *TextItemDoc::create(TextData *data, QObject *parent) {
  TableData *tabledata = dynamic_cast<TableData *>(data);
  if (tabledata)
    return new TableItemDoc(tabledata, parent);
  else
    return new TextItemDoc(data, parent);
}
