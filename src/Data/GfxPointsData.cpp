// Data/GfxPointsData.cpp - This file is part of eln

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

// GfxPointsData.C

#include "GfxPointsData.H"
#include <QPointF>

static Data::Creator<GfxPointsData> c("gfxpoints");

GfxPointsData::GfxPointsData(Data *parent):
  GfxData(parent) {
  setType("gfxpoints");
}

GfxPointsData::~GfxPointsData() {
}

QList<double> const &GfxPointsData::xx() const {
  return xx_;
}

QList<double> const &GfxPointsData::yy() const {
  return yy_;
}

void GfxPointsData::clear() {
  xx_.clear();
  yy_.clear();
  markModified();
}

int GfxPointsData::size() const {
  return xx_.size();
}

QPointF GfxPointsData::point(int i) const {
  return QPointF(xx_[i], yy_[i]);
}

void GfxPointsData::addPoint(QPointF p, bool hush) {
  xx_.append(p.x());
  yy_.append(p.y());
  if (!hush)
    markModified();
}

void GfxPointsData::setPoint(int i, QPointF p, bool hush) {
  xx_[i] = p.x();
  yy_[i] = p.y();
  if (!hush)
    markModified();
}

void GfxPointsData::removePoint(int i) {
  xx_.removeAt(i);
  yy_.removeAt(i);
  markModified();
}

void GfxPointsData::loadMore(QVariantMap const &src) {
  GfxData::loadMore(src);
  xx_.clear();
  yy_.clear();

  foreach (QVariant v, src["xx"].toList())
    xx_.append(v.toDouble());
  foreach (QVariant v, src["yy"].toList())
    yy_.append(v.toDouble());
}

void GfxPointsData::saveMore(QVariantMap &dst) const {
  GfxData::saveMore(dst);
  QVariantList xl;
  foreach (double v, xx_)
    xl.append(QVariant(v));
  dst["xx"] = QVariant(xl);

  QVariantList yl;
  foreach (double v, yy_)
    yl.append(QVariant(v));
  dst["yy"] = QVariant(yl);
}
  
  
