// Style.C

#include "Style.H"

Style *Style::dflt = 0;

Style const *Style::style() {
  if (dflt==0)
    dflt = new Style();
  return dflt;
}


Style::Style() {
  // define fonts for real here
  tfont_.setBold(true);
}

double Style::textWidth() const {
  return 6*72;
}

double Style::textHeight() const {
  return 9*72;
}

double Style::titleSep() const {
  return 5;
}

double Style::topMargin() const {
  return 1*72;
}

double Style::bottomMargin() const {
  return .5*72;
}

double Style::leftMargin() const {
  return .75*72;
}

double Style::rightMargin() const {
  return .75*72;
}

double Style::pageWidth() const {
  return textWidth() + leftMargin() + rightMargin();
}

double Style::pageHeight() const {
  return textHeight() + topMargin() + bottomMargin();
}
