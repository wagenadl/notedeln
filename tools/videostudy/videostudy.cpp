#include <QApplication>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QDebug>
#include <QDateTime>

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  QMediaPlayer mp;
  QVideoWidget vw;
  qDebug() << QDateTime::currentDateTime();
  mp.setVideoOutput(&vw);
  //  mp.setMedia(QUrl("file:///home/wagenaar/Desktop/film.webm"));
  mp.setMedia(QUrl("file:///home/wagenaar/Desktop/water_test_anim1.mp4"));
  mp.setVolume(0);
  QObject::connect(&mp, &QMediaPlayer::videoAvailableChanged,
                   [](bool b) { qDebug() << QDateTime::currentDateTime() << "video available" << b; });
  QObject::connect(&mp, &QMediaPlayer::mediaStatusChanged,
                   [](QMediaPlayer::MediaStatus b) { qDebug() << QDateTime::currentDateTime() << "media status" << b; });
  qDebug() << mp.mediaStatus() << mp.isVideoAvailable();
  vw.show();
  mp.play();
  qDebug() << mp.mediaStatus() << mp.isVideoAvailable();
  app.exec();
  return 0;
}
