// Items/HtmlParser.h - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// HtmlParser.h

#ifndef HTMLPARSER_H

#define HTMLPARSER_H

#include "MarkupData.h"

class HtmlParser {
public:
  HtmlParser(QString html);
  ~HtmlParser();

  /* HtmlParser attempts to parse a bit of html, extracting tags such as
     <b> and <sup> to build MarkupData. The result is intended to be
     pasted into a TextData.
     Parsing is somewhat simplistic, and is not robust against such nastiness
     as   <div style="badchoice> etcetera">   (which ought to be encoded as
     <div style="badchoice&gt; etcetera">   but isn't necessarily).
     It is therefore recommended that the resulting text is checked against
     a plain text version of the html, if available.
     Additionally, <p>, <br>, and </tr> tags are replaced with newlines.
     The combination </td><td> is replaced with TAB.
  */
  QList<MarkupData *> const &markups() { return marks; }
  /* The MarkupData are all owned by the HtmlParser and will be destroyed
     when the parser is. */
  QString text() const { return txt; }
private:
  QList<MarkupData *> marks;
  QString txt;
};

#endif
