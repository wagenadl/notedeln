// PubMedMagician.C

#include "PubMedMagician.H"

bool PubMedMagician::matches(QString s) const {
  int n = s.toInt();
  return n>1e5;
}
QUrl PubMedMagician::webUrl(QString s) const {
  return QUrl(QString("http://www.ncbi.nlm.nih.gov/pubmed/%1").arg(s));
}

QUrl PubMedMagician::objectUrl(QString s) const {
  return QUrl(QString("http://www.ncbi.nlm.nih.gov/pmc/articles/pmid/%1/pdf")
	      .arg(s));
}
