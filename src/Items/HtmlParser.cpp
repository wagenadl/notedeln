// Items/HtmlParser.cpp - This file is part of eln

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

// HtmlParser.cpp

#include "HtmlParser.h"

#include <QTextDocument>
#include <QDebug>

static QString taglessHtmlToPlainText(QString html) {
  /* This converts things like "&gt;" to ">". */
  if (!html.contains("&"))
    return html;
  QTextDocument doc;
  doc.setHtml(html);
  return doc.toPlainText();
}

HtmlParser::HtmlParser(QString html) {
  html.replace("\n", " ");
  html.replace("\t", " ");
  html.replace(QRegExp(" *<(p|br)(\\s+[^>]*)?> *"), "\n");
  html.replace("</td><td>", "\t");
  html.replace("</tr>", "\n");
  QRegExp tag("<([^>]*)>");
  tag.setMinimal(true);
  QList<int> italicStarts;
  QList<int> boldStarts;
  QList<int> superStarts;
  QList<int> subStarts;
  while (!html.isEmpty()) {
    int idx = tag.indexIn(html);
    if (idx>=0) {
      QString cap = tag.cap(1);
      txt += taglessHtmlToPlainText(html.left(idx));
      html = html.mid(idx + tag.matchedLength());
      if (cap=="i") 
	italicStarts.append(txt.size());
      else if (cap=="b")
	boldStarts.append(txt.size());
      else if (cap=="sub")
	subStarts.append(txt.size());
      else if (cap=="sup")
	superStarts.append(txt.size());
      else if (cap=="/i" && !italicStarts.isEmpty()) 
	marks << new MarkupData(italicStarts.takeLast(), txt.size(),
				MarkupData::Italic);
      else if (cap=="/b" && !boldStarts.isEmpty()) 
	marks << new MarkupData(boldStarts.takeLast(), txt.size(),
				MarkupData::Bold);
      else if (cap=="/sub" && !subStarts.isEmpty()) 
	marks << new MarkupData(subStarts.takeLast(), txt.size(),
				MarkupData::Subscript);
      else if (cap=="/sup" && !superStarts.isEmpty()) 
	marks << new MarkupData(superStarts.takeLast(), txt.size(),
				MarkupData::Superscript);
    } else {
      txt += html;
      break;
    }
  }
  while (!italicStarts.isEmpty())
    marks << new MarkupData(italicStarts.takeLast(), txt.size(),
			    MarkupData::Italic);
  while (!boldStarts.isEmpty())
    marks << new MarkupData(boldStarts.takeLast(), txt.size(),
			    MarkupData::Bold);
  while (!subStarts.isEmpty())
    marks << new MarkupData(subStarts.takeLast(), txt.size(),
			    MarkupData::Subscript);
  while (!superStarts.isEmpty())
    marks << new MarkupData(superStarts.takeLast(), txt.size(),
			    MarkupData::Superscript);
}

HtmlParser::~HtmlParser() {
  foreach (MarkupData *md, marks)
    delete md;
}
