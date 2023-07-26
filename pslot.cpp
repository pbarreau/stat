#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtCore>
#include <QtNetwork>
#include <QProcess>
#include <QProgressDialog>

#include <QtGui>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QMdiArea>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <cstdio>

#include "BCompress.h"

class QSslError;

using namespace std;

#include "mainwindow.h"
#include "choixjeux.h"

QString FdjDbZip = "FdjDbZip";
QString FdjDbUse = "FdjDbUse";

void MainWindow::pslot_newGame()
{
 ChoixJeux *selection = new ChoixJeux(this);
 selection->setModal(true);
 selection->show();
}

void MainWindow::pslot_open()
{
}

bool MainWindow::pslot_save()
{
 bool status = true;

 return status;
}

bool MainWindow::pslot_saveAs()
{
 bool status = true;

 return status;

}

void MainWindow::pslot_close()
{
 for(int i= 0; i<3; i++)
 {
  //delete(une_vue[i]);
 }
 close();
}

// Demande utilisateur du telechargement des bases
// depuis le site de la Francaise des jeux
void MainWindow::pslot_GetFromFdj()
{

 QAction *src_click =qobject_cast<QAction *>(sender());

 /// regarder si il existe le repertoire de reception des telechargements.
 QDir rep_fdj(".");
 ///QDir rep_fdj("C:\\Qt\\my-temp-dir\\");

 if (!rep_fdj.exists(FdjDbZip)) {
  rep_fdj.mkdir(FdjDbZip);
 }

QStringList urlFdJeux;
#ifndef QT_NO_DEBUG
 // https://www.programmersought.com/article/71774817627/
 qDebug()
     << QSslSocket::supportsSsl() // doit retourner true
     << QSslSocket::sslLibraryBuildVersionString() // la version utilise pour compiler Qt
     << QSslSocket::sslLibraryVersionString(); // la version disponible
#endif

 QString urlLoto []= {
  "https://media.fdj.fr/static/csv/loto/grandloto_201912.zip",
  "https://media.fdj.fr/static/csv/loto/loto_197605.zip",
  "https://media.fdj.fr/static/csv/loto/loto_200810.zip",
  "https://media.fdj.fr/static/csv/loto/loto_201703.zip",
  "https://media.fdj.fr/static/csv/loto/loto_201902.zip",
  "https://media.fdj.fr/static/csv/loto/loto_201911.zip",
  "https://media.fdj.fr/static/csv/loto/lotonoel_201703.zip",
  "https://media.fdj.fr/static/csv/loto/superloto_199605.zip",
  "https://media.fdj.fr/static/csv/loto/superloto_200810.zip",
  "https://media.fdj.fr/static/csv/loto/superloto_201703.zip",
  "https://media.fdj.fr/static/csv/loto/superloto_201907.zip",
 };

 QString urlEuro[] = {
  "https://media.fdj.fr/static/csv/euromillions/euromillions_202002.zip",
  "https://media.fdj.fr/static/csv/euromillions/euromillions_201902.zip",
  "https://media.fdj.fr/static/csv/euromillions/euromillions_201609.zip",
  "https://media.fdj.fr/static/csv/euromillions/euromillions_201402.zip",
  "https://media.fdj.fr/static/csv/euromillions/euromillions_201105.zip",
  "https://media.fdj.fr/static/csv/euromillions/euromillions_200402.zip"
 };

 /// A Ameliorer
 for (const QString &arg : urlLoto) {
  urlFdJeux << arg;
 }
 for (const QString &arg : urlEuro) {
  urlFdJeux << arg;
 }


 /// https://forum.qt.io/topic/95700/qsslsocket-tls-initialization-failed/29
 /// https://kb.firedaemon.com/support/solutions/articles/4000121705-openssl-3-1-3-0-and-1-1-1-binary-distributions-for-microsoft-windows
 qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();

 QApplication::setOverrideCursor(Qt::BusyCursor);
 src_click->setEnabled(false);
 // Create a QProgressDialog object to show the download progress
 QProgressDialog downloadProgress;
 //downloadProgress.setWindowModality(Qt::WindowModal);
 downloadProgress.setModal(true);
 downloadProgress.setLabelText("Downloading file...");
 downloadProgress.setMaximum(urlFdJeux.size());

 for (int loop = 0; loop < urlFdJeux.size(); loop++) {
  downloadProgress.setValue(loop);

  QString arg = urlFdJeux[loop];
  QUrl url = QUrl::fromEncoded(arg.toLocal8Bit());

  bool status = doDownload(rep_fdj, url);
  if(status == false){
   downloadProgress.setLabelText(QString("Skiped : %1...").arg(arg));
  }
  else{
   QString cur_file = rep_fdj.path() + "\\" + FdjDbZip + "\\" + QFileInfo(url.path()).fileName();
   downloadProgress.setLabelText(QString("Unziping : %1\n\nto -> %2 ..\n").arg(arg).arg(cur_file));
   status = do7zip(cur_file);
  }

  if(status == false){
   continue;
  }
 }
 QApplication::restoreOverrideCursor(); // end loop
 src_click->setEnabled(true);

}



