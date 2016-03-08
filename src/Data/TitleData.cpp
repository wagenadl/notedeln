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

#include "TitleData.h"
#include "Style.h"
#include "Notebook.h"
#include <QDebug>

static Data::Creator<TitleData> c("title");

TitleData::TitleData(Data *parent): Data(parent) {
  qDebug() << "TitleData" << this << parent;
  setType("title");
  TextData *v0 = new TextData(this);
  v0->setText(defaultTitle());
  connect(v0, SIGNAL(mod()), this, SIGNAL(textMod()));
}

TitleData::~TitleData() {
}

void TitleData::loadMore(QVariantMap const &vm) {
  Data::loadMore(vm);
  connect(text(), SIGNAL(mod()), this, SIGNAL(textMod()));
}
  
QString TitleData::defaultTitle() {
  return "";
}

bool TitleData::isDefault() const {
  qDebug() << "isDefault" << this;
  return children<TextData>().size()==1 && text()->text()==defaultTitle();
}

TextData const *TitleData::text() const {
  return children<TextData>().last();
}

TextData *TitleData::text() {
  return children<TextData>().last();
}
