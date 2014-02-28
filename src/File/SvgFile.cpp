// File/SvgFile.cpp - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// SvgFile.cpp

#include "SvgFile.H"
#include <QTemporaryFile>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include "WebGrab.H"

namespace SvgFile {
  QImage downloadAsImage(QUrl const &url) {
    QTemporaryFile f(QDir::tempPath() + "/eln_XXXXXX.png");
    f.open(); // without this, no filename is generated
    QStringList args; args << "-l" << url.toString() << f.fileName();
    int res = QProcess::execute(WebGrab::executable(), args);
    if (res) {
      qDebug() << "SvgFile::downloadAsImage failed";
      return QImage();
    } else {
      return QImage(f.fileName());
    }
  }
}

