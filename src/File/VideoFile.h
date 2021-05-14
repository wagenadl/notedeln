// VideoFile.h

#ifndef VIDEOFILE_H

#define VIDEOFILE_H

#include <QUrl>
#include <QImage>

class VideoFile {
public:
  VideoFile(QUrl url);
  bool checkValidity(); // this is not fast, the first time around
  QImage keyImage() const;
  double duration() const; // in seconds, 0 if unknown
  static bool plausiblyVideo(QUrl url);
private:
  QUrl url;
  bool tested;
  bool valid;
  QImage keyimg;
  double dur;
};

#endif
