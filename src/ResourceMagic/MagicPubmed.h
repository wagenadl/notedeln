 // ResourceMagic/MagicPubmed.H - This file is part of eln

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

// MagicPubmed.H

#ifndef MAGICPUBMED_H

#define MAGICPUBMED_H

#include <QUrl>
#include <QVariant>

class MagicPubmed {
  /*:C MagicPubmed
   *:D Magic bibliographic references
   *:D A properly formatted tag is at least 5 numbers: a PMID.
   */
public:
  MagicPubmed(QString tag, class Style const &st);
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
