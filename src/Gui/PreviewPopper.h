// App/PreviewPopper.H - This file is part of eln

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

// PreviewPopper.H

#ifndef PREVIEWPOPPER_H

#define PREVIEWPOPPER_H

#include <QWidget>

class PreviewPopper: public QObject {
  Q_OBJECT;
public:
  PreviewPopper(class Resource *res, QRect over, QObject *parent);
  virtual ~PreviewPopper();
  QWidget *popup();
  /* POPUP - Open popup immediately and return widget pointer
     Returns zero if resource not available.
  */
  void closeSoon();
  /* CLOSESOON - Close popup soon, unless mouse enters it. */
signals:
  void clicked(Qt::KeyboardModifiers);
  /* CLICKED - Emitted when the popup is clicked */
protected:
  virtual void timerEvent(QTimerEvent *);
private slots:
  void closeAndDie();
private:
  void smartPosition();
private:
  Resource *res;
  QRect over;
  class PopLabel *widget;
  int timerID;
};

#endif
