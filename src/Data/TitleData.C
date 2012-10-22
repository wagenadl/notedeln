// TitleData.C

#include "TitleData.H"
#include "Style.H"

#define DEFAULTTITLE "Untitled"

static Data::Creator<TitleData> c("title");

TitleData::TitleData(Data *parent): Data(parent) {
  versions_.append(new TextData(this));
  setType("title");
  versions_[0]->setText(DEFAULTTITLE);
}

TitleData::~TitleData() {
}

QList<TextData *> const &TitleData::versions() const {
  return versions_;
}

TextData const *TitleData::current() const {
  return versions_.last();
}

TextData *TitleData::current() {
  return versions_.last();
}

TextData const *TitleData::orig() const {
  return versions_[0];
}

TextData *TitleData::revise() {
  TextData *r = versions_.last();

  if (r->text() == DEFAULTTITLE) {
    r->setCreated(QDateTime::currentDateTime());
    r->setModified(QDateTime::currentDateTime());
    return r;
  }
  if (r->modified().secsTo(QDateTime::currentDateTime()) <
      Style::defaultStyle()["title-revision-threshold"].toDouble()*60*60)
    return r;
  
  TextData *r0 = Data::deepCopy(r);
  versions_.last() = r0; // store copy as previous version
  versions_.append(r);
  r->setCreated(QDateTime::currentDateTime());
  r->setModified(QDateTime::currentDateTime());
  markModified();
  return r;
}

void TitleData::loadMore(QVariantMap const &src) {
  foreach (TextData *v, versions_)
    delete v;
  versions_.clear();
  
  QVariantList vl = src["versions"].toList();
  foreach (QVariant v, vl) {
    TextData *t = new TextData(this);
    t->load(v.toMap());
    versions_.append(t);
  }
}

void TitleData::saveMore(QVariantMap &dst) const {
  QVariantList vl;
  foreach (TextData *t, versions_) {
    QVariantMap v = t->save();
    vl.append(v);
  }
  dst["versions"] = vl;
}
  
