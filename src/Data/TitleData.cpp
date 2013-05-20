// TitleData.C

#include "TitleData.H"
#include "Style.H"
#include "Notebook.H"

static Data::Creator<TitleData> c("title");

TitleData::TitleData(Data *parent): Data(parent) {
  setType("title");
  TextData *v0 = new TextData(this);
  v0->setText(defaultTitle());
}

TitleData::~TitleData() {
}

QString TitleData::defaultTitle() {
  return "Untitled";
}

bool TitleData::isDefault() const {
  return children<TextData>().size()==1 && current()->text()==defaultTitle();
}

QList<TextData *> TitleData::versions() const {
  return children<TextData>();
}

TextData const *TitleData::current() const {
  return versions().last();
}

TextData *TitleData::current() {
  return versions().last();
}

TextData const *TitleData::orig() const {
  return versions()[0];
}

TextData *TitleData::revise() {
  QList<TextData *> vv = versions();
  TextData *r = vv.last();

  if (vv.size()==1 && r->text() == defaultTitle()) {
    r->setCreated(QDateTime::currentDateTime());
    r->setModified(QDateTime::currentDateTime());
    return r;
  }
  
  Notebook *b = book();
  double thr_h = b ? b->style().real("title-revision-threshold") : 6;
  if (r->modified().secsTo(QDateTime::currentDateTime()) < thr_h*60*60)
    return r;
  
  TextData *r0 = Data::deepCopy(r);
  insertChildBefore(r0, r); // reinsert the copy
  r->setCreated(QDateTime::currentDateTime());
  r->setModified(QDateTime::currentDateTime());
  markModified();
  return r;
}

  
