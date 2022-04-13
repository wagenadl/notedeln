// Items/GfxVideoItem.H - This file is part of NotedELN

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

// GfxVideoItem.H

#ifndef GFXVIDEOITEM_H

#define GFXVIDEOITEM_H

#include "GfxImageItem.h"
#include "GfxVideoData.h"
#include <QMediaPlayer>

class GfxVideoItem: public GfxImageItem {
  Q_OBJECT;
public:
  GfxVideoItem(GfxVideoData *data, Item *parent=0);
  virtual ~GfxVideoItem();
  DATAACCESS(GfxVideoData);
  virtual void setScale(double);
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
private:
  void loadVideo();
  void playVideo();
  void repositionAnnotation(double s=0);
protected slots:
  void dragSlider(double pos_s);
  void durationChange(int t_ms);
  void showTime();
private:
  class QMediaPlayer *player;
  class QGraphicsVideoItem *vidmap;
  //  class ToolItem *playbutton;
  class QGraphicsTextItem *annotation;
  class VideoSlider *slider;
  bool neverplayed;
};

#endif
