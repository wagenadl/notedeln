// Magic.H

#ifndef MAGIC_H

#define MAGIC_H

class Magic {
public:
  Magic();
  bool matches(QString);
  virtual QUrl url(QString);
  virtual QUrl arch(QString);
  virtual QString title(QString);
};

#endif
