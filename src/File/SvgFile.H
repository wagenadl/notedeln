// SvgFile.H

#ifndef SVGFILE_H

#define SVGFILE_H

#include <QImage>
#include <QUrl>

namespace SvgFile {
  QImage downloadAsImage(QUrl const &url);
};

#endif
