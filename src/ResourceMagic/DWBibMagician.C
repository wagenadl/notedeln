// DWBibMagician.C

#include "DWBibMagician.H"
#include "Style.H"
#include "MagicBiblio.H"
#include <QDebug>

DWBibMagician::DWBibMagician(): style(Style::defaultStyle()) {
}

DWBibMagician::DWBibMagician(Style const &style): style(style) {
}

bool DWBibMagician::matches(QString s) const {
  MagicBiblio b(s, style);
  qDebug() << "DWBibMagician"<<s<<b.ok();
  return b.ok();
}

QUrl DWBibMagician::objectUrl(QString s) const {
  MagicBiblio b(s, style);
  return b.ok() ? b.url() : QUrl();
}

QString DWBibMagician::title(QString s) const {
  MagicBiblio b(s, style);
  return b.ref();
}

