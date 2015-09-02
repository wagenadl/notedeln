// Scenes/Restacker.h - This file is part of eln

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

// Restacker.H

#ifndef RESTACKER_H

#define RESTACKER_H

#include <QList>
#include <QMap>
#include <QMultiMap>
#include <QSet>

class Restacker {
public:
  Restacker(QList<class BlockItem *> const &blocks, int start);
  void restackData();
  void restackItems(class EntryScene &es);
  static void sneakilyRepositionNotes(QList<class BlockItem *> const &blocks,
				      int sheet);
private:
  void restackBlocks();
  void restackFootnotesOnSheet();
  void restackBlock(int i);
  void restackBlockSplit(int i, double ysplit);
  void restackBlockOne(int i);
  void restackItem(EntryScene &es, int i);
private:
  QList<BlockItem *> const &blocks;
  int start;
  int end;
  double y0;
  double y1;
  double yblock; // top of next block
  double yfn; // bottom of next footnote
  int isheet; // sheet for next block & footnote
  QMap<int, QMultiMap<double, class FootnoteItem *> > footplace;
  // Maps sheet numbers to a map of reference positions to footnotes.
  // A reference position is the vertical position of the referring text
  // plus 0.001 * the horizontal position to break ties b/w multiple
  // references on the same line.
  QSet<int> changedSheets;
};

#endif
