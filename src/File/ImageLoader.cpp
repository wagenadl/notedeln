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
	  this, &ImageLoader::complete);
  thr->start();
  return true;
}

void ImageLoader::complete() {
  QImage img = thr->img;
  emit loaded(img);
  deleteLater();
}
