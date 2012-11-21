// Options.H

#ifndef OPTIONS_H

#define OPTIONS_H

#include <QStringList>

class Options {
public:
  Options(int &argc, char **&argv);
  bool paginate;
  int imSize;
  QString url;
  QStringList out;
  void usage();
  
};

#endif