// Debut de demande de telechargement
bool MainWindow::doDownload(const QDir &dir, const QUrl &url, bool isLoopin)
{
 /// On effectue un download si distant plus recent que existant.
 QString filename = dir.path() + "\\" + FdjDbZip + "\\" + QFileInfo(url.path()).fileName();
 QFile file(filename);

 // Create a QNetworkRequest object for the HTTPS URL
 QNetworkRequest request(url);

 // Set up the SSL configuration for the HTTPS request
 QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
 sslConfig.setProtocol(QSsl::TlsV1_2);
 request.setSslConfiguration(sslConfig);


 // Create a QNetworkAccessManager object to download the zip file
 QNetworkAccessManager nam;
 QNetworkReply *reply = nullptr;

 QDateTime remoteDate;
 QDateTime localDate = QFileInfo(file).lastModified();
 int content_length = -1;
 if((file.exists() == true) && (isLoopin == false)){
  /// Analyse date local / distant
  reply = nam.head(request);
 }
 else{
  reply = nam.get(request);
 }

#if 0
 // Create a QProgressDialog object to show the download progress
 QProgressDialog downloadProgress;
 downloadProgress.setWindowModality(Qt::WindowModal);
 downloadProgress.setLabelText("Downloading file...");

 // Connect the download progress signal to the QProgressDialog object
 QObject::connect(reply, &QNetworkReply::downloadProgress, [&downloadProgress, &url](qint64 bytesReceived, qint64 bytesTotal) {
  //downloadProgress.setRange(0,bytesTotal);
  downloadProgress.setMaximum(bytesTotal);
  downloadProgress.setValue(bytesReceived);
  downloadProgress.setLabelText(QString("Downloading %1...").arg(QFileInfo(url.path()).fileName()));
 });
#endif

 // Start the event loop to wait for the download to complete
 QEventLoop downloadLoop;
 QObject::connect(reply, &QNetworkReply::finished, &downloadLoop, &QEventLoop::quit);
 downloadLoop.exec();

 /// Analyse retour de la commande
 if (reply->error() != QNetworkReply::NoError){
  return false;
 }
 else{
  content_length = reply->header(QNetworkRequest::ContentLengthHeader).toInt();

  // C'est une demande HEAD
  if(reply->operation() == QNetworkAccessManager::HeadOperation){
   /// Verifier si distant plus recent que local
   remoteDate = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
   if(remoteDate > localDate){
    return doDownload(dir,url, true);
   }
  }
 }

 if ((reply->error() == QNetworkReply::NoError) && isHttpRedirect(reply)) {
  // https://www.meetingcpp.com/blog/items/http-and-https-in-qt.html
  QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

  if(redirect.isValid() && reply->url() != redirect)
  {
   if(redirect.isRelative()){
    redirect = reply->url().resolved(redirect);
   }
   return doDownload(dir, redirect, true);
  }
 }

 if ((reply->error() == QNetworkReply::NoError) &&
     (reply->operation() == QNetworkAccessManager::GetOperation)){
  // C'est un Get : sauver fichier
  /// Verifier si distant plus recent que local
  remoteDate = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
  if(remoteDate > localDate){
   return saveToDisk(filename, reply);
  }
 }
 else
 {
  qWarning() << "Failed to download zip file:" << reply->errorString();
  return false;
 }

 // Clean up the QNetworkReply object
 reply->deleteLater();
 return false;
}


