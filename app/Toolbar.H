// Toolbar.H

#ifndef TOOLBAR_H

#define TOOLBAR_H

#include <QToolBar>
#include <QMap>
#include <QAction>

class Toolbar: public QToolBar {
  Q_OBJECT;
public:
  enum Action {
    Act_Type,
    Act_Canvas,
    Act_Line,
    Act_Arrow,
    Act_Emph,
    Act_Strikeout,
    Act_Plain,
    Act_InsertImage,
    Act_Screenshot,
  };
public:
  Toolbar(QWidget *parent);
  virtual ~Toolbar();
  QAction *action(Action);
private:
  void add(Action, QString, QString);
  QMap<Action, QAction *> actions;
};

#endif
