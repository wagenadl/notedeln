// MathMode.cpp

#include "TextItem.H"
#include "TextMarkings.H"
#include "TeXCodes.H"
#include <QKeyEvent>
#include <QTextDocument>
#include <QDebug>

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
    if ((prevChar>='A' && prevChar<='Z') || (prevChar>='a' && prevChar<='z')) {
      // previous was also a letter; potential deitalicize
      MarkupData *md = markupAt(c.position(), MarkupData::Italic);
      if (md)
        markings_->deleteMark(md);
      c.insertText(txt);
      if (prevChar=='d') // magic for "dx"
        addMarkup(MarkupData::Italic,
                  c.position()-txt.length(), c.position());
      return true; // got it
    } else {
      // previous was not a letter, let's italicize, except for "e"
      c.insertText(txt);
      if (true) // (txt != "e")
        addMarkup(MarkupData::Italic,
                  c.position()-txt.length(), c.position());
      return true; // got it
    }
  } else if (txt=="-") {
    if (c.hasSelection()) 
      c.deleteChar();
    c.insertText(QString::fromUtf8("−"));
    return true;
  } else if (txt=="^") {
    if (c.hasSelection()) 
      return false; // we're overwriting, let some other piece of code deal.
    // de-italicize "e^"
    if (document()->characterAt(c.position()-1)=='e') {
      MarkupData *md = markupAt(c.position(), MarkupData::Italic);
      if (md)
        markings_->deleteMark(md);
    }
    tryScriptStyles(textCursor());
    return false; // let the hat be inserted
  } else if (txt!="") {
    // we may apply finished TeX Code
    if (c.hasSelection()) 
      return false; // we're overwriting, let some other piece of code deal.
    QTextCursor m = document()->find(QRegExp("([^A-Za-z])"),
				     c, QTextDocument::FindBackward);
    int start = m.hasSelection() ? m.selectionEnd() : 0;
    if (start<c.position()) {
      c.setPosition(start, QTextCursor::KeepAnchor);
      QString key = c.selectedText();
      if (key.size()<2)
        return false;
      if (!TeXCodes::contains(key))
        return false;
      c.deleteChar();
      QString val = TeXCodes::map(key);
      if (val.startsWith("x")) {
        if (document()->characterAt(c.position()-1).isSpace())
          c.deletePreviousChar(); // delete previous space
        c.insertText(val.mid(1));
      } else {
        c.insertText(val);
      }
    }
    qDebug() << "ti_mm: " << txt;
    if (QString(" _").contains(txt))
      tryScriptStyles(textCursor());
    return false; // still insert the character
  } else {
    return false;
  }
}

 
