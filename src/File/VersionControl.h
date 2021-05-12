// File/VersionControl.H - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// VersionControl.H

#ifndef VERSIONCONTROL_H

#define VERSIONCONTROL_H

#include <QString>

namespace VersionControl {
  bool update(QString path, QString program);
  /* If update fails, queries the user. User can quit the program, decide
     to edit anyway, or open the notebook read-only.
     Return value is true if update is OK or if user chooses to edit anyway,
     false if user chooses to open read-only.
     Does not return if user chooses to quit.
  */

  bool commit(QString path, QString program); // returns true on success

  bool isGitAvailable();
  void globallyDisable(); // causes update and commit to quietly return success
  bool isGloballyDisabled();
};

#endif
