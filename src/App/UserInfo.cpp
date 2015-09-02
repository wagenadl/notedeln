// App/UserInfo.cpp - This file is part of eln

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
  
