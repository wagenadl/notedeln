// GotoPageDialog.h

#ifndef GOTOPAGEDIALOG_H

#define GOTOPAGEDIALOG_H

class GotoPageDialog {
private:
  GotoPageDialog();
public:
  static int exec(class QWidget *parent=0, int max=9999999);
};

#endif
