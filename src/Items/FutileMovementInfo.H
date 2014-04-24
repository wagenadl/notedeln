// Items/FutileMovementInfo.H - This file is part of eln

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

// FutileMovementInfo.H

#ifndef FUTILEMOVEMENTINFO_H

#define FUTILEMOVEMENTINFO_H

#include <Qt>
#include <QPointF>

class FutileMovementInfo {
public:
  FutileMovementInfo();
  FutileMovementInfo(int key, Qt::KeyboardModifiers mod, class TextItem *src);
  QPointF scenePos() const;
  int key() const;
  int pos();
  Qt::KeyboardModifiers modifiers() const;
private:
  QPointF scenePos_;
  int key_;
  int pos_;
  Qt::KeyboardModifiers modifiers_;
};

#endif
