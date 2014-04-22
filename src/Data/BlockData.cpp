// Data/BlockData.cpp - This file is part of eln

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

// BlockData.C

#include "BlockData.H"
#include "EntryData.H"
#include "NoteData.H"

BlockData::BlockData(Data *parent): Data(parent) {
  y0_ = 0;
  h_ = 0;
  sheet_ = -1;
  setType("block");
}

BlockData::~BlockData() {
  // QObject will delete the notes
}

double BlockData::y0() const {
  return y0_;
}

double BlockData::height() const {
  return h_;
}

int BlockData::sheet() const {
  return sheet_;
}

void BlockData::setY0(double y0) {
  y0_ = y0;
  markModified(InternalMod);
}

void BlockData::setHeight(double h) {
  h_ = h;
  markModified(InternalMod);
}

void BlockData::sneakilySetY0(double y0) {
  y0_ = y0;
}

void BlockData::sneakilySetHeight(double h) {
  h_ = h;
}

void BlockData::setSheet(int sheet) {
  sheet_ = sheet;
  markModified(InternalMod);
  if (!loading())
    emit newSheet(sheet);
}

bool BlockData::isEmpty() const {
  return true;
}

bool BlockData::setSheetAndY0(int n, double y0) {
  bool chg = false;
  if (n!=sheet_) {
    sheet_ = n;
    chg = true;
  }
  if (y0!=y0_) {
    y0_ = y0;
    chg = true;
  }
  if (chg)
    markModified(InternalMod);
  return chg;
}
