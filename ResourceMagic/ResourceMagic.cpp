// ResourceMagic/ResourceMagic.cpp - This file is part of eln

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

// ResourceMagic.C

#include "ResourceMagic.h"
#include "Style.h"
#include "Resource.h"
#include "Magician.h"
#include "Magicians.h"
#include "TextItemDoc.h"
#include "Notebook.h"
#include "TOC.h"
#include "TOCEntry.h"

#include <QRegExp>
#include <QDebug>

static TextCursor basicLinkAt(TextCursor const &c) {
  TextItemDoc *doc = c.document();
  TextCursor e = c.findForward(QRegExp("\\s"));
  int end = e.hasSelection() ? e.selectionStart() : -1;
  if (end<0) {
    e = c;
    e.movePosition(TextCursor::End);
    end = e.position();
  }
  QString endchars = QString::fromUtf8(";:.,)]}’”!?—");
  while (end>0 && endchars.contains(doc->characterAt(end-1)))
    end--;
  TextCursor s = c.findBackward(QRegExp("\\s"));
  int start = s.hasSelection() ? s.selectionEnd() : 0;
  QString startchars = QString::fromUtf8("([{‘“¡¿—");
  while (start<end && startchars.contains(doc->characterAt(start)))
    start++;
  if (start>=end)
    return TextCursor();
    
  // Now, start..end is the area that we will work with
  TextCursor m = c;
  m.setPosition(start);
  m.setPosition(end, TextCursor::KeepAnchor);
  return m;
}

TextCursor ResourceMagic::explicitLinkAt(TextCursor const &c,
					 Style const &) {
  if (c.hasSelection()) 
    return c;
  else
    return basicLinkAt(c);
}

ResourceMagic::ResourceMagic(QString refText, Data *parent):
  QObject(parent), refText(refText), 
  magicians(Magicians::magicians(parent->style())),
  book(parent ? parent->book() : 0) {  
  iter = magicians.first(refText);
}

ResourceMagic::~ResourceMagic() {
}

void ResourceMagic::next() {
  iter = magicians.next(refText, iter);
}

bool ResourceMagic::isExhausted() const {
  return iter==0;
}

QUrl ResourceMagic::webUrl() const {
  if (isExhausted())
    return QUrl();
  else
    return completePageUrl(iter->webUrl(refText));
}

QUrl ResourceMagic::objectUrl() const {
  if (isExhausted())
    return QUrl();
  else
    return completePageUrl(iter->objectUrl(refText));
}

QUrl ResourceMagic::objectUrlFromWebPage(QString page) const {
  if (isExhausted())
    return QUrl();
  else
    return iter->objectUrlFromWebPage(refText, page);
}

QUrl ResourceMagic::completePageUrl(QUrl u) const {
  if (u.scheme() != "page")
    return u;
  if (!book)
    return u;
  TOC *toc = book->toc();
  if (!toc)
    return u;
  QString s = u.host();
  int p0 = s.toInt();
  int dp = 0;
  if (s[s.size()-1]>='a') {
    p0 = s.left(s.size()-1).toInt();
    dp = 1 + s.at(s.size()-1).unicode() - 'a';
  } 
  TOCEntry *te = toc->find(p0);
  if (!te)
    return u;
  u.setPath(QString("%1/%2").arg(te->uuid()).arg(p0 + dp - te->startPage()));
  return u;
}

bool ResourceMagic::objectUrlNeedsWebPage() const {
  if (isExhausted())
    return false;
  else
    return iter->objectUrlNeedsWebPage(refText);
}

QString ResourceMagic::title() const {
  if (isExhausted())
    return QString();
  else
    return iter->title(refText);
}

QString ResourceMagic::desc() const {
  if (isExhausted())
    return QString();
  else
    return iter->desc(refText);
}

//bool ResourceMagic::keepAlways() const {
//  if (isExhausted())
//    return false;
//  else
//    return iter->keepAlways(refText);
//}
