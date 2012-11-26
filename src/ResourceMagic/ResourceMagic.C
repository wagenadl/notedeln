// ResourceMagic.C

#include "ResourceMagic.H"
#include "Style.H"
#include "Resource.H"

#include <QTextCursor>
#include <QTextDocument>
#include <QRegExp>
#include <QDebug>

QTextCursor ResourceMagic::autoLinkAt(QTextCursor const &c, Style const &) {
  QTextDocument *doc = c.document();
  QTextCursor s = doc->find(QRegExp("\\s"), c, QTextDocument::FindBackward);
  int start = s.hasSelection() ? s.selectionEnd() : 0;
  QTextCursor e = doc->find(QRegExp("\\s"), c);
  int end = e.hasSelection() ? s.selectionStart() : -1;
  if (end<0) {
    e = c;
    e.movePosition(QTextCursor::End);
    end = e.position();
  }
  e = doc->find(QRegExp("[^][{}()-:;'\".,?!]"), e,
		QTextDocument::FindBackward);
  if (e.hasSelection())
    end = e.selectionEnd();
  else
    return QTextCursor();
    
  // Now, start..end is the area that we will work with
  QTextCursor m = c;
  m.setPosition(start);
  m.setPosition(end, QTextCursor::KeepAnchor);
  QString txt = m.selectedText();
  if (txt.startsWith("file://")
      || txt.startsWith("http://")
      || txt.startsWith("www."))
    // || QRegExp("/[^/]+?/[^/]+").indexIn(txt)==0) // filenames?
    return m;
  else
    return QTextCursor();
}

static QTextCursor matchBackward(QRegExp const &re, QTextCursor const &e) {
  QTextCursor s = e.document()->find(re, e, QTextDocument::FindBackward);
  if (!s.hasSelection())
    return QTextCursor();
  if (s.selectionEnd()<e.position())
    return QTextCursor();
  if (s.selectionEnd()>e.position())
    s.setPosition(e.position(), QTextCursor::KeepAnchor);
  qDebug() << "matchBackward" << s.selectionStart() << s.selectionEnd() << s.selectedText() << e.position();
  return s;
}

QTextCursor ResourceMagic::explicitLinkAt(QTextCursor const &c,
					  Style const &) {
  if (c.hasSelection()) {
    return c;
  } else {
    QTextDocument *doc = c.document();
    QTextCursor e = doc->find(QRegExp("\\s"), c);
    if (!e.hasSelection()) {
      e = c;
      e.movePosition(QTextCursor::End);
    }
    qDebug() << e.position();
    e = doc->find(QRegExp("[-\\w/]"), e,
		  QTextDocument::FindBackward);
    if (!e.hasSelection())
      return QTextCursor();
    e.setPosition(e.selectionEnd());
    qDebug() << e.position();
      
    // OK. Now we have a valid end pointer. Let's see what kind of link
    // we can find.
    // Standard url
    QTextCursor s = matchBackward(QRegExp("\\b(file|http|https)://[^\\s]+"), e);
    if (s.hasSelection())
      return s;

    // starting with "www."
    s = matchBackward(QRegExp("\\bwww\\.[^\\s]+"), e);
    if (s.hasSelection())
      return s;

    // full path name without spaces
    { int start = e.position();
      s = matchBackward(QRegExp("/"), e); // optional final slash
      if (s.hasSelection()) {
	start = s.selectionStart();
	s.setPosition(start);
      } else {
	s = e;
      }
      while (true) {
	s = matchBackward(QRegExp("/[^/\\s]+"), s);
	if (s.hasSelection()) {
	  start = s.selectionStart();
	  s.setPosition(start);
	} else {
	  break;
	}
      }
      if (start<e.position()) {
	s = e;
	s.setPosition(start, QTextCursor::KeepAnchor);
	if (matchBackward(QRegExp("^|\\s"), s).hasSelection())
	  return s;
      }
    }

    // DW bibliography style
    s = matchBackward(QRegExp("\\b[A-Z]?\\d\\d-[A-Za-z0-9]{1,4}"), e);
    if (s.hasSelection())
      return s;

    // pubmed as pm.12367812
    s = matchBackward(QRegExp("\\bpm\\.\\d+"), e);
    if (s.hasSelection())
      return s;

    // Thor Labs part as Thor #BA2
    s = matchBackward(QRegExp("\\bThor ?# ?\\w+"), e);
    if (s.hasSelection())
      return s;
      
    return QTextCursor();
  }
}

bool ResourceMagic::magicLink(Resource *r) {
  if (r->tag().startsWith("www.")) 
    r->setSourceURL("http://" + r->tag());
  else if (r->tag().startsWith("http://")
	   || r->tag().startsWith("https://")
	   || r->tag().startsWith("file://"))
    r->setSourceURL(r->tag());
  else if (r->tag().startsWith("/"))
    r->setSourceURL("file://" + r->tag());
  // add other stuff later
  return !r->sourceURL().isEmpty();
}


/* Code for actually looking up a link: This is going to be done by
   TextMarkings/HoverRegion instead.

   if (md) {
   QString txt = markedText(md);
   QUrl url(txt.startsWith("www.") ? ("http://"+txt) : txt);
   if (!url.isValid()) {
   qDebug() << "Invalid url:" << url;
   return false;
   }
   Q_ASSERT(data()->book());
   Q_ASSERT(data()->resMgr());
   connect(data()->resMgr(), SIGNAL(finished(QString)),
   this, SLOT(linkFinished(QString)),
   Qt::UniqueConnection);
   data()->resMgr()->link(url);
   return true;
   } else {
   return false;
   }
   }

   void TextItem::linkFinished(QString resName) {
   if (!data()->resMgr()->contains(resName))
   return; // failed
   QString url = data()->resMgr()->url(resName).toString();
   markings_->foundUrl(url);
   }
*/

