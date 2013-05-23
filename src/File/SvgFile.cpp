// SvgFile.cpp

#include "SvgFile.H"
#include <QTemporaryFile>
#include <QProcess>
#include <QDir>
#include <QDebug>

namespace SvgFile {
  QImage downloadAsImage(QUrl const &url) {
    QTemporaryFile f(QDir::tempPath() + "/eln_XXXXXX.png");
    f.open(); // without this, no filename is generated
    QStringList args; args << "-l" << url.toString() << f.fileName();
    int res = QProcess::execute("webgrab", args);
    if (res) {
      qDebug() << "SvgFile::downloadAsImage failed";
      return QImage();
    } else {
      return QImage(f.fileName());
    }
  }
}

