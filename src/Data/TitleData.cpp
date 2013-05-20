// Data/TitleData.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

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

  
