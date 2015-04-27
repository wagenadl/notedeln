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
  QUrl objectUrl() const;
  QUrl objectUrlFromWebPage(QString) const;
  bool objectUrlNeedsWebPage() const;
  QString title() const;
  QString desc() const;
  bool isExhausted() const;
  //  bool keepAlways() const; // true if a resource should be kept even if nothing can be downloaded
  void next();
public:
  static TextCursor explicitLinkAt(TextCursor const &, class Style const &);
  /*:F explicitLinkAt
   *:D Scans the vicinity of the given text cursor for anything that could
       be interpreted as a magic link. The "vicinity" is:
       (1) the selection if the cursor has a selection
       (2) the contiguous non-white space area around the cursor otherwise,
           with punctutation at end removed, plus anything before that fits
	   one of the magic regexps.
   */
private:
  QString refText;
  class Magicians const &magicians;
  class Magician const *iter;
};

#endif
