// UserInfo.cpp

#include "UserInfo.h"
#include <QDebug>

#if defined(Q_OS_UNIX) || defined(Q_OS_MAC)

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
  
namespace UserInfo {
  QString fullName() {
    int uid = getuid();
    struct passwd pwd;
    char buf[1024];
    struct passwd *pwd_;
    if (getpwuid_r(uid, &pwd, buf, 1023, &pwd_) != 0) {
      qDebug() << "Failed to get user name for " << uid;
      return "";
    }
    
    QString gecos = pwd.pw_gecos;
    int idx = gecos.indexOf(",");
    QString name = (idx>=0) ? gecos.left(idx) : gecos;
    return name;
  }
}

#elif defined(Q_OS_WIN32)

#include <windows.h>
#include <tchar.h>
//#include <Lmcons.h>
#define SECURITY_WIN32
#include <security.h>

namespace UserInfo {
  QString fullName() {
    TCHAR username[1024];
    DWORD username_len = 1023;
    if (GetUserNameExW(NameDisplay, username, &username_len))
      return QString::fromWCharArray(username);
    //qDebug() << QString::fromWCharArray(username);
    //if (GetUserNameW(username, &username_len)) // try login iname instead
    //  return QString::fromWCharArray(username);
    return "";
  }
}

#else

namespace UserInfo {
  QString fullName() {
    return "";
  }
}

#endif
  
