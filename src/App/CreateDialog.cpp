// CreateDialog.cpp

#include "CreateDialog.H"

#include <QPushButton>
#include <QRadioButton>
#include <QEventLoop>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QKeyEvent>
#include <QDebug>

CreateDialog::CreateDialog(QWidget *parent): QDialog(parent) {
  setWindowTitle("Create new notebook");
  
  QVBoxLayout *vlay = new QVBoxLayout(this);
  vlay->addWidget(new QLabel("File path for new notebook:"));

  QHBoxLayout *hlay = new QHBoxLayout();
  hlay->addWidget(loc = new QLineEdit());
  loc->setAutoFillBackground(true);
  connect(loc, SIGNAL(textEdited(QString)), SLOT(checkOK()));
  hlay->addWidget(locbrowse = new QPushButton(QString::fromUtf8("…")));
  connect(locbrowse, SIGNAL(clicked()), SLOT(browse()));
  locbrowse->setFixedSize(locbrowse->sizeHint().height(),
                          locbrowse->sizeHint().height());
  vlay->addLayout(hlay);

  vlay->addSpacing(15);

  vlay->addWidget(usearch = new QCheckBox("Create VC archive"));
  connect(usearch, SIGNAL(toggled(bool)), SLOT(useArchChanged(bool)));

  hlay = new QHBoxLayout();
  QWidget *gb = new QWidget();
  QVBoxLayout *vv = new QVBoxLayout(gb);
  vv->addWidget(bzr = new QRadioButton("bzr"));
  vv->addWidget(git = new QRadioButton("git"));
  bzr->setChecked(true);
  hlay->addWidget(gb);

  gb = new QWidget();
  vv = new QVBoxLayout(gb);
  vv->addWidget(local = new QRadioButton("local"));
  local->setChecked(true);
  vv->addWidget(remote = new QRadioButton("remote"));
  connect(remote, SIGNAL(toggled(bool)), SLOT(remoteChanged(bool)));
  hlay->addWidget(gb);
  vlay->addLayout(hlay);

  vlay->addWidget(archhost_l = new QLabel("Remote host:"));
  vlay->addWidget(archhost = new QLineEdit(""));
  archhost->setAutoFillBackground(true);
  connect(archhost, SIGNAL(textEdited(QString)), SLOT(checkOK()));
  vlay->addWidget(archloc_l = new QLabel("Remote path:"));
  vlay->addWidget(archloc = new QLineEdit(""));
  archloc->setAutoFillBackground(true);
  connect(archloc, SIGNAL(textEdited(QString)), SLOT(checkOK()));

  vlay->addWidget(bindarch = new QCheckBox("Bind to remote location"));
  bindarch->setChecked(true);
  
  vlay->addSpacing(15);
  vlay->addStretch(20);

  hlay = new QHBoxLayout();
  hlay->addWidget(cancelb = new QPushButton("Cancel"));
  connect(cancelb, SIGNAL(clicked()), SLOT(cancelClicked()));
  hlay->addStretch(20);
  hlay->addWidget(okb = new QPushButton("Create"));
  connect(okb, SIGNAL(clicked()), SLOT(okClicked()));
  vlay->addLayout(hlay);

  useArchChanged(false);
  bindarch->hide();

  QSize s = sizeHint();
  resize(s.width()*2, s.height());
}

CreateDialog::~CreateDialog() {
}

QString CreateDialog::path() const {
  return loc->text();
}

bool CreateDialog::isAcceptable() const {
  return !path().isEmpty() &&
    (!useRemoteArchive()
     || (!archiveHost().isEmpty() && !archivePath().isEmpty()));
}

bool CreateDialog::useBzr() const {
  return usearch->isChecked() && bzr->isChecked();
}

bool CreateDialog::useGit() const {
  return usearch->isChecked() && git->isChecked();
}

bool CreateDialog::useRemoteArchive() const {
  return useArchive() && remote->isChecked();
}

bool CreateDialog::bindRemoteArchive() const {
  return useRemoteArchive() && bindarch->isChecked();
}

QString CreateDialog::archiveHost() const {
  return useArchive() ? archhost->text() : "";
}

QString CreateDialog::archivePath() const {
  return useArchive() ? archloc->text() : "";
}

void CreateDialog::keyPressEvent(QKeyEvent *e) {
  switch (e->key()) {
  case Qt::Key_Escape:
    cancelClicked();
    break;
  case Qt::Key_Return:
    if (e->modifiers() & Qt::ControlModifier)
      okClicked();
    else if (!useRemoteArchive()) // i.e., only one text editor
      okClicked();
    break;
  default:
    break;
  }
}

void CreateDialog::useArchChanged(bool x) {
  if (x) {
    bzr->show();
    git->show();
    local->show();
    remote->show();
    remoteChanged(remote->isChecked());
  } else {
    bzr->hide();
    git->hide();
    local->hide();
    remote->hide();
    remoteChanged(false);
  }
  checkOK();
}

void CreateDialog::remoteChanged(bool x) {
  if (x) {
    archhost_l->show();
    archloc_l->show();
    archhost->show();
    archloc->show();
  } else {
    archhost_l->hide();
    archloc_l->hide();
    archhost->hide();
    archloc->hide();
  }
  checkOK();
}

void CreateDialog::cancelClicked() {
  qDebug() << "cancel clicked";
  loc->setText("");
  reject();
}

void CreateDialog::okClicked() {
  qDebug() << "ok clicked";
  if (isAcceptable())
    accept();
}

void CreateDialog::checkOK() {
  okb->setEnabled(isAcceptable());
}

void CreateDialog::browse() {
  QString fn = QFileDialog::getSaveFileName(this,
                                            "Create new notebook...",
                                            "",
                                            "Notebooks (*.nb)");
  if (fn.isEmpty())
    return; 
  if (!fn.endsWith(".nb"))
    fn += ".nb";
  if (QDir::current().exists(fn)) {
    QMessageBox::warning(this, "eln",
                         "Will not create a new notebook '" + fn
                         + "': file exists.",
                         QMessageBox::Cancel);
    return; 
  }

  loc->setText(fn);
  checkOK();
}
   
