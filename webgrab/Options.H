// Options.H

#ifndef OPTIONS_H

#define OPTIONS_H

#include <QStringList>

class Options {
public:
  Options(int &argc, char **&argv);
  bool paginate;
  enum { Tiny, Small, Medium, Large, Huge } imSize;
  QString url;
  QStringList out;
  void usage();
  
};

#endif
