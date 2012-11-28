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
