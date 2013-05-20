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

#include "ResourceMagic.H"
#include "Style.H"
#include "Resource.H"
#include "Magician.H"
#include "Magicians.H"

#include <QTextCursor>
#include <QTextDocument>
#include <QRegExp>
#include <QDebug>

static QTextCursor basicLinkAt(QTextCursor const &c) {
  QTextDocument *doc = c.document();
  QTextCursor e = doc->find(QRegExp("\\s"), c);
  int end = e.hasSelection() ? e.selectionStart() : -1;
  if (end<0) {
    e = c;
    e.movePosition(QTextCursor::End);
    end = e.position();
  }
  while (end>0
	 && QString::fromUtf8(";:.,)]}’”!?—").contains(doc->characterAt(end-1)))
    end--;
  QTextCursor s = doc->find(QRegExp("\\s"), c, QTextDocument::FindBackward);
  int start = s.hasSelection() ? s.selectionEnd() : 0;
  while (start<end
	 && QString::fromUtf8("([{‘“¡¿—").contains(doc->characterAt(start)))
    start++;
  if (start>=end)
    return QTextCursor();
    
  // Now, start..end is the area that we will work with
  QTextCursor m = c;
  m.setPosition(start);
  m.setPosition(end, QTextCursor::KeepAnchor);
  return m;
}

QTextCursor ResourceMagic::autoLinkAt(QTextCursor const &c, Style const &) {
  QTextCursor m = basicLinkAt(c);
  if (!m.hasSelection())
    return m;
  QString txt = m.selectedText();
  if (txt.startsWith("file://")
      || txt.startsWith("http://")
      || txt.startsWith("www."))
    // || QRegExp("/[^/]+?/[^/]+").indexIn(txt)==0) // filenames?
    return m;

  m.clearSelection();
  return m;
}


QTextCursor ResourceMagic::explicitLinkAt(QTextCursor const &c,
					  Style const &) {
  if (c.hasSelection()) 
    return c;
  else
    return basicLinkAt(c);
}

ResourceMagic::ResourceMagic(QString refText, Data *parent):
  QObject(parent), refText(refText),
  magicians(Magicians::magicians(parent->style())) {
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
    return iter->webUrl(refText);
}

QUrl ResourceMagic::objectUrl() const {
  if (isExhausted())
    return QUrl();
  else
    return iter->objectUrl(refText);
}

QUrl ResourceMagic::objectUrlFromWebPage(QString page) const {
  if (isExhausted())
    return QUrl();
  else
    return iter->objectUrlFromWebPage(refText, page);
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
