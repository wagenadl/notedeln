// GotoPageDialog.cpp

#include "GotoPageDialog.h"
#include <QInputDialog>

GotoPageDialog::GotoPageDialog() {
}

int GotoPageDialog::exec(QWidget *parent, int max) {
  bool ok;
  int n = QInputDialog::getInt(parent, "Go to page",
			       "Go to page number:",
			       1,
			       1, max, 1,
			       &ok);
  return ok ? n : 0;
}
