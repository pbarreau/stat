#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

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
 QStringList urlFdJeux;
 manager = new QNetworkAccessManager(this);

 /// regarder si il existe le repertoire de reception des telechargements.
 QDir rep_fdj;
 //QString FdjDb = "FdjDb";

 if (!rep_fdj.exists(FdjDbZip)) {
  rep_fdj.mkdir(FdjDbZip);
 }

#ifndef QT_NO_DEBUG
 // https://www.programmersought.com/article/71774817627/
 qDebug()
     << QSslSocket::supportsSsl() // doit retourner true
     << QSslSocket::sslLibraryBuildVersionString() // la version utilise pour compiler Qt
     << QSslSocket::sslLibraryVersionString(); // la version disponible
#endif

#if 0
 QString urlLoto []= {
  "https://www.fdj.fr/generated/game/loto/loto2017.zip",
  "https://www.fdj.fr/generated/game/loto/nouveau_loto.zip",
  "https://www.fdj.fr/generated/game/loto/loto.zip",
  "https://www.fdj.fr/generated/game/loto/superloto2017.zip",
  "https://www.fdj.fr/generated/game/loto/nouveau_superloto.zip",
  "https://www.fdj.fr/generated/game/loto/superloto.zip",
  "https://www.fdj.fr/generated/game/loto/lotonoel2017.zip"
 };
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

 for (const QString &arg : urlLoto) {
  urlFdJeux << arg;
 }
#if 0
 QString urlEuro[] = {
  "https://www.fdj.fr/generated/game/euromillions/euromillions_4.zip",
  "https://www.fdj.fr/generated/game/euromillions/euromillions_3.zip",
  "https://www.fdj.fr/generated/game/euromillions/euromillions_2.zip",
  "https://www.fdj.fr/generated/game/euromillions/euromillions.zip"
 };
#endif
 QString urlEuro[] = {
  "https://media.fdj.fr/static/csv/euromillions/euromillions_202002.zip",
  "https://media.fdj.fr/static/csv/euromillions/euromillions_201902.zip",
  "https://media.fdj.fr/static/csv/euromillions/euromillions_201609.zip",
  "https://media.fdj.fr/static/csv/euromillions/euromillions_201402.zip",
  "https://media.fdj.fr/static/csv/euromillions/euromillions_201105.zip",
  "https://media.fdj.fr/static/csv/euromillions/euromillions_200402.zip"
 };

 for (const QString &arg : urlEuro) {
  urlFdJeux << arg;
 }


 connect(manager, SIGNAL(finished(QNetworkReply*)),
         this, SLOT(slot_replyFinished(QNetworkReply*)));

 for (const QString &arg : urlFdJeux) {
  QUrl url = QUrl::fromEncoded(arg.toLocal8Bit());
  doDownload(url);
 }
}

// Debut de demande de telechargement
void MainWindow::doDownload(const QUrl &url)
{
 QNetworkRequest request(url);
 QNetworkReply *m_reply;

 QString filename = QFileInfo(url.path()).fileName();
 QString localFile = FdjDbZip+ "\\" + filename;

 // Si fichier deja present telecharger le plus recent
 if (QFile::exists(localFile)) {
  // faire une demande head au serveur
  m_reply = manager->head(request);
  currentDownloads.append(m_reply);
 }
 else
 {
  // recuperer fichier
  m_reply = manager->get(request);
  ///connect(m_reply, SIGNAL(metaDataChanged()), this, SLOT(BSLOT_fileSize()));

  currentDownloads.append(m_reply);
 }

 /// Traitement asynchrone
}

#if 0
void MainWindow::BSLOT_fileSize()
{
 QNetworkReply *m_reply = qobject_cast<QNetworkReply *>(sender());
 QNetworkAccessManager *m_netmanager = m_reply->manager();

 qDebug() << "Content Length: " << m_reply->header(QNetworkRequest::ContentLengthHeader).toString();
 int taille_fichier = m_reply->header(QNetworkRequest::ContentLengthHeader).toInt();
 if(taille_fichier){
  connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(BSLOT_DownloadProgress(qint64,qint64)));

 }
 m_reply->deleteLater();
 m_netmanager->deleteLater();
}

void MainWindow::BSLOT_DownloadProgress(qint64 in_byte, qint64 tot_byte)
{

}
#endif

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

 currentDownloads.removeAll(reply);
 reply->deleteLater();

 if (currentDownloads.isEmpty()) {
  // all downloads finished
  msg = "Telechargement completement termine\n";
  QMessageBox::information(NULL, "slot_replyFinished", msg,QMessageBox::Yes);
  return;
  //QCoreApplication::instance()->quit();
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

