// App/EProcess.h - This file is part of eln

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

// EProcess.h

#ifndef EPROCESS_H

#define EPROCESS_H

#include <QStringList>

class EProcess {
public:
  EProcess();
  ~EProcess();
  void setWorkingDirectory(QString);
  void setNoStartMessage(QString);
  void setCommandAndArgs(QString, QStringList);
  void setWindowCaption(QString);
  bool exec();
  QString stdOut() const { return so; }
  QString stdErr() const { return se; }
private:
  QString msgNoStart;
  QString cmd;
  QStringList args;
  QString winCap;
  QString so;
  QString se;
  QString wd;
};

#endif
