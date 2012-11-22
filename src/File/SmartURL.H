// SmartURL.H

#ifndef SMARTURL_H

#define SMARTURL_H

/* A smart url is a contiguous string of non-whitespace characters, not ending
   in any punctuation other than slash. A regular url is a smart url, as are
   the following:
   www.XXXX -> http://www.XXXX (e.g., www.danielwagenaar.net)
   /XXXX -> file:///XXXX (e.g., /home/wagenaar/foo.pdf)
   NN-XXXX -> recognized as DW bibliography reference (e.g., 12-NW)
   XXXX.YYY -> recognized as a file either in ~, ~/Desktop, ~/Downloads, or
               ~/tmp, in that order. This may be restricted to recognized
	       extensions YYY; not yet sure.
   pm#NNNN -> recognized as PubMed bibliography reference
   Thor#XX -> recognized as a Thorlabs part number
   Naturally, this is extensible through a map in "style.json". For instance:
   "Thor#([-A-Z0-9]+)":
     "https://www.thorlabs.us/thorProduct.cfm?partNumber=(\\1)"
   Perhaps I should somehow allow internal spaces anyway, but that might
   make it too computationally intensive.
   I will let the be MarkupData::URL, and repurpose MarkupData::CustomRef
   to become MarkupData::FootnoteRef. For smart URLs, hitting ^N could create
   a footnote with the page title of the resolved URL as its contents.
   Some mechanism should be created to make that happen in the background.
*/

class SmartURL {
public:
  SmartURL(Style const &style);
  QUrl makeUrl(QString);
  QTextCursor smartAt(QTextCursor); // creates a selection if the textcursor is
  // inside a smartUrl
  QTextCursor smartBefore(QTextCursor); // creates a selection if the cursor
  // is right after a smartUrl (possibly after punctuation or whitespace)
  QUrl makeUrl(MarkupData const *md);
};

#endif
