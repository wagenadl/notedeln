// Style.H

#ifndef STYLE_H

#define STYLE_H

class Style: public QObject {
  Q_OBJECT;
public:
  Style();
  Style(QString filename);
  virtual ~Style();
public:
  static Style const t*style();
public:
  QFont font() const;
  QFont titleFont() const;
  double textWidth() const; // in pt
  double textHeight() const; // in pt
  double titleSep() const;
  double topMargin() const;
  double leftMargin() const;
  double rightMargin() const;
  double bottomMargin() const;
  double pageWidth() const;
  double pageHeight() const;
private:
  static Style *dflt;
  QFont font_;
  QFont tfont_;
};

#endif
