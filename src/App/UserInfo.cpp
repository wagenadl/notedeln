// UserInfo.cpp

#include "UserInfo.h"
#include <QDebug>

namespace UserInfo {
#if defined(Q_OS_UNIX) || defined(Q_OS_MAC)
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
  
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
  
#elif defined(Q_OS_WIN32)

#include <windows.h>
#include <Lmcons.h>

  QString fullName() {
    char username[1024];
    DWORD username_len = 1023;
    if (GetUserNameEx(3, username, &username_len))
      return username;
    else if (GetUserName(username, &username_len)) // try login iname instead
      return username;
    else
      return "";
  }
    
#else

  QString fullName() {
    return "";
  }

#endif
  
}
