// ResourceMagic/Magician.H - This file is part of eln

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

 // Magician.H

#ifndef MAGICIAN_H

#define MAGICIAN_H

#include <QString>
#include <QUrl>
#include <QRegExp>
#include <QVariant>

class Magician {
public:
  Magician();
  virtual ~Magician();
  virtual bool matches(QString) const; // base version returns true
  virtual QUrl webUrl(QString) const; // base version returns null
  virtual QUrl objectUrl(QString) const; // base version returns null
  virtual QString title(QString) const; // returns "" if unable to build
  virtual QString desc(QString) const; // returns "" if unable to build
  /* It is fine for a magician to provide no Urls, but it may only give
     a webUrl if it also provides an objectUrl.
     Perhaps that's not what I ultimately want, but that's how it is now.
  */
  virtual bool objectUrlNeedsWebPage(QString) const;
  virtual QUrl objectUrlFromWebPage(QString, QString) const;
};

class SimpleMagician: public Magician {
public:
  SimpleMagician(QVariantMap const &dict);
  /* dict must contain keys:
       "re": a regexp
       "web": a url with %1 (optional)
       "object": a url with %1
  */
  SimpleMagician();
  virtual ~SimpleMagician();
  virtual bool matches(QString) const;
  virtual QUrl webUrl(QString) const;
  virtual QUrl objectUrl(QString) const;
  void setMatcher(QRegExp);
  void setWebUrlBuilder(QString); // arg must contain "%1" to receive refText
  void setObjectUrlBuilder(QString); // arg must contain "%1" to receive refText
private:
  QRegExp re;
  QString webUrlBuilder;
  QString objectUrlBuilder;
};

class UrlMagician: public Magician {
public:
  virtual bool matches(QString) const;
  virtual QUrl objectUrl(QString) const;
};

#endif
