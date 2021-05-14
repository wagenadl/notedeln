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
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>
  
static Item::Creator<GfxVideoData, GfxVideoItem> c("gfxvideo");

GfxVideoItem::GfxVideoItem(GfxVideoData *data, Item *parent):
  GfxImageItem(data, parent) {
  setCropAllowed(false);
  player = 0;
  vidmap = 0;
  neverplayed = true;
  annotation = new QGraphicsTextItem("▶");
  showTime();
  annotation->setFont(style().font("text-font"));
  annotation->setDefaultTextColor(QColor(255,255,128)); //style().color("text-color"));
  QGraphicsDropShadowEffect *eff = new QGraphicsDropShadowEffect;
  eff->setColor(QColor(0,0,0));
  eff->setOffset(QPointF(0, 0));
  eff->setBlurRadius(6);
  annotation->setGraphicsEffect(eff);

  annotation->setParentItem(this);
  annotation->setZValue(100);
  repositionAnnotation();

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
              showTime();
            });
    connect(player, &QMediaPlayer::durationChanged,
            [this](int t_ms) {
              if (t_ms>0 && data()->isWritable())
                data()->setDur(t_ms/1000.0);
              showTime();
            });
    connect(player, &QMediaPlayer::positionChanged,
            [this](int t_ms) {
              showTime();
            });
    player->setNotifyInterval(100);
    if (data()->dur()==0 && data()->isWritable())
      data()->setDur(player->duration()/1000.0);
    showTime();
  }
}

void GfxVideoItem::playVideo() {
  neverplayed = false;
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
  player->play();
}

void GfxVideoItem::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  // click starts the video in certain modes, unless shift or control is held
  if ((mode()->mode()==Mode::Browse || mode()->mode()==Mode::Type)
      && !(e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))) {
    playVideo();
    e->accept();
  } else {
    GfxImageItem::mousePressEvent(e);
  }
}

void GfxVideoItem::repositionAnnotation(double s) { 
  if (s<=0) 
    s = data()->scale();
  double x = 0;
  double y = data()->height();
  QRectF r = annotation->boundingRect();
  x += 1/s*r.height()*.1;
  y -= 1/s*r.height()*1.1;
  annotation->setScale(1/s);
  annotation->setPos(x, y);
}

void GfxVideoItem::setScale(double s) {
  GfxImageItem::setScale(s);
  repositionAnnotation(s);
}

void GfxVideoItem::showTime() {
  QMediaPlayer::State state = player ? player->state()
    : QMediaPlayer::StoppedState;
  double t_s = data()->dur();
  if (player) {
    if (state==QMediaPlayer::StoppedState) {
      if (player->duration()>0)
        t_s = player->duration()/1000.0;
    } else {
      t_s = player->position()/1000.0;
    }
  }

  if (t_s==0 && state==QMediaPlayer::StoppedState) {
    annotation->setPlainText("▶"); // big version
    return;
  }

  int h = t_s/3600;
  t_s -= h*3600;
  int m = t_s/60;
  t_s -= m*60;
  int s = t_s;
  t_s -= s;
  int ds = t_s*10;
  QString t = state==QMediaPlayer::PausedState ? "⏸"
    : state==QMediaPlayer::PlayingState ? "⏵"
    : neverplayed ? "▶"
    : "▶"; // "⏹";
  t += " ";
  
  if (h)
    t += QString("%1:").arg(h);
  t += QString("%1:%2.%3")
    .arg(m, h ? 2 : 1, 10, QChar('0'))
    .arg(s, 2, 10, QChar('0'))
    .arg(ds);
  annotation->setPlainText(t);
}
