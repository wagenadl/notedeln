// ResourceMagic.H

#ifndef RESOURCEMAGIC_H

#define RESOURCEMAGIC_H

#include <QTextCursor>
#include "Data.H"

class ResourceMagic: public QObject {
  Q_OBJECT;
public:
  ResourceMagic(QString refText, Data *parent=0);
  virtual ~ResourceMagic();
  QUrl webUrl() const;
  QUrl objectUrl() const;
  QString title() const;
  QString desc() const;
  bool isExhausted() const;
  //  bool keepAlways() const; // true if a resource should be kept even if nothing can be downloaded
  void next();
public:
  static QTextCursor explicitLinkAt(QTextCursor const &, class Style const &);
  /*:F explicitLinkAt
   *:D Scans the vicinity of the given text cursor for anything that could
       be interpreted as a magic link. The "vicinity" is:
       (1) the selection if the cursor has a selection
       (2) the contiguous non-white space area around the cursor otherwise,
           with punctutation at end removed, plus anything before that fits
	   one of the magic regexps.
   */
  static QTextCursor autoLinkAt(QTextCursor const &, class Style const &);
  /*:F autoLinkAt
   *:D Scans the vicinity of the given text cursor for anything that can only
       be interpreted as a link. The "vicinity" is the contiguous non-white
       space area around the cursor, with punctutation at end removed.
   */
private:
  QString refText;
  class Magicians const &magicians;
  class Magician const *iter;
};

#endif
