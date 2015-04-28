// File/JSONParser.H - This file is part of eln

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

// JSONParser.H

#ifndef JSONPARSER_H

#define JSONPARSER_H

#include <QString>
#include <QVariant>

class JSONParser {
public:
  class Error {
  public: 
    QString message;
    int line; // counted from zero
    int col; // counted from zero; -1 means end of line
  public:
    Error(QString msg, int l, int c): message(msg), line(l), col(c) { }
    void report() const;
  };   
private:
  QString input;
  QVariant result;
  int currentOffset;
  int currentLine; // counted from zero
  int currentLineStartOffset; // start of current line
public:
  JSONParser(QString);
  bool atEnd() const throw();
  QVariantMap readObject() throw(Error);
  QVariantList readArray() throw(Error);
  QVariant readAny() throw(Error);
  void assertEnd() const throw(Error);
protected:
  QString readString() throw(Error);
  QVariant readNumber() throw(Error);
  QVariant readValue(QString exp="value") throw(Error);
  void skipWhite() throw();
  QChar peekNext() const throw(Error);
  QChar getNext() throw(Error);
  bool conditionalReadLiteral(QString s) throw(Error);
  void assertNext() const throw(Error);
  void makeError(QString msg, bool atPrev=false) const throw(Error);
};


#endif
