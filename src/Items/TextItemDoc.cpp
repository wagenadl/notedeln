// TextItemDoc.cpp

#include "TextItemDoc.h"
#include "TextData.h"
#include <QRegExp>
#include <QFontMetricsF>
#include "MarkupEdgeMap.h"
#include <QVector>

class TextItemDocData {
public:
  TextData *data;
  QFont font_;
  double indent_;
  double width_;
  double lineheight_;
  QColor color_;
  QRectF br;
public:
  TextItemDocData(TextData *data): data(data) { }
  QVector<double> const &charWidths() const;
  void forgetMetrics() { metrics_.clear(); forgetWidths(); }
  void forgetWidths() { charwidths.clear(); }
  QMap<MarkupData::Styles, QFontMetricsF> const &metrics() const;
  // map will contain Normal, Italic, Bold, and Superscript and combinations
  static MarkupData::Styles simplifiedStyle(MarkupData::Styles s);
private:
  mutable QMap<MarkupData::Styles, QFontMetricsF> metrics_;
  mutable QVector<double> charwidths;
};

MarkupData::Styles TextItemDocData::simplifiedStyle(MarkupData::Styles s) {
  if (s & MarkupData::Subscript)
    s |=  MarkupData::Superscript;
  return s & (MarkupData::Italic | MarkupData::Bold | MarkupData::Italic);
}

static QFont italicVersion(QFont f) {
  f.setStyle(QFont::StyleItalic);
  return f;
}

static QFont boldVersion(QFont f) {
  f.setWeight(QFont::Bold);
  return f;
}

static QFont scriptVersion(QFont f) {
  f.setPixelSize(0.8*f.pixelSize());
  return f;
}

QVector<double> const &TextItemDocData::charWidths() const {
  if (!charwidths.isEmpty()) {
    // should I assert that the text is unchanged somehow?
    return charwidths;
  }
  
  /* Returns widths for every character. */
  /* Currently does not yet do italics correction, but it will. */
  
  QMap<MarkupData::Styles, QFontMetricsF> const &fms = metrics();
  QMap<int, MarkupData::Styles> ends;
  QMap<int, MarkupData::Styles> starts;
  foreach (MarkupData *m, data->markups()) {
    if (m->end()>m->start())
      starts[m->start()] |= m->style();
    ends[m->end()] |= m->style();
  }

  MarkupData::Styles current = MarkupData::Normal;
  QMap<MarkupData::Styles, QFontMetricsF>::const_iterator currentit
    = fms.find(MarkupData::Normal);
  QString text = data->text();
  int N = text.size();
  charwidths.resize(N);
  for (int n=0; n<N; n++) {
    QChar c = text[n];
    if (ends.contains(n)) {
      current &= ~ends[n];
      currentit = fms.find(simplifiedStyle(current));
    }
    if (starts.contains(n)) {
      current |= ends[n];
      currentit = fms.find(simplifiedStyle(current));
    }

    charwidths[n] = (*currentit).width(c);
  }
  return charwidths;  
}

QMap<MarkupData::Styles, QFontMetricsF> const
&TextItemDocData::metrics() const {
  if (metrics_.isEmpty()) {
    metrics_.insert(MarkupData::Normal, QFontMetricsF(font_));
    metrics_.insert(MarkupData::Italic, QFontMetricsF(italicVersion(font_)));
    metrics_.insert(MarkupData::Bold, QFontMetricsF(boldVersion(font_)));
    metrics_.insert(MarkupData::Bold | MarkupData::Italic,
                   QFontMetricsF(italicVersion(boldVersion(font_))));
    metrics_.insert(MarkupData::Superscript,
                   QFontMetricsF(scriptVersion(font_)));
    metrics_.insert(MarkupData::Superscript | MarkupData::Italic,
                   QFontMetricsF(italicVersion(scriptVersion(font_))));
    metrics_.insert(MarkupData::Superscript | MarkupData::Bold,
                   QFontMetricsF(boldVersion(scriptVersion(font_))));
    metrics_.insert(MarkupData::Superscript | MarkupData::Bold
                   | MarkupData::Italic,
                   QFontMetricsF(italicVersion
                                 (boldVersion(scriptVersion(font_)))));
  }
  return metrics_;
}

void TextItemDoc::setFont(QFont const &f) { d->font_ = f; d->forgetMetrics(); }
QFont TextItemDoc::font() const { return d->font_; }
void TextItemDoc::setIndent(double pix) { d->indent_ = pix; }
double TextItemDoc::indent() const { return d->indent_; }
void TextItemDoc::setWidth(double pix) { d->width_ = pix; }
double TextItemDoc::width() const { return d->width_; }
void TextItemDoc::setLineHeight(double pix) { d->lineheight_ = pix; }
double TextItemDoc::lineHeight() const { return d->lineheight_; }
void TextItemDoc::setColor(QColor const &c) { d->color_ = c; }
QColor TextItemDoc::color() const { return d->color_; }
QRectF TextItemDoc::boundingRect() const { return d->br; }

TextItemDoc::TextItemDoc(TextData *data): d(new TextItemDocData(data)) {
}

TextItemDoc::~TextItemDoc() {
  delete d;
}

void TextItemDoc::relayout() {
  d->forgetMetrics();
  
  /* We'll relayout the entire text. We are not handling tables yet. */
  QVector<double> charwidths = d->charWidths();

  /* Let's find splittable positions. */
  QList<int> bitstarts;
  QList<QString> bits;
  QList<bool> spacebefore;
  QList<bool> parbefore;

  QString text = d->data->text();
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

  QFontMetricsF fm(d->font_);
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
  QList<int> linestarts;
  for (int idx=0; idx<bits.size(); ) {
    double availwidth = d->width_;
    if (parbefore[idx] && d->indent_>0)
      availwidth -= d->indent_;
    else if (!parbefore[idx] && d->indent_<0)
      availwidth+= d->indent_;
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

  d->data->setLineStarts(linestarts);
}

void TextItemDoc::partialRelayout(int) {
  relayout();
}

