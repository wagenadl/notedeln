// Footstacker.cpp

#include "Footstacker.H"
#include <QMultiMap>

Footstacker::Footstacker(BlockItem *bi): bi(bi) {
  bd = bi->data();
  QMultiMap<double, FootnoteItem *> foots;
  foreach (FootnoteItem *fni, bi->footnotes()) {
    FootnoteData *fnd = fni->data();
    QString tag = fnd->tag();
    QPointF p = bi->findRefText(tag);
    foots.insert(p.y() + 0.001*p.x(), fni);
  }

  /* Now we know where each of the footnotes wants to be attached, and
     we know their heights. But it is not in a convenient shape. Let's
     reorganize.
   */
  double cumh = 0;
  cumulHBefore << cumh;
  for (QMultiMap<double, FootnoteItem *>::iterator i=foots.begin();
       i!=foots.end(); ++i) {
    attach << i.key();
    notes << i.value();
    double h = i.value()->data()->height();
    height << h;
    cumh += h;
    cumulHBefore << cumh;
  }
}

int Footstacker::nBefore(double y) {
  int n = 0;
  foreach (double a, attach) {
    if (a<y)
      n++;
    else
      break;
  }
  return n;
}
