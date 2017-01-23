// Items/TextItem_MathMode.cpp - This file is part of eln

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

// MathMode.cpp

#include "TextItem.h"
#include "TeXCodes.h"
#include <QKeyEvent>
#include <QDebug>

static bool isLatinLetter(QChar x) {
  return (x>='A' && x<='Z') || (x>='a' && x<='z');
}

static bool isLatinLetter(QString x) {
  return x.size()==1 && isLatinLetter(x[0]);
}

void TextItem::letterAsMath(QString txt) {
  if (cursor.hasSelection()) {
    // we are going to overwrite this selection, I guess
    cursor.deleteChar();
  }
  // we may italicize or deitalice
  QChar prevChar = document()->characterAt(cursor.position()-1);
  QChar antePrevChar = document()->characterAt(cursor.position()-2);
  if (isLatinLetter(prevChar)) {
    // previous was also a letter; potential deitalicize or bold face
    MarkupData *mdi = data()->markupAt(cursor.position(), MarkupData::Italic);
    MarkupData *mdb = data()->markupAt(cursor.position(), MarkupData::Bold);
    if (prevChar==txt[0] && !isLatinLetter(antePrevChar)) {
      // we had the same letter before -> cycle faces
      // order is italic -> bold italic -> bold -> plain -> italic
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
      if (mdb)
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
    // previous was not a letter, let's italicize (except for "I" and "a")
    cursor.insertText(txt);
    if (txt != "I" && txt != "a")
      addMarkup(MarkupData::Italic,
		cursor.position()-txt.length(), cursor.position());
  }
}  

bool TextItem::keyPressAsMath(QKeyEvent *e) {
  //  int key = e->key();
  //  Qt::KeyboardModifiers mod = e->modifiers();
  QString txt = e->text();
  if (isLatinLetter(txt)) {
    letterAsMath(txt);
    return true;
  } else if (txt=="-") {
    if (cursor.hasSelection()) 
      cursor.deleteChar();
    cursor.insertText(QString::fromUtf8("−"));
    return true;
  } else if (txt!="") {
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
      tryScriptStyles();
    } else if (txt=="_") {
      tryScriptStyles();
    } else if (txt==" ") {
      bool afterPunct = QString(",;.:")
	.contains(document()->characterAt(cursor.position()-1));
      if (afterPunct)
	cursor.movePosition(TextCursor::Left);
      tryScriptStyles(true);
      if (afterPunct)
	cursor.movePosition(TextCursor::Right);
    }
    return false; // still insert the character
  } else {
    return false;
  }
}

 
