// CreateDialog.h

#ifndef CREATEDIALOG_H

#define CREATEDIALOG_H

#include <QDialog>

class CreateDialog: public QDialog {
  Q_OBJECT;
public:
  CreateDialog(QWidget *parent=0);
  ~CreateDialog();
  bool isAcceptable() const;
  QString path() const;
  bool useArchive() const { return useBzr()||useGit(); }
  bool useBzr() const;
  bool useGit() const;
  bool useRemoteArchive() const;
  bool bindRemoteArchive() const;
  QString archiveHost() const;
  QString archivePath() const;
protected:
  void keyPressEvent(QKeyEvent *);
private slots:
  void useArchChanged(bool);
  void remoteChanged(bool);
  void cancelClicked();
  void okClicked();
  void checkOK();
  void browse();
private:
  class QLineEdit *loc;
  class QPushButton *locbrowse;
  class QLineEdit *archhost, *archloc;
  class QCheckBox *usearch;
  class QLabel *vc_l;
  class QRadioButton *git, *bzr, *local, *remote;
  class QLabel *archloc_l, *archhost_l;
  class QCheckBox *bindarch;
  class QPushButton *cancelb, *okb;
};

#endif
