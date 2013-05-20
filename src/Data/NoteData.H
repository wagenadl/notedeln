// Data/NoteData.H - This file is part of eln

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

// NoteData.H

#ifndef NOTEDATA_H

#define NOTEDATA_H

#include "Data.H"

class NoteData: public Data {
  Q_OBJECT;
  Q_PROPERTY(double x READ x WRITE setX)
  Q_PROPERTY(double y READ y WRITE setY)
  Q_PROPERTY(QString text READ text WRITE setText)
public:
  // constructor and destructor  
  NoteData(class BlockData *parent=0);
  virtual ~NoteData();
  // read properties
  double x() const;
  double y() const;
  QString text() const;
  // write properties
  void setX(double);
  void setY(double);
  void setText(QString const &);
protected:
  double x_;
  double y_;
  QString text_;
};

#endif
