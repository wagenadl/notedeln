 // Magician.H

#ifndef MAGICIAN_H

#define MAGICIAN_H

class Magician {
public:
  Magician();
  virtual ~Magician();
  virtual bool matches(QString) const; // base version returns true
  virtual QUrl webUrl(QString) const; // base version returns null
  virtual QUrl objectUrl(QString) const; // base version returns null
  virtual QString title(QString) const; // returns "" if unable to build
  virtual QString desc(QString) const; // returns "" if unable to build
};

class SimpleMagician: public Magician {
public:
  SimpleMagician();
  virtual ~SimpleMagician();
  virtual QUrl webUrl(QString) const;
  virtual QUrl objectUrl(QString) const;
  void setWebUrlBuilder(QString); // arg must contain "%1" to receive refText
  void setObjectUrlBuilder(QString); // arg must contain "%1" to receive refText
private:
  QString webUrlBuilder;
  QString objectUrlBuilder;
};

#endif
