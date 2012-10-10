// TitleData.H

#ifndef TITLEDATA_H

#define TITLEDATA_H

#include "Data.H"
#include <QList>
#include "TextData.H"

class TitleData: public Data {
  Q_OBJECT;
public:
  TitleData(Data *parent=0);
  virtual ~TitleData();
  QList<TextData *> const &versions() const;
  TextData const *current() const;
  TextData const *orig() const;
  TextData *revise();
  // revise() creates a new version if previous is more than X hours old
protected:
  void loadMore(QVariantMap const &src);
  void saveMore(QVariantMap &dst) const;
private:
  QList<TextData *> versions_;
  
};

#endif
