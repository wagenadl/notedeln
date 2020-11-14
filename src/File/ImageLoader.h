// ImageLoader.h

#ifndef IMAGELOADER_H

#define IMAGELOADER_H

#include <QObject>
#include <QImage>

class ImageLoader: public QObject {
  Q_OBJECT;
public:
  ImageLoader();
  // ImageLoaders must be constructed using NEW; never as a local object
  // on the stack.
  // Do not reparent the ImageLoader---otherwise, Qt might try to delete it.
  virtual ~ImageLoader();
  // Do not manually delete the ImageLoader after calling loadThenDelete.
  bool loadThenDelete(QString fn);
  // LOADTHENDELETE may be called only once.
  // Returns true, except if already called before.
signals:
  void loaded(QImage img);
private slots:
  void complete();
private:
  class ILThread *thr;
};

#endif
