// Items/FutileMovementInfo.cpp - This file is part of eln

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

// FutileMovementInfo.C

#include "FutileMovementInfo.H"
#include "TextItem.H"
#include <QTextCursor>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextLine>

FutileMovementInfo::FutileMovementInfo() {
}

FutileMovementInfo::FutileMovementInfo(int key, Qt::KeyboardModifiers mod,
				       TextItem *src) {
  key_ = key;
  modifiers_ = mod;
  if (src) {
    QTextCursor c = src->textCursor();
    QTextBlock b = c.block();
    QTextLayout *lay = b.layout();
    QPointF xy0 = lay->position(); // in item
    int p = c.positionInBlock();
    QTextLine line = lay->lineForTextPosition(p);
    QPointF xy(line.cursorToX(p), line.y()+line.ascent()); // in layout
    scenePos_ = src->mapToScene(xy0 + xy);
  }
}


QPointF FutileMovementInfo::scenePos() const {
  return scenePos_;
}

int FutileMovementInfo::key() const {
  return key_;
}

Qt::KeyboardModifiers FutileMovementInfo::modifiers() const {
  return modifiers_;
}
