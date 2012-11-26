// ResourceMagic.H

#ifndef RESOURCEMAGIC_H

#define RESOURCEMAGIC_H

#include <QTextCursor>

class ResourceMagic {
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
       be interpreted as a link. The "vicinity" is:
       (2) the contiguous non-white space area around the cursor otherwise,
           with punctutation at end removed.
   */
  static bool magicLink(class Resource *);
  /*:F magicLink
   *:D Given a resource with a tag defined, fills out the URL, title, and
       description fields if the tag can be interpreted as a magic link.
       Returns true if successful.
  */
};

#endif
