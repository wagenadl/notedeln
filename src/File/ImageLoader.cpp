// File/ImageLoader.cpp - This file is part of NotedELN

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

// ImageLoader.cpp

#include "ImageLoader.h"
#include <QThread>

class ILThread: public QThread {
public:
  ILThread(QString fn, QObject *parent): QThread(parent), fn(fn) { }
protected:
  void run() override;
public:
  QString fn;
  QImage img;
};

void ILThread::run() {
  img = QImage(fn);
}

ImageLoader::ImageLoader() {
  thr = 0;
}

ImageLoader::~ImageLoader() {
  // thr is deleted by Qt since it is our child
}

bool ImageLoader::loadThenDelete(QString fn) {
  if (thr)
    return false;
  thr = new ILThread(fn, this);
  connect(thr, &QThread::finished,
	  this, &ImageLoader::complete,
          Qt::QueuedConnection);
  thr->start();
  return true;
}

void ImageLoader::complete() {
  QImage img = thr->img;
  emit loaded(img);
  deleteLater();
}
