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
#include "Digraphs.h"
#include <QKeyEvent>
#include <QDebug>

static bool isLatinLetter(QChar x) {
  return (x>='A' && x<='Z') || (x>='a' && x<='z');
}

static bool isLatinLetter(QString x) {
  return x.size()==1 && isLatinLetter(x[0]);
}

void TextItem::letterAsMath(QString txt) {
  const QString rquote = QString::fromUtf8("’");
  if (cursor.hasSelection())
    return; // don't do it.

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
    // previous was not a letter, let's insert.
    cursor.insertText(txt);
    // and let's italicize, except in a few conditions:
    if (prevChar==rquote) {
      // don't italicize after "’" (for "somebody's" etc.)
    } else if (txt=="I" || txt=="a" || txt=="A")  {
      // don't italicize the one-letter words "I" and "a"
    } else if (prevChar=="."
	       && ((antePrevChar=="i" && txt=="e")
		   || (antePrevChar=="e" && txt=="g")
		   || (antePrevChar=="d" && txt=="d"))) {
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
  static QString closing = ")]}\"'”’";
  static QString suremath = "+-_^*/=";

  if (cursor.hasSelection())
    return false;
  
  QString txt = e->text();

  if (txt.isEmpty())
    return false;
  qDebug() << "keyasmath" << txt;

  if (isLatinLetter(txt)) {
    letterAsMath(txt);
    return true;
  } else if (suremath.contains(txt)) {
    QChar charBefore = document()->characterAt(cursor.position()-1);
    QChar charBefore2 = document()->characterAt(cursor.position()-2);
    if ((charBefore=='a' || charBefore=='A' || charBefore=='I') && !isLatinLetter(charBefore2)) {
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
    
    if (txt=="-") {
      QString digraph = QString(charBefore) + "-";
      QString trigraph = QString(charBefore2) + digraph;
      if (Digraphs::contains(digraph)) {
	cursor.deletePreviousChar();
	cursor.insertText(Digraphs::map(digraph));
      } else if (Digraphs::contains(trigraph)) {
      cursor.deletePreviousChar();
      cursor.deletePreviousChar();
      cursor.insertText(Digraphs::map(trigraph));
      } else {
	cursor.insertText(QString::fromUtf8("−"));
      }
      return true;
    }
  }

  if (txt!="") {
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
  }
  return false; // still insert the character
}

 
