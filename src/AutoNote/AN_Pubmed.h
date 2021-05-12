 // ResourceMagic/AN_Pubmed.H - This file is part of NotedELN

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

// AN_Pubmed.H

#ifndef AN_PUBMED_H

#define AN_PUBMED_H

#include <QUrl>
#include <QVariant>

class AN_Pubmed {
  /*:C AN_Pubmed
   *:D AN_ bibliographic references
   *:D A properly formatted tag is at least 5 numbers: a PMID.
   */
public:
  AN_Pubmed(QString tag, class Style const &st);
  bool ok() const; // true if the tag can be interpreted as a reference
  QUrl url() const; // url of a pdf file of the referred article
  QString ref() const; // html-formatted reference
private:
  bool ok_;
  QString tag_;
  QUrl url_;
  QString ref_;
};

#endif
