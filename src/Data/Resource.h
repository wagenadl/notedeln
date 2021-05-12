// Data/Resource.H - This file is part of NotedELN

/* NotedELN is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   NotedELN is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with NotedELN.  If not, see <http://www.gnu.org/licenses/>.
*/

// Resource.H

#ifndef RESOURCE_H

#define RESOURCE_H

#include "Data.h"
#include <QUrl>
#include <QDir>
#include <QFile>

class Resource: public Data {
  Q_OBJECT;
  Q_PROPERTY(QString tag READ tag WRITE setTag);
  /* TAG - The name of the resource as it appears in the text. */
  Q_PROPERTY(QUrl source READ sourceURL WRITE setSourceURL);
  /* SOURCE - The URL from which the resource can be/has been downloaded. */
  Q_PROPERTY(QString preview READ previewFilename WRITE setPreviewFilename);
  /* PREVIEW - The filename of the downloaded preview image of this resource.
     Only the leaf name is stored, not the "PGNO-PGUID-res/" part.
     Normally, constructed from the TAG plus the UUID plus "p.png".
   */
  Q_PROPERTY(QString archive READ archiveFilename WRITE setArchiveFilename);
  /* ARCHIVE - The filename of the downloaded copy of this resource.
     Only the leaf name is stored, not the "PGNO-PGUID-res/" part.
     Normally, constructed from the TAG plus the UUID plus ".ext".
     Some resource types, including html are converted to pdf.
  */
  Q_PROPERTY(QString title READ title WRITE setTitle);
  /* TITLE - Title of this resource.
     Typically constructed from the <title> of a web page. */
  Q_PROPERTY(QString desc READ description WRITE setDescription);
  /* DESCRIPTION - Long description of this resource.
     Not used for any resources at present.
   */
public: // strict Data functions
  Resource(Data *parent=0);
  virtual ~Resource();
  QString tag() const;
  QUrl sourceURL() const;
  QString previewFilename() const;
  QString archiveFilename() const;
  QString title() const;
  QString description() const;
  void setTag(QString);
  void setSourceURL(QUrl);
  void setPreviewFilename(QString);
  void setArchiveFilename(QString);
  void setTitle(QString);
  void setDescription(QString);
public: // functions to do with using the resource
  void setRoot(QDir);
  /* SETROOT - Determine where archives and previews are kept.
     SETROOT(dir) sets the storage location for this resource. Normally of
     the form "notebook/pages/nnnn-uuid.res".
     This must be called before the HASARCHIVE, ARCHIVEPATH, GETARCHIVE, etc.
     functions.
   */
  bool hasArchive() const;
  /* HASARCHIVE - Do we have a stored archive?
     HASARCHIVE returns true if an archive file exists for this resource.
     It returns false if the archive is currently being loaded.
  */
  bool needsArchive() const;
  /* NEEDSARCHIVE - Are we lacking an archive?
     NEEDSARCHIVE returns true iff HASARCHIVE returns false, except for "page:"
     references (internal references to other pages of the notebook), for
     which NEEDSARCHIVE always returns false.
  */
  bool hasPreview() const;
  /* HASPREVIEW - Do we have a stored preview?
     HASPREVIEW returns true if an preview image exists for this resource.
     It returns false if the preview is currently being constructed.
  */
  bool needsPreview() const;
  /* NEEDSPREVIEW - Are we lacking a preview?
     NEEDSPREVIEW returns true if the archive is a pdf file for which a preview
     png would be useful.
  */
  QString archivePath() const;
  /* ARCHIVEPATH - Full path of archive file */
  QString previewPath() const;
  /* PREVIEWPATH - Full path of preview file */
public: // functions to do with actually acquiring a resource
  bool importImage(QImage);
  /* IMPORTIMAGE - Add an image directly as a resource
     IMPORTIMAGE(image) saves the given IMAGE immediately as the ARCHIVE file
     for this resource. Returns true if successful.
     Requires TAG and ROOT to be set first.
  */
  void getArchiveAndPreview();
  /* GETARCHIVEANDPREVIEW - Request that archival copy be downloaded
     GETARCHIVEANDPREVIEW() starts downloading of the resource. Completes in
     background. A preview image is automatically generated upon success.
     Upon completion (successful or not), DOWNLOADFINISHED() is emitted.
     Requires TAG, SOURCEURL and ROOT to be set first. The ARCHIVE and
     PREVIEW filenames are automatically generated if not previously set.
  */
  void ensureDir();
  /* ENSUREDIR - Ensure that the ROOT directory actually exists.
     ENSUREDIR() causes the ROOT dir to be created (with mkpath) if
     it doesn't already exist.
   */
  bool hasFailed() const;
  /* HASFAILED - Report whether a download has been attempted unsuccessfully.
     HASFAILED() returns true if GETARCHIVEANDPREVIEW has been called
     *in this session* but failed to bring home the bacon.
     */
  bool inProgress() const;
  /* INPROGRESS - Report whether downloading is in progress.
     INPROGRESS() returns true if downloading is currently in progress.
     This is a 1:1 indicator that a FINISHED() signal can be expected.
  */
signals:
  void finished();
  /* FINISHED - Emitted when the archive is downloaded */
private slots:
  void downloadFinished();
private:
  void ensureArchiveFilename();
  void validateSource();
private:
  QString tag_;
  QUrl src;
  QString prev;
  QString arch;
  QString ttl;
  QString desc;
private:
  class ResLoader *loader;
  QDir dir;
  bool failed;
};

#endif
