// Items/HtmlBuilder.h - This file is part of NotedELN

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

// HtmlBuilder.h

#ifndef HTMLBUILDER_H

#define HTMLBUILDER_H

#include <QString>
#include "MarkupData.h"
#include <QVector>
#include <QList>
#include "MarkupStyles.h"

class HtmlBuilder {
public:
  HtmlBuilder(class TextData const *src, int start=0, int end=-1);
  QString toHtml() const { return html; }
private:
  static QString openingTag(MarkupData::Style);
  static QString closingTag(MarkupData::Style);
  static QString tagName(MarkupData::Style);
private:
  QVector<int> nowedges;
  QVector<MarkupStyles> nowstyles;
  QList<MarkupData::Style> stack;
  int start;
  int end;
  QString text;
  QString html;
};

#endif
