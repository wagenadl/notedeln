// AutoNote.C


#include "AutoNote.H"
#include "TextItem.H"
#include "Style.H"
#include "Resources.H"
#include "TextData.H"
#include "JSONFile.H"

#include <QTextCursor>
#include <QTextDocument>
#include <QDebug>

static QVariantMap biblio;

/* We use style options:
    auto-notes: map of regexps to strings naming automatic note generators
    bib-file: full path to biblio.json file
    bib-dir: full path to papers file where TAG.pdf files may be found.
*/

static bool autoNoteBiblio(QString tag, TextItem *dest, Style const &st) {
  // where do we get the biblio?
  if (!st.contains("bib-file"))
    return false;
  
  if (biblio.isEmpty())
    biblio = JSONFile::load(st.string("bib-file"));

  if (tag.indexOf(QRegExp("[A-Z]"))==0)
    tag = tag.mid(1);
  qDebug() << "Corrected tag" << tag;
  if (!biblio.contains(tag))
    return false;

  QString bib = biblio[tag].toString();
  
  dest->document()->clear();
  QTextCursor c(dest->document());

  QRegExp starttag("<[bi]>");
  QRegExp endtag("</[bi]>");
  while (!bib.isEmpty()) {
    int idx = bib.indexOf(starttag);
    if (idx>=0) {
      c.insertText(bib.left(idx));
      bool isIt = bib[idx+1]=='i';
      bib = bib.mid(idx+3);
      int p = c.position();
      int idx = bib.indexOf(endtag);
      if (idx>=0) {
	if ((isIt && bib[idx+2]!='i')
	    || (!isIt && bib[idx+2]!='b'))
	  qDebug() << "autoNoteBiblio: tag confusion. Possible unsupported nesting.";
	c.insertText(bib.left(idx));
	dest->addMarkup(isIt ? MarkupData::Italic : MarkupData::Bold,
			p, c.position());
	bib = bib.mid(idx+4);
      } else {
	c.insertText(bib);
	bib = "";
	qDebug() << "autoNoteBiblio: missing end tag";
      }
    } else {
      c.insertText(bib);
      bib = "";
    }
  }

  /* get the paper? 
  if (st.contains("bib-dir"))
    dest->data()->resources()->linkCustom(tag,
					  "file://" + st.string("bib-dir")
					  + "/" + tag + ".pdf");
  */
  return true;
}

static bool autoNote(QString funcName, QString tag, TextItem *dest,
		     Style const &st) {
  if (funcName=="bib")
    return autoNoteBiblio(tag, dest, st);
  else
    return false;
}

bool AutoNote::autoNote(QString tag, TextItem *dest, Style const &st) {
  QVariantMap autos = st["auto-notes"].toMap();
  foreach (QString k, autos.keys()) {
    QRegExp re(k);
    if (re.exactMatch(tag))
      return ::autoNote(autos[k].toString(), tag, dest, st);
  }
  return false;
}


  
