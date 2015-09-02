// MathMode.cpp

#include "TextItem.h"
#include "TeXCodes.h"
#include <QKeyEvent>
#include <QDebug>

static bool isLatinLetter(QChar x) {
  return (x>='A' && x<='Z') || (x>='a' && x<='z');
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
	if (!(antePrevChar>='A' && antePrevChar<='Z')
	    && !(antePrevChar>='a' && antePrevChar<='z')) {
	  addMarkup(MarkupData::Italic,
		    cursor.position()-txt.length(), cursor.position());
	}
      }
    }
  } else {
    // previous was not a letter, let's italicize
    cursor.insertText(txt);
    addMarkup(MarkupData::Italic,
	      cursor.position()-txt.length(), cursor.position());
  }
}  

bool TextItem::keyPressAsMath(QKeyEvent *e) {
  //  int key = e->key();
  //  Qt::KeyboardModifiers mod = e->modifiers();
  QString txt = e->text();
  if ((txt>="A" && txt<="Z") || (txt>="a" && txt<="z")) {
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

 