// Analyse de la reponse a la demande de telechargement
void MainWindow::slot_replyFinished(QNetworkReply *reply)
{
 //BCompress fichier;

 QUrl url = reply->url();
 QString msg = "";

 QString filename = QFileInfo(url.path()).fileName();
 QString localFile = FdjDbZip+ "\\" + filename;
 int content_length = reply->header(QNetworkRequest::ContentLengthHeader).toInt();
 QDateTime remoteDate = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
 QDateTime localDate = QFileInfo(localFile).lastModified();

 if (reply->error()) {
  msg = url.toEncoded().constData()
        + QString(" echec ") + qPrintable(reply->errorString());

  QMessageBox::information(this, "slot_replyFinished", msg,QMessageBox::Yes);
  fprintf(stderr, "Download of %s failed: %s\n",
          url.toEncoded().constData(),
          qPrintable(reply->errorString()));
 }
 else {
  if (isHttpRedirect(reply)) {
   // https://www.meetingcpp.com/blog/items/http-and-https-in-qt.html
   QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

   if(redirect.isValid() && reply->url() != redirect)
   {
    if(redirect.isRelative())
     redirect = reply->url().resolved(redirect);
    QNetworkRequest req(redirect);
    QNetworkReply* reply = manager->get(req);
    currentDownloads.append(reply);
   }
   msg = "Request was redirected.\n";
   //QMessageBox::information(this, "slot_replyFinished", msg,QMessageBox::Yes);
  }
  else {// info fichier
   // C'est une demande HEAD
   if (reply->operation() == QNetworkAccessManager::HeadOperation){


    // effectuer get si serveur plus recent
    if(remoteDate > localDate){
     QNetworkReply *reply_2 = manager->get(QNetworkRequest(url));
     currentDownloads.append(reply_2);
    }
   }
   else{
    // C'est une demande GET
    if (reply->operation() == QNetworkAccessManager::GetOperation){
#if 0
     int content_length = reply->header(QNetworkRequest::ContentLengthHeader).toInt();
     QDateTime remoteDate = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
     QDateTime localDate = QFileInfo(localFile).lastModified();
#endif
     // effectuer get si serveur plus recent
     if(remoteDate > localDate){
      QString filename = saveFileName(url);

      if (saveToDisk(filename, reply)) {
       printf("Download of %s succeeded (saved to %s)\n",
              url.toEncoded().constData(), qPrintable(filename));

       /// decompression du ficher
       ///fichier.decompressFolder(filename,FdjDbUse);
      }
     }
    }
   }
  }
 }
}

#if 0
void MainWindow::slot_sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
 for (const QSslError &error : sslErrors)
  fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
 Q_UNUSED(sslErrors);
#endif
}
#endif

QString MainWindow::saveFileName(const QUrl &url)
{
 QString path = url.path();
 QString localFile = FdjDbZip+ "\\" + QFileInfo(url.path()).fileName();

 if (localFile.isEmpty())
  localFile = FdjDbZip+ "\\" + "download.zip";

 if (QFile::exists(localFile)) {

  if (QMessageBox::question(this, tr("HTTP"),
                            tr("There already exists a file called %1 in "
                               "the current directory. Overwrite?").arg(localFile),
                            QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
      == QMessageBox::No){

   //don't overwrite
   int i = 0;
   localFile += '.';
   while (QFile::exists(localFile + QString::number(i)))
    ++i;

   localFile += QString::number(i);
  }
  else
  {
   QFile::remove(localFile);
  }
 }

 return localFile;
}

bool MainWindow::saveToDisk(const QString &filename, QNetworkReply *data)
{
 QFile file(filename);
 if (!file.open(QIODevice::WriteOnly)) {
  fprintf(stderr, "Could not open %s for writing: %s\n",
          qPrintable(filename),
          qPrintable(file.errorString()));
  return false;
 }

 file.write(data->readAll());
 file.close();

 return true;
}

bool MainWindow::do7zip(QString fileCompressed)
{
 // Set the path to the downloaded zip file
 QString zipFilePath = fileCompressed;

 // Set the output directory for the extracted files
 QString outputDir = zipFilePath.section("\\",0,-2);

 // Construct the 7zip command to extract the zip file
 QString sevenZipPath = "C:/Program Files/7-Zip/7z.exe";
 QString command = QString("\"%1\" x \"%2\" -o\"%3\" -aoa").arg(sevenZipPath).arg(zipFilePath).arg(outputDir);

 // Create a QProcess object to execute the 7zip command
 QProcess process;
 process.start(command);
 if (!process.waitForFinished()) {
  qWarning() << "Failed to extract zip file:" << process.errorString();
  return false;
 }

 return true;
}

bool MainWindow::isHttpRedirect(QNetworkReply *reply)
{
 int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
 return statusCode == 301 || statusCode == 302 || statusCode == 303
   || statusCode == 305 || statusCode == 307 || statusCode == 308;
}


void MainWindow::pslot_about()
{
 QString msg = tr("Version : ") + (L1.at(0).split(",")).at(0)
               + tr("\nDate : ") + (L1.at(0).split(",")).at(1)
               + tr("\nRef : ") + (L1.at(0).split(",")).at(4);
 QMessageBox::about(this, tr("A propos de Prevision"),msg);
}

