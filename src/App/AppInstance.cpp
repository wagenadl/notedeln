// AppInstance.cpp

#include "AppInstance.h"

#include "Notebook.h"
#include "SceneBank.h"
#include "PageEditor.h"
#include "AlreadyOpen.h"
#include "DefaultSize.h"
#include "App.h"
#include <QDebug>

AppInstance::AppInstance(App *app, Notebook *nb): book(nb) {
  connect(app, SIGNAL(aboutToQuit()), book, SLOT(commitNow()));

  bank = new SceneBank(nb);

  PageEditor *editor = new PageEditor(bank);
  editor->resize(DefaultSize::onScreenSize(editor->sizeHint()));
  editor->show();
  registerEditor(editor);

  aopen = new AlreadyOpen(nb->dirPath(), editor);
}

AppInstance::~AppInstance() {
  delete aopen;
  for (auto pe: editors)
    delete pe;
  delete bank;
  delete book;
}

void AppInstance::registerEditor(QObject *e) {
  PageEditor *ed = dynamic_cast<PageEditor *>(e);
  if (ed) {
    editors << ed;
    connect(ed, SIGNAL(destroyed(QObject*)), SLOT(forgetEditor(QObject*)));
  }
}

void AppInstance::forgetEditor(QObject *e) {
  PageEditor *ed = static_cast<PageEditor *>(e);
  /* Cannot "dynamic_cast", because object already deleted. */
  if (ed) {
    editors.remove(ed);
  }
}
