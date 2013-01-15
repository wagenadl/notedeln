// ResourceMagic.C

#include "ResourceMagic.H"
#include "Style.H"
#include "Resource.H"
#include "Magician.H"

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
  qDebug() << "matchBackward" << s.selectionStart() << s.selectionEnd()
	   << s.selectedText() << e.position();
  return s;
}

QTextCursor ResourceMagic::explicitLinkAt(QTextCursor const &c,
					  Style const &) {
  if (c.hasSelection()) 
    return c;

  int end;
  QTextDocument *doc = c.document();
  QTextCursor e = doc->find(QRegExp("\\s"), c);
  int end = e.hasSelection() ? e.selectionStart() : doc->characterCount();
  while (end>0
	 && QString.fromUtf8(";:.,)]}’”!?—").contains(doc->characterAt(end-1)))
    end--;
  QTextCursor s = doc->find(QRegExp("\\s"), c);
  int start = s.hasSelection() ? s.selectionStart() : 0;
  while (start<end
	 && QString.fromUtf8("([{‘“¡¿—").contains(doc->characterAt(start)))
    start++;
  if (start>=end)
    return QTextCursor();
  QTextCursor m(doc->textCursor());
  m.setPosition(start);
  m.movePosition(end, QTextCursor::KeepAnchor);
  return m;
}

ResourceMagic::ResourceMagic(QString refText, QObject *parent):
  QObject(parent), refText(refText) {
  iter = -1;
  next();
}

void ResourceMagic::next() {
  ++iter;
  while (!isExhausted() && magicians()[iter].matches(refText))
    ++iter;
}

bool ResourceMagic::isExhausted() const {
  return iter >= magicians().size();
}

QUrl ResourceMagic::webUrl() const {
  if (isExhausted())
    return QUrl();
  else
    return magicians()[iter]->webUrl(refText);
}

QUrl ResourceMagic::objectUrl() const {
  if (isExhausted())
    return QUrl();
  else
    return magicians[iter]->objectUrl(refText);
}

QString ResourceMagic::title() const {
  if (isExhausted())
    return QString();
  else
    return magicians[iter]->title(refText);
}

QString ResourceMagic::desc() const {
  if (isExhausted())
    return QString();
  else
    return magicians[iter]->desc(refText);
}

/*  

bool ResourceMagic::magicLink(Resource *r) {
  QString tag = r->tag();
  qDebug() << "magicLink" << tag;
  if (tag.startsWith("www.")) 
    r->setSourceURL("http://" + tag);
  else if (tag.startsWith("http://")
	   || tag.startsWith("https://")
	   || tag.startsWith("file://"))
    r->setSourceURL(tag);
  else if (tag.startsWith("/"))
    r->setSourceURL("file://" + tag);
  else if (QRegExp("[A-Z]?\\d\\d(\\d\\d)?-[A-Za-z0-9]+")
	   .exactMatch(tag)) {
    qDebug() << "Magic link: shaped like a DW biblio ref";
    MagicBiblio bib(tag, r->style());
    if (bib.ok()) {
      qDebug() << "  -> got it";
      r->setSourceURL(bib.url());
      r->setTitle(bib.ref());
    }
  }
  // add other stuff later
  return !r->sourceURL().isEmpty();
}

*/

QList<Magician *> const &ResourceMagic::magicians() {
  static QList<Magician *> list;
  if (!list.isEmpty())
    return list;

  // let's create a bunch of magicians
  return list;
}
