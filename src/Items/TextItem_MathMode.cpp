// Items/TextItem_MathMode.cpp - This file is part of NotedELN

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

// MathMode.cpp

#include "TextItem.h"
#include "TeXCodes.h"
#include "Digraphs.h"
#include <QKeyEvent>
#include <QDebug>

static bool isLatinLetter(QChar x) {
  return (x>='A' && x<='Z') || (x>='a' && x<='z');
}

static bool isLatinLetter(QString x) {
  return x.size()==1 && isLatinLetter(x[0]);
}

static bool isDigit(QChar x) {
  return (x>='0' && x<='9');
}

static bool isDigit(QString x) {
  return x.size()==1 && isDigit(x[0]);
}

void TextItem::letterAsMath(QString txt) {
  static QChar lastdup(' ');
  // txt can be a letter or a digit, actually
  const QString rquote = QString::fromUtf8("’");

  // we may italicize or deitalice
  QChar prevChar = document()->characterAt(cursor.position()-1);
  QChar antePrevChar = document()->characterAt(cursor.position()-2);
  if (isLatinLetter(prevChar)) {
    // previous was also a letter; potential deitalicize or bold face
    MarkupData *mdi = data()->markupAt(cursor.position(), MarkupData::Italic);
    MarkupData *mdb = data()->markupAt(cursor.position(), MarkupData::Bold);
    if (prevChar==txt[0] && !isLatinLetter(antePrevChar)
	&& antePrevChar!=rquote && prevChar!='m') {
      // we had the same letter before -> cycle faces
      // order is italic -> bold italic -> bold -> plain -> italic
      // but "mm" is redupped instead because millimeter is a unit
      lastdup = txt[0];
      if (mdb) {
	if (mdi) 
	  deleteMarkup(mdi);
	else
	  deleteMarkup(mdb);
      } else {
	if (mdi) 
	  addMarkup(MarkupData::Bold, cursor.position()-1,
		    cursor.position());
	else
	  addMarkup(MarkupData::Italic, cursor.position()-1,
		    cursor.position());
      }
    } else {
      // previous was _different_ letter; we'll deitalicize / debold; redup
      if (mdi)
	deleteMarkup(mdi);
      if (mdb) 
	deleteMarkup(mdb);
      if (mdb || (mdi && prevChar==QChar('a') && lastdup==QChar('a')))
        // redup double letters, including "a" for aardvark
	cursor.insertText(QString(prevChar));
      cursor.insertText(txt);
      if (prevChar=='d') { // magic for "dx"
	if (!isLatinLetter(antePrevChar)
	    && txt!="o") { // don't italicize d_o_.
	  addMarkup(MarkupData::Italic,
		    cursor.position()-txt.length(), cursor.position());
	}
      }
    }
  } else {
    // previous was not a letter, let's insert.
    cursor.insertText(txt);
    // and let's italicize, except in a few conditions:
    if (isDigit(txt)) {
      // don't italicize numbers
    } else if (prevChar==rquote) {
      // don't italicize after "’" (for "somebody's" etc.)
    } else if (txt=="I" || txt=="a" || txt=="A")  {
      // don't italicize the one-letter words "I" and "a"
    } else if (prevChar==QChar('.')
	       && ((antePrevChar==QChar('i') && txt==QChar('e'))
		   || (antePrevChar==QChar('e') && txt==QChar('g'))
		   || (antePrevChar==QChar('s') && txt==QChar('t'))
		   || (antePrevChar==QChar('d') && txt==QChar('d')))) {
      // treat "e.g.", "i.e.", etc. specially
      qDebug() << "." << antePrevChar << txt;
      MarkupData *mdi = data()->markupAt(cursor.position()-2,
					 MarkupData::Italic);
      if (mdi)
	deleteMarkup(mdi);
    } else {
      addMarkup(MarkupData::Italic,
                cursor.position()-txt.length(), cursor.position());
    }
  }
}  

bool TextItem::keyPressAsMath(QKeyEvent *e) {
  //  int key = e->key();
  //  Qt::KeyboardModifiers mod = e->modifiers();
  static QString punct = ",;.:";
  static QString spacing = " \t\n\r";
  static QString closing = ")]}\"”";
  static QString suremath = "+_^*/=";

  if (cursor.hasSelection())
    return false;
  
  QString txt = e->text();

  if (txt.isEmpty())
    return false;

  if (isLatinLetter(txt)) {
    letterAsMath(txt);
    return true;
  }
  
  if (isDigit(txt)) {
    QChar charBefore = document()->characterAt(cursor.position()-1);
    if (charBefore=='-') {
      QChar charBefore2 = document()->characterAt(cursor.position()-2);
      if (charBefore2.isNull() || charBefore2.isSpace() || charBefore2.isPunct()) {
        cursor.deletePreviousChar();
        cursor.insertText(QString::fromUtf8("−"));
      }
    }
    letterAsMath(txt);
    return true;
  }
  
  bool ismath = suremath.contains(txt);
  if (txt == "-") {
    QChar charBefore = document()->characterAt(cursor.position()-1);
    if (charBefore.isNull() || charBefore.isSpace() || charBefore.isPunct())
      ismath = true;
  }
  
  if (ismath) {
    QChar charBefore = document()->characterAt(cursor.position()-1);
    QChar charBefore2 = document()->characterAt(cursor.position()-2);
    if ((charBefore=='a' || charBefore=='A' || charBefore=='I')
	&& !isLatinLetter(charBefore2)) {
      // italicize after all
      if (!data()->markupAt(cursor.position()-1, MarkupData::Italic))
        addMarkup(MarkupData::Italic,
                  cursor.position()-1, cursor.position());
    } else if ((charBefore2=='a' || charBefore2=='A' || charBefore2=='I')
	       && !isLatinLetter(charBefore)
	       && !isLatinLetter(document()->characterAt(cursor.position()-3))) {
      // italicize after all
      if (!data()->markupAt(cursor.position()-2, MarkupData::Italic))
        addMarkup(MarkupData::Italic,
                  cursor.position()-2, cursor.position()-1);
    }
    
    if (txt == "-") {
      cursor.insertText(QString::fromUtf8("−"));
      return true;
    }
  }

  // we may apply finished TeX Code
  if (cursor.hasSelection()) 
    return false; // we're overwriting, let some other piece of code deal.

  tryTeXCode(true, true);
  if (txt=="^") {
    // de-italicize "e^"
    if (document()->characterAt(cursor.position()-1)=='e') {
      MarkupData *md
        = data()->markupAt(cursor.position(), MarkupData::Italic);
      if (md)
        deleteMarkup(md);
    }
    tryScriptStyles(true);
  } else if (txt=="_") {
    tryScriptStyles(true);
  } else if ((spacing.contains(txt) || closing.contains(txt))
             && punct.contains(document()->characterAt(cursor.position()-1))) {
    cursor.movePosition(TextCursor::Left);
    tryScriptStyles(true);
    cursor.movePosition(TextCursor::Right);
  } else if (spacing.contains(txt)) {
    tryScriptStyles(true);
  } else if (closing.contains(txt)) {
    // try script styles if no corresponding opening
    tryScriptStyles(true);
  }

  return false; // still insert the character
}

 
