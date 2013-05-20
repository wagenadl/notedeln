// Data/NoteData.cpp - This file is part of eln

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

// NoteData.C

#include "NoteData.H"
#include "BlockData.H"

NoteData::NoteData(class BlockData *parent): Data(parent) {
  setType("note");
}

NoteData::~NoteData() {
}

double NoteData::x() const {
  return x_;
}

double NoteData::y() const {
  return y_;
}

QString NoteData::text() const {
  return text_;
}

void NoteData::setX(double x) {
  x_ = x;
  markModified();
}


void NoteData::setY(double y) {
  y_ = y;
  markModified();
}

void NoteData::setText(QString const &t) {
  text_ = t;
  markModified();
}
