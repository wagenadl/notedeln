// File/JSONParser.cpp - This file is part of NotedELN

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

// JSONParser.C

#include "JSONParser.h"
#include <QDebug>

void JSONParser::Error::report() const {
  QString txt = message + " at l. " + QString::number(line) + " (";
  if (col<0) 
    txt += "end of line";
  else
    txt += "c. " + QString::number(col);
  txt += ")";
  qDebug() << txt;
}

JSONParser::JSONParser(QString i): input(i) {
  currentOffset = 0;
  currentLine = 1;
  currentLineStartOffset = 0;
}

void JSONParser::makeError(QString msg, bool atPrev) const {
  int l = currentLine;
  int c = currentOffset-currentLineStartOffset;
  if (atPrev)
    if (--c<0)
      l--;
  throw Error(msg, l, c);
}

bool JSONParser::conditionalReadLiteral(QString s) {
  if (input.mid(currentOffset, s.size()) == s) {
    currentOffset += s.size();
    if (atEnd())
      return true;
    if (peekNext().isLetterOrNumber())
      makeError("Bad termination of literal value");
    skipWhite();
    return true;
  } else {
    return false;
  }
}

bool JSONParser::atEnd() const throw() {
  return currentOffset>=input.size();
}

void JSONParser::assertEnd() const {
  if (!atEnd())
    makeError("Expected EOF");
}

void JSONParser::assertNext() const {
  if (atEnd())
    makeError("Unexpected EOF");
}

QChar JSONParser::peekNext() const {
  assertNext();
  QChar r = input[currentOffset];
  if (r=='\r')
    r = '\n';
  return r;
}

QChar JSONParser::getNext() {
  assertNext();
  QChar r = input[currentOffset++];
  if (r=='\n' || r=='\r') {
    currentLine++;
    currentLineStartOffset = currentOffset;
    if (!atEnd() && input[currentOffset].unicode() + r.unicode() == '\n' + '\r')
      currentLineStartOffset = ++currentOffset;
    r = '\n';
  }
  return r;
}

void JSONParser::skipWhite() throw() {
  while (currentOffset<input.size()) {
    switch (input[currentOffset].unicode()) {
      case ' ': case '\t':
        currentOffset++;
        break;
      case '\n': case '\r':
        currentLine++;
        currentOffset++;
	currentLineStartOffset = currentOffset;
	if (currentOffset<input.size()
	    && input[currentOffset].unicode() + input[currentOffset-1].unicode()
	       == '\n' + '\r') {
	  currentOffset++;
	  currentLineStartOffset = currentOffset;
	}
        break;
      default:
        return;
    }  
  }
}

QString JSONParser::readString() {
  if (getNext()!='"')
    makeError("Expected a string", true);
  QString res = "";
  for (QChar c = getNext(); c != '"'; c = getNext()) {
    if (c<' ') 
      makeError("Illegal control char inside string", true);
    else if (c=='\\') {
      switch (getNext().unicode()) {
        case 'b': res += '\b'; break;
        case 'f': res += '\f'; break;
        case 'n': res += '\n'; break;
        case 'r': res += '\r'; break;
        case 't': res += '\t'; break;
        case '\\': res += '\\'; break;
        case '/': res += '/'; break;
        case '"': res += '"'; break;
        case 'u': {
          int a = 0;
          for (int k=0; k<4; k++) {
	    a <<= 4;
            short x = getNext().unicode();
            if (x>='0' && x<='9')
              a += x - '0';
            else if (x>='A' && a<='F')
              a += 10 + x - 'A';
            else if (a>='a' && a<'f')
              a += 10 + x - 'a';
          }
          res += QChar(a);
        } break;
        default:
          makeError("Unexpected character after backslash", true);
      }
    } else {
      res += c;
    }
  }
  skipWhite();
  return res;
}

QVariant JSONParser::readNumber() {
  QString bit = "";
  bool isInt = true;
  while (!atEnd()) {
    QChar next = peekNext();
    if (next>='0' && next<='9') {
      bit += next;
      currentOffset++;
    } else if (next=='e' || next=='E' || next=='.' || next=='+' || next=='-') {
      isInt = false;
      bit += next;
      currentOffset++;
    } else {
      break;
    }
  }
  if (bit.isEmpty() || (!atEnd() && peekNext().isLetterOrNumber()))
    makeError("Expected a number");

  skipWhite();

  if (isInt)
    return QVariant(bit.toInt());
  bool ok;
  double v = bit.toDouble(&ok);
  if (!ok)
    makeError("Expected a number");
  return QVariant(v);
}

QVariant JSONParser::readValue(QString exp) {
  QChar c = peekNext();
  if (c=='"')
    return QVariant(readString());
  else if (c=='-' || (c>='0' && c<='9'))
    return readNumber();
  else if (conditionalReadLiteral("false"))
    return QVariant(false);
  else if (conditionalReadLiteral("true"))
    return QVariant(true);
  else if (conditionalReadLiteral("null"))
    return QVariant();
  else
    makeError("Expected a " + exp, false);
  return QVariant(); // not executed
}

QVariantMap JSONParser::readObject() {
  if (getNext()!='{') 
    makeError("Not an object", true);
  skipWhite();
  QVariantMap res;
  if (conditionalReadLiteral("}"))
    return res;
  while (true) {
    QString key = readString();
    if (getNext()!=':') 
      makeError("Expected colon");
    skipWhite();
    QVariant val = readAny();
    res[key] = val;
    switch (getNext().unicode()) {
      case '}':
	skipWhite();
	return res;
      case ',':
	skipWhite();
        continue;
      default:
        makeError("Expected comma or closing brace", true);
    }
  }
  return res; // not executed
}

QVariantList JSONParser::readArray() {
  if (getNext()!='[')
    makeError("Not an array", true);
  skipWhite();
  QVariantList res;
  if (conditionalReadLiteral("]"))
    return res;
  while (true) {
    QVariant val = readAny();
    res.append(val);
    switch (getNext().unicode()) {
      case ']':
	skipWhite();
	return res;
      case ',':
	skipWhite();
        continue;
      default:
        makeError("Expected comma or closing bracket", true);
    }
  }
  return res; // not reached
}

QVariant JSONParser::readAny() {
  switch (peekNext().unicode()) {
    case '{':
      return QVariant(JSONParser::readObject());
    case '[':
      return QVariant(JSONParser::readArray());
    default:
      return readValue("value, object, or array");
  }
}

#if 0
// djsonTest.C
#include <QDebug>

void test(QString x) {
  qDebug() << x;
  JSONParser p(x);
  try {
    QVariant v = p.readAny();
    qDebug() << v << "/" << p.atEnd();
  } catch (JSONParser::Error e) {
    e.report();
  }
  qDebug();
}

int main() {
  // good cases
  test("34");
  test("0.234");
  test("-20.234e2");
  test("true");
  test("null");
  test("\"Hello world\"");
  test("\"Hello\\tworld\\u0021\"");
  test("[ 1, 2, 3 ]");
  test("{ \"a\": 1, \"b\": 2 }");
  test("{ \"a\": 1, \"b\": [3, true ] }");
  test("{ \"a\": 1,\n \"b\": [3, true ] }");

  // bad cases
  test("{ \"a\": ");
  test(".23");
  test("17x");
  test("[1, 2, ]");
  test("{ \"a\": 1,}");
  return 0;
}

#endif
