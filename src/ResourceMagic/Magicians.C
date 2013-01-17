// Magicians.C

#include "Magicians.H"
#include "DWBibMagician.H"

QMap<Style const *, Magicians *> &Magicians::stylemap() {
  static QMap<Style const *, Magicians *> map;
  return map;
}

Magicians const &Magicians::magicians(Style const &st) {
  if (stylemap().contains(&st))
    return *stylemap()[&st];
  Magicians *m1 = new Magicians(st);
  stylemap()[&st] = m1;
  return *m1;
}


Magician const *Magicians::first(QString refText) const {
  foreach (Magician const *m, mm)
    if (m->matches(refText))
      return m;
  return 0;
}

Magician const *Magicians::next(QString refText, Magician const *m0) const {
  bool gotcha = false;
  foreach (Magician const *m, mm) {
    if (gotcha)
      if (m->matches(refText))
	return m;
    if (m==m0)
      gotcha = true;
  }
  return 0;
}

// ----------------------------------------------------------------------
// Here's where we build magicians
// ----------------------------------------------------------------------
Magicians::Magicians(Style const &st) {
  mm.append(new UrlMagician());

  QVariantMap vm = st["magicians"].toMap();
  foreach (QVariant v, vm) {
    QVariantMap m = v.toMap();
    SimpleMagician *sm = new SimpleMagician;
    sm->setMatcher(QRegExp(m["re"].toString()));
    sm->setWebUrlBuilder(m["web"].toString());
    sm->setObjectUrlBuilder(m["object"].toString());
    mm.append(sm);
  }
  mm.append(new DWBibMagician(st));
}
