// File/BackgroundVC.h - This file is part of NotedELN

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

// BackgroundVC.H

#ifndef BACKGROUNDVC_H

#define BACKGROUNDVC_H

#include <QObject>

class BackgroundVC: public QObject {
  Q_OBJECT;
public:
  BackgroundVC(QObject *parent);
  virtual ~BackgroundVC();
  bool commit(QString path, QString program); // returns true if started,
  // false if could not start or globally disabled
  void setTimeout(int secs);
  bool isBusy() const;
signals:
  void done(bool ok);
private slots:
  void processFinished();
  void processStderr();
  void processStdout();
  void timeout();
private:
  void cleanup(bool ok);
private:
  class QProcess *vc;
  class QTimer *guard;
  class DFBlocker *block;
  QString program;
  QString path;
  int step;
  int maxt_s;
};

#endif
