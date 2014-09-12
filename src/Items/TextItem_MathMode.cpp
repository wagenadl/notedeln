// MathMode.cpp

#include "TextItem.H"
#include "TextMarkings.H"
#include "TeXCodes.H"
#include <QKeyEvent>
#include <QTextDocument>
#include <QDebug>

static bool isLatinLetter(QChar x) {
  return (x>='A' && x<='Z') || (x>='a' && x<='z');
}

bool TextItem::keyPressAsMath(QKeyEvent *e) {
  //  int key = e->key();
  //  Qt::KeyboardModifiers mod = e->modifiers();
  QString txt = e->text();
  QTextCursor c = textCursor();
  if ((txt>="A" && txt<="Z") || (txt>="a" && txt<="z")) {
    if (c.hasSelection()) {
      // we are going to overwrite this selection, I guess
      c.deleteChar();
    }
    // we may italicize or deitalice
    QChar prevChar = document()->characterAt(c.position()-1);
    QChar antePrevChar = document()->characterAt(c.position()-2);
    int dpos = 1;
    if (prevChar == 0x200a) {
      // thin space
      prevChar = antePrevChar;
      antePrevChar = document()->characterAt(c.position()-3);
      dpos = 2;
    }
    if (isLatinLetter(prevChar)) {
      // previous was also a letter; potential deitalicize or bold face
      MarkupData *mdi = markupAt(c.position(), MarkupData::Italic);
      MarkupData *mdb = markupAt(c.position(), MarkupData::Bold);
      if (prevChar==txt[0] && !isLatinLetter(antePrevChar)) {
	// we had the same letter before -> cycle faces
	// order is italic -> bold italic -> bold -> plain -> italic
	if (mdb) {
	  if (mdi) 
	    markings_->deleteMark(mdi);
	  else
	    markings_->deleteMark(mdb);
	} else {
	  if (mdi) 
	    addMarkup(MarkupData::Bold, c.position()-dpos, c.position());
	  else
	    addMarkup(MarkupData::Italic, c.position()-dpos, c.position());
	}
      } else {
	// previous was _different_ letter; we'll deitalicize / debold; redup
	if (mdi)
	  markings_->deleteMark(mdi);
	if (mdb) 
	  markings_->deleteMark(mdb);
        if (dpos>1)
          c.deletePreviousChar();
        if (mdb)
	  c.insertText(QString(prevChar));
	c.insertText(txt);
	if (prevChar=='d') { // magic for "dx"
	  if (!(antePrevChar>='A' && antePrevChar<='Z')
	      && !(antePrevChar>='a' && antePrevChar<='z')) {
            c.insertText(QString::fromUtf8(" "));
	    addMarkup(MarkupData::Italic,
		      c.position()-txt.length()-1, c.position());
          }
	}
      }
      return true; // got it
    } else {
      // previous was not a letter, let's italicize
      c.insertText(txt + QString::fromUtf8(" "));
      addMarkup(MarkupData::Italic,
		c.position()-txt.length() - 1, c.position());
      return true; // got it
    }
  } else if (txt=="-") {
    if (c.hasSelection()) 
      c.deleteChar();
    c.insertText(QString::fromUtf8("−"));
    return true;
  } else if (txt!="") {
    // we may apply finished TeX Code
    if (c.hasSelection()) 
      return false; // we're overwriting, let some other piece of code deal.
    tryTeXCode(true);
    if (txt=="^") {
      // de-italicize "e^"
      if (document()->characterAt(c.position()-1)=='e') {
	MarkupData *md = markupAt(c.position(), MarkupData::Italic);
	if (md)
	  markings_->deleteMark(md);
      }
      tryScriptStyles(c);
    } else if (txt=="_") {
      tryScriptStyles(c);
    } else if (txt==" ") {
      if (QString(",;.:").contains(document()->characterAt(c.position()-1)))
	c.movePosition(QTextCursor::Left);
      tryScriptStyles(c, true);
    }
    return false; // still insert the character
  } else {
    return false;
  }
}

 
