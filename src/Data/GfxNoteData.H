// Data/GfxNoteData.H - This file is part of eln

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

// GfxNoteData.H

#ifndef GFXTEXTDATA_H

#define GFXTEXTDATA_H

#include "GfxData.H"
#include "TextData.H"

class GfxNoteData: public GfxData {
  Q_OBJECT;
  Q_PROPERTY(double dx READ dx WRITE setDx);
  Q_PROPERTY(double dy READ dy WRITE setDy);
  Q_PROPERTY(double textWidth READ textWidth WRITE setTextWidth);
  Q_PROPERTY(int sheet READ sheet WRITE setSheet);
public:
  GfxNoteData(class Data *parent=0);
  virtual ~GfxNoteData();
  double dx() const;
  double dy() const;
  double textWidth() const;
  int sheet() const;
  void setDx(double);
  void setDy(double);
  void setTextWidth(double);
  void setSheet(int);
  TextData const *text() const;
  TextData *text();
  QPointF delta() const;
  void setDelta(QPointF);
protected:
  void loadMore(QVariantMap const &src);
private:
  TextData *text_;
  double dx_, dy_;
  double textWidth_;
  int sheet_;
};

#endif
