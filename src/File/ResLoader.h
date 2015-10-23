// File/ResLoader.H - This file is part of eln

/* eln is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eln is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eln.  If not, see <http://www.gnu.org/licenses/>.
*/

// ResLoader.H

#ifndef RESLOADER_H

#define RESLOADER_H

#include <QObject>
#include <QUrl>
#include <QFile>

class ResLoader: public QObject {
  Q_OBJECT;
public:
  ResLoader(class Resource *parent, bool convertHtmlToPdf=true);
  virtual ~ResLoader();
  void start();
  bool getNow(double timeout_s=-1); // true if finished without error
  bool getNowDialog(double delay_s = 0.2);
  bool complete() const; // true if finished without error
  bool failed() const; // true if finished with error
  // should we offer networkError() from qnr?
  // In case of error, the DST is deleted.
  QString mimeType() const; // may return "" if not (yet) known
  static QString mime2ext(QString);
signals:
  void finished(); // ok or not
  void progress(int); // percent, or -1 if unknown
private slots:
  void qnrFinished();
  void qnrDataAv();
  void qnrProgress(qint64, qint64);
  void processFinished();
  void processError();
private:
  void startDownload();
  bool makePdfAndPreview();
  bool makePreview(QString mimetype); // return true if we're attempting it
  void startProcess(QString prog, QStringList args);
  bool parentAlive() const;
  void getTitleFromHtml();
private:
  static class QNetworkAccessManager &networkAccessManager();
  class Resource *parentRes;
  class QNetworkReply *qnr;
  class QProcess *proc;
  bool ok;
  bool err;
  QUrl src;
  QFile *dst;
  int redirectCount;
  bool convertHtmlToPdf;
};

#endif
  
