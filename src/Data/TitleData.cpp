// Data/TitleData.cpp - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// TitleData.C

#include "TitleData.h"
#include "Style.h"
#include "Notebook.h"
#include <QDebug>

static Data::Creator<TitleData> c("title");

TitleData::TitleData(Data *parent): Data(parent) {
  setType("title");
  TextData *v0 = new TextData(this);
  connect(v0, &TextData::mod, this, &TitleData::textMod);
}

TitleData::~TitleData() {
}

void TitleData::loadMore(QVariantMap const &vm) {
  Data::loadMore(vm);
  connect(text(), &TextData::mod, this, &TitleData::textMod);
}
  
bool TitleData::isDefault() const {
  /* Check if we are properly initialized */
  TextData const *txt = text();
  return !txt || txt->text()=="";
}

TextData const *TitleData::text() const {
  return firstChild<TextData>();
}

TextData *TitleData::text() {
  return firstChild<TextData>();
}
