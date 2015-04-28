// ResourceMagic/MagicBiblio.H - This file is part of eln

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

// MagicBiblio.H

#ifndef MAGICBIBLIO_H

#define MAGICBIBLIO_H

#include <QUrl>
#include <QVariant>

class MagicBiblio {
  /*:C MagicBiblio
   *:D Magic bibliographic references
   *:D A properly formatted tag is YY-ABCD or YYYY-ABCD.
       A single letter preceding the YY is automatically skipped.
   */
public:
  MagicBiblio(QString tag, class Style const &st);
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
