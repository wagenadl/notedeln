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
  /* START - Starts the loading process
     Upon completion, FINISHED() is emitted, whether or not successful.
  */
  bool isComplete() const;
  /* ISCOMPLETE - Returns true if finished without error */
  bool isFailed() const;
  /* ISFAILED - Returns true if finished with error */
  QString mimeType() const;
  /* MIMETYPE - Mime type of downloaded resource
     May return "" if not (yet) known. */
  static QString mime2ext(QString);
  /* MIME2EXT - Convert a mime type to a file extension.
     Currently only knows about "pdf" and "html". Returns "" if unknown.
   */
signals:
  void finished();
  /* FINISHED - Emitted when process finishes.
     Emitted whether or not the download was successful.
  */
private slots:
  void qnrFinished();
  void qnrDataAv();
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
  
