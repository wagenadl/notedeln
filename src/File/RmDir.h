// RmDir.h

#ifndef RMDIR_H

#define RMDIR_H

#include <QString>

namespace RmDir {
  bool recurse(QString); // true if successful
  // Will refuse to delete anything that does not end in ".nb"
};

#endif
