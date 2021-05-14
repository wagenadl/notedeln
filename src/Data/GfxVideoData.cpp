// Data/GfxVideoData.cpp - This file is part of NotedELN

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

// GfxVideoData.C

#include "GfxVideoData.h"
#include <QDebug>
#include <QImage>

static Data::Creator<GfxVideoData> c("gfxvideo");

GfxVideoData::GfxVideoData(Data *parent): GfxImageData(parent) {
  setType("gfxvideo");
  dur_ = 0;
}

GfxVideoData::GfxVideoData(QString resName,
                           QImage const &keyimg,
                           double dur,
                           Data *parent):
  GfxImageData(resName, keyimg, parent) {
  setType("gfxvideo");
  dur_ = dur;
}

GfxVideoData::~GfxVideoData() {
}

void GfxVideoData::setDur(double t_s) {
  if (dur_==t_s)
    return;
  dur_ = t_s;
  markModified();
}

double GfxVideoData::dur() const {
  return dur_;
}
