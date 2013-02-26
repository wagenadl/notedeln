// VersionControl.H

#ifndef VERSIONCONTROL_H

#define VERSIONCONTROL_H

#include <QString>

namespace VersionControl {
  bool update(QString path, QString program); // returns true on success
  bool commit(QString path, QString program); // returns true on success
};

#endif
