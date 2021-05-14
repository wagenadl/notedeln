// Items/GfxVideoItem.cpp - This file is part of NotedELN

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

// GfxVideoItem.C

#include "GfxVideoItem.h"
#include "GfxVideoData.h"
#include "Mode.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QKeyEvent>
#include "ResManager.h"
#include <QGraphicsVideoItem>
#include <QMediaPlayer>
#include "ToolItem.h"

static Item::Creator<GfxVideoData, GfxVideoItem> c("gfxvideo");

GfxVideoItem::GfxVideoItem(GfxVideoData *data, Item *parent):
  GfxImageItem(data, parent) {
  setCropAllowed(false);
  player = 0;
  vidmap = 0;
  playbutton = new ToolItem();
  playbutton->setSvg(":icons/video-play.svg");
  playbutton->setParentItem(this);
  playbutton->setZValue(100);
  playbutton->setAlpha(0.6);
  repositionPlayButton();
  connect(playbutton, &ToolItem::leftClick,
          [this]() { playVideo(); });

  ResManager *resmgr = data->resManager();
  if (!resmgr) {
    qDebug() << "GfxVideoItem: no resource manager";
    return;
  }
  Resource *res = resmgr->byTag(data->resName());
  if (!res) {
    qDebug() << "GfxVideoItem: missing resource" << data->resName();
    return;
  }
  image.load(res->previewPath());
  qDebug() << "VideoItem loaded image" << image.size();
  pixmap->setPixmap(QPixmap::fromImage(image));
}

GfxVideoItem::~GfxVideoItem() {
}

void GfxVideoItem::loadVideo() {
  bool firsttime = !vidmap;

  if (firsttime) {
    player = new QMediaPlayer(this);
    vidmap = new QGraphicsVideoItem(this);
    player->setVideoOutput(vidmap);
    vidmap->setAcceptedMouseButtons(0);
  }
  
  // get the video
  ResManager *resmgr = data()->resManager();
  if (!resmgr) {
    qDebug() << "GfxVideoItem: no resource manager";
    return;
  }
  Resource *res = resmgr->byTag(data()->resName());
  if (!res) {
    qDebug() << "GfxVideoItem: missing resource" << data()->resName();
    return;
  }
  player->setMedia(QUrl::fromLocalFile(res->archivePath()));

  if (firsttime) {
    connect(player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            [](QMediaPlayer::Error error) {
              qDebug() << "player error" << error;
            });
    connect(player, &QMediaPlayer::stateChanged,
            [this](QMediaPlayer::State state) {
              qDebug() << "player state" << state;
              if (state==QMediaPlayer::StoppedState)
                playbutton->show();
            });
  }
}

void GfxVideoItem::playVideo() {
  if (player) {
    if (player->state() == QMediaPlayer::PausedState) {
      player->play();
      return;
    } else if (player->state() == QMediaPlayer::PlayingState) {
      player->pause();
      return;
    }
  }
  loadVideo();
  qDebug() << "pixmap size" << pixmap->boundingRect();
  qDebug() << "vidmap size" << vidmap->boundingRect();
  vidmap->setSize(pixmap->boundingRect().size());
  playbutton->hide();
  player->play();
}

void GfxVideoItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  qDebug() << playbutton->boundingRect() << playbutton->pos() << playbutton->isVisible();
  // click starts the video in certain modes, unless shift or control is held
  if ((mode()->mode()==Mode::Browse || mode()->mode()==Mode::Type)
      && !(e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))) {
    playVideo();
    e->accept();
  } else {
    GfxImageItem::mousePressEvent(e);
  }
}

void GfxVideoItem::repositionPlayButton(double s) { 
  if (s<=0) 
    s = data()->scale();
  double x = 0;
  double y = data()->height();
  QRectF r = playbutton->boundingRect();
  x += 1/s*r.width()*.1;
  y -= 1/s*r.height()*1.1;
  playbutton->setScale(1/s);
  playbutton->setPos(x, y);
  qDebug() << "gvi scale" << x << y << s << r.width() << r.height();
}

void GfxVideoItem::setScale(double s) {
  GfxImageItem::setScale(s);
  repositionPlayButton(s);
}
