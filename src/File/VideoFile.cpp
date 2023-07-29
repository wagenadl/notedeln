// VideoFile.cpp

#include "VideoFile.h"

#include <QEventLoop>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#ifdef QT_MULTIMEDIA_LIB
#include <QMediaPlayer>
#include <QVideoSink>
#include <QVideoFrame>
#endif

VideoFile::VideoFile(QUrl url): url(url) {
  tested = false;
  dur = 0;
}

bool VideoFile::checkValidity() {
#ifdef QT_MULTIMEDIA_LIB
  qDebug() << "videofile checkvalidity" << tested << valid;
  if (tested)
    return valid;

  //  KeyGrabber kg;
  QVideoSink sink;
  QMediaPlayer mp;
  mp.setVideoOutput(&sink);
  mp.setAudioOutput(0);
  mp.setSource(url);

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
  QImage img;
  QObject::connect(&sink, &QVideoSink::videoFrameChanged,
                   [&sink, &img]() {
                     if (img.isNull()) {
                       img = sink.videoFrame().toImage();
                       qDebug() << "grabbed frame";
                     }
                   });
  tm.start(500);
  el.exec(QEventLoop::ExcludeUserInputEvents);
  tested = true;
  keyimg = img;
  valid = !keyimg.isNull();
  qDebug() << "videofile returned" << tested << valid << s << keyimg.size();
  return valid;
#else
  return false;
#endif
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
