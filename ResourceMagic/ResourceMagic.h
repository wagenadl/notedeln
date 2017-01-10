// ResourceMagic/ResourceMagic.H - This file is part of eln

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

// ResourceMagic.H

#ifndef RESOURCEMAGIC_H

#define RESOURCEMAGIC_H

#include "TextCursor.h"
#include "Data.h"

class ResourceMagic: public QObject {
  Q_OBJECT;
public:
  ResourceMagic(QString refText, Data *parent=0);
  virtual ~ResourceMagic();
  QUrl webUrl() const;
  /* WEBURL - URL where the web page for this magic resource can be found.
   Apparently, can be null. In which case, OBJECTURL should be used.
   (This confuses me - 1/29/16.) */
  QUrl objectUrl() const;
  /* OBJECTURL - URL where the object for this magic resource can be found.
     For most magicians, the object is the same as the web page, but in some
     cases the object can be, e.g., a pdf to be downloaded from that page.
     Null if the URL cannot be simply determined.
  */
  QUrl objectUrlFromWebPage(QString) const;
  /* OBJECTURLFROMWEBPAGE - Object URL constructed from web page text.
     OBJECTURLFROMWEBPAGE(html) returns the object URL as constructed from
     the download HTML content of the web page.
     Null for magicians that do not use this method.
  */
  bool objectUrlNeedsWebPage() const;
  /* OBJECTURLNEEDSWEBPAGE - Is object URL obtained from web page contents?
     OBJECTURLNEEDSWEBPAGE() returns true if OBJECTURLFROMWEBPAGE should be
     used to determine the object URL. Otherwise, OBJECTURL should be used.
   */
  QString title() const;
  /* TITLE - Title of the resource
     TITLE() returns the title of the resource, typically obtained from
     a web page.
  */
  QString desc() const;
  bool isExhausted() const;
  void next();
public:
  static TextCursor explicitLinkAt(TextCursor const &, class Style const &);
  /* EXPLICITLINKAT - Look for magic link
     EXPLICITLINKAT(cursor, style) scans the vicinity of the given text CURSOR
     for anything that could be interpreted as a magic link given the information
     in STYLE.
     The "vicinity" is:
       (1) the selection if the cursor has a selection
       (2) the contiguous non-white space area around the cursor otherwise,
           with punctutation at end removed, plus anything before that fits
	   one of the magic regexps.
   */
private:
  QUrl completePageUrl(QUrl) const;
private:
  QString refText;
  class Magicians const &magicians;
  class Magician const *iter;
  class Notebook *book;
};

#endif
