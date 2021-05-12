// ResourceMagic/AN_Biblio.H - This file is part of NotedELN

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

// AN_Biblio.H

#ifndef AN_BIBLIO_H

#define AN_BIBLIO_H

#include <QUrl>
#include <QVariant>

class AN_Biblio {
  /*:C AN_Biblio
   *:D AN_ bibliographic references
   *:D A properly formatted tag is YY-ABCD or YYYY-ABCD.
       A single letter preceding the YY is automatically skipped.
   */
public:
  AN_Biblio(QString tag, class Style const &st);
  bool ok() const; // true if the tag can be interpreted as a reference
  QUrl url() const; // url of a pdf file of the referred article
  QString ref() const; // html-formatted reference
private:
  static QVariantMap const &biblio(Style const &st);
  QString tag_;
  QUrl url_;
  QString ref_;
};

#endif
