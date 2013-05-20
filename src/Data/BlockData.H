// Data/BlockData.H - This file is part of eln

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

// BlockData.H

#ifndef BLOCKDATA_H

#define BLOCKDATA_H

#include "Data.H"

class BlockData: public Data {
  Q_OBJECT;
  Q_PROPERTY(double y0 READ y0 WRITE setY0)
  Q_PROPERTY(double h READ height WRITE setHeight)
  Q_PROPERTY(int sheet READ sheet WRITE setSheet)
public:
  // constructor and destructor  
  BlockData(Data *parent=0);
  virtual ~BlockData();
  // read properties
  double y0() const;
  double height() const;
  int sheet() const;
  // QList<class NoteData *> notes() const;
  // write properties
  void setY0(double);
  void setHeight(double);
  void sneakilySetY0(double); 
  void sneakilySetHeight(double);
  /* The two "sneakilySet" functions serve to change the stored y0 and
     height on objects that were created before the height of blocks or
     the y0 of notes was stored in the data structure.
     In the spirit of never writing to old files, we are careful to not
     write those data when such files are opened. But the modern EntryScene
     and friends cannot function without meaningful height and y0 information,
     so we do write the information into the Data tree. We just do not
     write it to file.
     These functions should never be called on blocks created after May 18,
     2013, but I am not at present explicitly ASSERTing that in the code.
     At some point, I should think more carefully about allowing the Data
     tree to write changes to old files at all. At present, this protection
     doesn't exist, and it is up to the Items to behave well. That is not
     such a great idea, if only because the Items comprise twice as much code
     as the Data.
  */
  void setSheet(int);
  // other
  // void addNote(NoteData *);
  // bool deleteNote(NoteData *);
  virtual bool isEmpty() const;
signals:
  void newSheet(int); // not emitted during loading
private:
  double y0_;
  double h_;
  int sheet_;
};

#endif
