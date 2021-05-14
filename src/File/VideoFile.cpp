// VideoFile.cpp

#include "VideoFile.h"

#include <QEventLoop>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QAbstractVideoSurface>
#include <QVideoFrame>

class KeyGrabber: public QAbstractVideoSurface {
public:
  bool present(QVideoFrame const &frame) override;
  QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType) const override;
  QImage const &image() const { return img; }
private:
  QImage img;
};

QList<QVideoFrame::PixelFormat> KeyGrabber::supportedPixelFormats(QAbstractVideoBuffer::HandleType ht) const {
  qDebug() << "supportedpixfmts" << ht;
  
  return QList<QVideoFrame::PixelFormat>{QVideoFrame::Format_RGB24};
}

bool KeyGrabber::present(QVideoFrame const &f) {
  qDebug() << "kg:present";
  if (img.isNull()) {
    qDebug() << "kg:present got frame";
    QVideoFrame frame(f); // gotta copy; otherwise I cannot map to main memory
    frame.map(QAbstractVideoBuffer::ReadOnly);
    QImage img0(frame.bits(),
                frame.width(),
                frame.height(),
                frame.bytesPerLine(),
                QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()));
    img = QImage(img0); img.detach(); // this way, the data survive after
                                      // the frame is unmapped
    qDebug() << "kg:present got image" << img.size();
    frame.unmap();
    // Note: From Qt 5.15, there is a simple QVideoFrame::image.
  }
  setError(StoppedError); // might as well stop right here
  return true;
  //  return false;
}

VideoFile::VideoFile(QUrl url): url(url) {
  tested = false;
}

bool VideoFile::checkValidity() {
  qDebug() << "videofile checkvalidity" << tested << valid;
  if (tested)
    return valid;

  KeyGrabber kg;
  QMediaPlayer mp;
  mp.setVideoOutput(&kg);
  mp.setMedia(url);
  mp.setVolume(0);

  QEventLoop el;
  QMediaPlayer::MediaStatus s = mp.mediaStatus();
  QObject::connect(&mp,  &QMediaPlayer::mediaStatusChanged,
                   [&s, &el](QMediaPlayer::MediaStatus s1) {
                     qDebug() << "videofile status change";
                     s = s1;
                     el.exit();
                   });
  dur = mp.duration();
  QObject::connect(&mp, &QMediaPlayer::durationChanged,
            [this](int t_ms) {
              dur = t_ms / 1000.0;
            });
  
  mp.play();
  QTimer tm;
  QObject::connect(&tm, &QTimer::timeout,
                   [&el]() {
                     qDebug() << "videofile timeout";
                     el.exit();
                   });
  tm.start(500);
  el.exec(QEventLoop::ExcludeUserInputEvents);
  tested = true;
  keyimg = kg.image();
  valid = !keyimg.isNull();
  qDebug() << "videofile returned" << tested << valid << s << keyimg.size();
  return valid;
}

QImage VideoFile::keyImage() const {
  return keyimg;
}

bool VideoFile::plausiblyVideo(QUrl url) {
  QString path = url.path();
  return path.endsWith(".mov")
    || path.endsWith(".mp4")
    || path.endsWith(".avi")
    || path.endsWith(".webm");
}

      
double VideoFile::duration() const {
  return dur;
}
