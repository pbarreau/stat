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

class QSslError;

using namespace std;

#include "mainwindow.h"
#include "choixjeux.h"

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

    QString urlLoto []= {
        "https://www.fdj.fr/generated/game/loto/loto2017.zip",
        "https://www.fdj.fr/generated/game/loto/nouveau_loto.zip",
        "https://www.fdj.fr/generated/game/loto/loto.zip",
        "https://www.fdj.fr/generated/game/loto/superloto2017.zip",
        "https://www.fdj.fr/generated/game/loto/nouveau_superloto.zip",
        "https://www.fdj.fr/generated/game/loto/superloto.zip",
        "https://www.fdj.fr/generated/game/loto/lotonoel2017.zip"
    };
    for (const QString &arg : urlLoto) {
        urlFdJeux << arg;
    }

    QString urlEuro[] = {
        "https://www.fdj.fr/generated/game/euromillions/euromillions_4.zip",
        "https://www.fdj.fr/generated/game/euromillions/euromillions_3.zip",
        "https://www.fdj.fr/generated/game/euromillions/euromillions_2.zip",
        "https://www.fdj.fr/generated/game/euromillions/euromillions.zip"
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
    QNetworkReply *reply;

    QString localFile = QFileInfo(url.path()).fileName();

    // Si fichier deja present telecharger le plus recent
    if (QFile::exists(localFile)) {
        // faire une demande head au serveur
        reply = manager->head(request);
        currentDownloads.append(reply);
    }
    else
    {
        // recuperer fichier
        reply = manager->get(request);
        currentDownloads.append(reply);
    }

    /// Traitement asynchrone
}

// Analyse de la reponse a la demande de telechargement
void MainWindow::slot_replyFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    QString msg = "";

    if (reply->error()) {
        msg = url.toEncoded().constData()
                + QString(" echec ") + qPrintable(reply->errorString());

        QMessageBox::information(this, "slot_replyFinished", msg,QMessageBox::Yes);
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
    } else {
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
            QMessageBox::information(this, "slot_replyFinished", msg,QMessageBox::Yes);
        } else {// info fichier


            // C'est une demande HEAD
            if (reply->operation() == QNetworkAccessManager::HeadOperation){
                QString filename = QFileInfo(url.path()).fileName();
                int content_length = reply->header(QNetworkRequest::ContentLengthHeader).toInt();
                QDateTime remoteDate = reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();
                QDateTime localDate = QFileInfo(filename).lastModified();
                // effectuer get si serveur plus recent
                if(remoteDate > localDate){
                    //QNetworkRequest request_2(url);
                    QNetworkReply *reply_2 = manager->get(QNetworkRequest(url));
                    //reply_2 = manager->get(request_2);
                    currentDownloads.append(reply_2);
                }
            }else{
                // C'est une demande GET
                if (reply->operation() == QNetworkAccessManager::GetOperation){
                    QString filename = saveFileName(url);

                    if (saveToDisk(filename, reply)) {
                        printf("Download of %s succeeded (saved to %s)\n",
                               url.toEncoded().constData(), qPrintable(filename));
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
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download.zip";

    if (QFile::exists(basename)) {

        if (QMessageBox::question(this, tr("HTTP"),
                                  tr("There already exists a file called %1 in "
                                     "the current directory. Overwrite?").arg(basename),
                                  QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
                == QMessageBox::No){

            //don't overwrite
            int i = 0;
            basename += '.';
            while (QFile::exists(basename + QString::number(i)))
                ++i;

            basename += QString::number(i);
        }
        else
        {
            QFile::remove(basename);
        }
    }

    return basename;
}

bool MainWindow::saveToDisk(const QString &filename, QIODevice *data)
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

